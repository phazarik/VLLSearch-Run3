import os, sys
import time
import argparse
import numpy as np
import uproot
import pandas as pd
from tqdm import tqdm
from rich.console import Console
import json
import matplotlib.pyplot as plt
from matplotlib.patches import Patch
import math

console = Console(highlight=False)
print = console.print

parser = argparse.ArgumentParser()
parser.add_argument('--test', action='store_true')
parser.add_argument('--dryrun', action='store_true')
args = parser.parse_args()
test = args.test
dryrun = args.dryrun

if test: print('[WARNING]: test mode', style="red")
if dryrun: print('[WARNING]: dryrun mode', style="red")

campaign_dict = {
    "2016preVFP_UL":    {"name": "2016-preVFP",      "color": "xkcd:royal blue",    "style": "o"},
    "2016postVFP_UL":   {"name": "2016-postVFP",     "color": "xkcd:deep sky blue", "style": "s"},
    "2017_UL":          {"name": "2017",             "color": "xkcd:teal",          "style": "^"},
    "2018_UL":          {"name": "2018",             "color": "green",              "style": "D"},
    "Run3Summer22":     {"name": "2022-preEE",       "color": "xkcd:purple",        "style": "o"},
    "Run3Summer22EE":   {"name": "2022-postEE",      "color": "xkcd:magenta",       "style": "s"},
    "Run3Summer23":     {"name": "2023-preBPix",     "color": "xkcd:burnt orange",  "style": "^"},
    "Run3Summer23BPix": {"name": "2023-postBPix",    "color": "xkcd:salmon",        "style": "D"},
    "Run2":             {"name": "Run-2 (combined)", "color": "xkcd:grey",          "style": "+"},
    "Run3":             {"name": "Run-3 (combined)", "color": "xkcd:slate grey",    "style": "+"},
    "FullDataset":      {"name": "Full dataset",     "color": "xkcd:black",         "style": "P"}
}

def main():
    plotObsExp(
        jsonfile = "jsons/2LOS_val1_obsbyexp.json",
        outfile  = "2LOS_VR1.png",
        name     = "Obs/exp",
        moretext = "2LOS VR1",
        syst     = False,
        yrange   = [0.6, 1.5]
    )
    plotObsExp(
        jsonfile = "jsons/2LOS_val2_obsbyexp.json",
        outfile  = "2LOS_VR2.png",
        name     = "Obs/exp",
        moretext = "2LOS VR2",
        syst     = False,
        yrange   = [0.6, 1.5]
    )
    plotObsExp(
        jsonfile = "jsons/2LOS_topcr_obsbyexp.json",
        outfile  = "2LOS_ttbarCR_uncorrected.png",
        name     = "Obs/exp",
        moretext = r"2LOS $t\bar{t}+X$ CR (uncorrected)",
        syst     = False,
        yrange   = [0.6, 1.5]
    )
    plotObsExp(
        jsonfile = "jsons/2LOS_dycr_obsbyexp.json",
        outfile  = "2LOS_dyCR_uncorrected.png",
        name     = "Obs/exp",
        moretext = r"2LOS DY CR (uncorrected)",
        syst     = False,
        yrange   = [0.6, 1.5]
    )
    plotObsExp(
        jsonfile = "jsons/2LSS_dycr_obsbyexp.json",
        outfile  = "2LSS_dyCR_uncorrected.png",
        name     = "Obs/exp",
        moretext = r"2LSS DY CR (uncorrected)",
        syst     = False,
        yrange   = [0.6, 1.5]
    )
    plotObsExp(
        jsonfile = "jsons/2LSS_val_obsbyexp.json",
        outfile  = "2LSS_VR.png",
        name     = "Obs/exp",
        moretext = r"2LSS VR",
        syst     = False,
        yrange   = [0.6, 1.5]
    )

#____________________________________________________________________________________________________
def plotObsExp(jsonfile, outfile, name="Obs/Exp", yrange=None, signal=False, moretext=None, syst=True):
    with open(jsonfile) as f: data = json.load(f)

    index_to_key = {0:'mm', 1:'me', 2:'em', 3:'ee', 4:'combined'}
    ch_string = {'mm':r'$\mu\mu$', "me":r'$\mu e$', "em":r'$e\mu$', "ee":r'$ee$', "combined":'Combined'}

    ## Determine channels with actual data
    channels_present = sorted({int(k) for v in data.values() for k in v.keys() if v[k] is not None})
    channels = [ch_string.get(index_to_key.get(i, ''), f"Ch-{i}") for i in channels_present]
    x = np.arange(len(channels))  # one slot per actual data channel

    ## Figure size
    fig_size = (2.5*len(channels), 4)
    fig, ax = plt.subplots(figsize=fig_size)

    ## Green 10% band
    if not signal:
        ax.axhspan(0.9, 1.1, color='green', alpha=0.1)
        ax.axhline(0.9, color='green', linestyle='dotted', linewidth=1, alpha=0.4)
        ax.axhline(1.1, color='green', linestyle='dotted', linewidth=1, alpha=0.4)
        ax.text(0.01, 1.12, "10%", color="green", fontsize=8, va="bottom", ha="left", transform=ax.get_yaxis_transform())

    x_offset_per_point = 0.08
    plotted_campaigns = set()

    ## Plot data with smart per-channel offsets
    for i, ch_index in enumerate(channels_present):
        campaigns_present = [k for k in campaign_dict if k in data and str(ch_index) in data[k] and data[k][str(ch_index)] is not None]
        n_campaigns_present = len(campaigns_present)

        for j, key in enumerate(campaigns_present):
            props = campaign_dict[key]
            val = data[key][str(ch_index)]
            y_val = val[0]
            y_err = val[1]

            x_offset = x[i] + (j - (n_campaigns_present - 1)/2) * x_offset_per_point

            ## Syst rectangles
            if syst:
                syst_up, syst_down = get_syst_err(key, index_to_key.get(ch_index, ""))
                rects = get_syst_rects([x_offset], [y_val], [syst_up], [syst_down], x_offset_per_point)
                for r in rects:
                    ax.add_patch(plt.Rectangle((r["x"], r["y"]), r["width"], r["height"],
                                               color='lightgray', alpha=1))

            ## Only label once per campaign
            label = props["name"] if key not in plotted_campaigns else None
            ax.errorbar(
                x_offset, y_val, yerr=y_err,
                capsize=3, markersize=4,
                fmt=props["style"], color=props["color"], label=label
            )
            plotted_campaigns.add(key)

    ## X-axis ticks and labels
    ax.set_xticks(x)
    ax.set_xticklabels(channels, fontsize=11)

    ## Vertical separators
    for i in range(len(channels) - 1):
        ax.axvline(x[i] + 0.5, color='gray', linestyle='--', linewidth=0.5, alpha=0.5)

    ## Axes labels and CMS text
    ax.set_ylabel(name, fontsize=12)
    ax.set_xlabel("Channel", fontsize=12)
    ax.tick_params(axis='both', which='both', top=True, right=True)
    ax.text(0, 1.02, 'CMS', transform=ax.transAxes, fontsize=22, fontweight='bold',
            family='sans-serif', ha='left', va='bottom')
    if moretext:
        ax.text(1, 1.02, moretext, transform=ax.transAxes, fontsize=10,
                family='sans-serif', ha='right', va='bottom')

    ## Reference line and y-range
    if not signal: ax.axhline(1.0, color='black', linestyle=':', linewidth=1)
    if yrange: ymin, ymax = yrange
    else: ymin, ymax = 0, 2
    ax.set_ylim(ymin, ymax)

    ## Legend
    legend_items, legend_labels = ax.get_legend_handles_labels()
    if syst:
        legend_items.append(Patch(facecolor='lightgray', edgecolor='lightgray', alpha=1, label="Syst. envelope"))
        legend_labels.append("Syst. envelope")
    nitems = len(plotted_campaigns)
    ncol = (nitems + 3) // 4
    ax.legend(legend_items, legend_labels, fontsize=8, ncol=ncol, loc='best',
              frameon=True, facecolor='white', framealpha=1.0, edgecolor='white')

    ## Save figure
    if not outfile.endswith('png'): outfile = f"{outfile}.png"
    fullname = os.path.join('plots', outfile)
    fig.set_size_inches(fig_size, forward=True)
    fig.subplots_adjust(left=0.12, right=0.98, top=0.95, bottom=0.2)
    plt.savefig(fullname, dpi=150, bbox_inches='tight', pad_inches=0.1)
    print(f'Created: {fullname}')
    plt.close(fig)

#____________________________________________________________________________________________________
def get_syst_err(campaign, channel):
    jsonfile = f"../../Systematics/systematic_uncertainties/{campaign}_{channel}.json"
    if not os.path.exists(jsonfile): return 0.0, 0.0
    with open(jsonfile) as f: data = json.load(f)
    if "combined" not in data or not data["combined"]: return 0.0, 0.0
    up_sq, down_sq = 0.0, 0.0
    for b, info in data["combined"].items():
        up_sq   += (info["systup"] - 1.0)**2
        down_sq += (info["systdown"] - 1.0)**2
    systup   = 1.0 + math.sqrt(up_sq)
    systdown = 1.0 + math.sqrt(down_sq)
    return systup-1, systdown-1

#____________________________________________________________________________________________________
def get_syst_rects(x_vals, y_vals, yerr_up, yerr_down, x_width, shrink=0.5):
    rects = []
    width = x_width * shrink
    for i in range(len(x_vals)):
        x_left = x_vals[i] - width/2
        y_bottom = y_vals[i] - yerr_down[i]
        height = y_vals[i] + yerr_up[i] - y_bottom
        rects.append({"x": x_left, "y": y_bottom, "width": width, "height": height})
    return rects

#____________________________________________________________________________________________________
if __name__=="__main__": main()
