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
    "2016preVFP_UL":    {"name": "2016-preVFP",   "color": "xkcd:royal blue",    "style": "o"},
    "2016postVFP_UL":   {"name": "2016-postVFP",  "color": "xkcd:deep sky blue", "style": "s"},
    "2017_UL":          {"name": "2017",          "color": "xkcd:teal",          "style": "^"},
    "2018_UL":          {"name": "2018",          "color": "green",              "style": "D"},
    "Run3Summer22":     {"name": "2022-preEE",    "color": "xkcd:purple",        "style": "o"},
    "Run3Summer22EE":   {"name": "2022-postEE",   "color": "xkcd:magenta",       "style": "s"},
    "Run3Summer23":     {"name": "2023-preBPix",  "color": "xkcd:burnt orange",  "style": "^"},
    "Run3Summer23BPix": {"name": "2023-postBPix", "color": "xkcd:salmon",        "style": "D"},
}

def main():

    plotCorrectionsBinned(
        jsonfile = "corrections/TTBar_HTbinned_corrections.json",
        name     = r"$\mathrm{t\bar{t}{+}X\ SF\ (H_T\text{-}binned)}$",
        outfile  = "TTBar_HTbinned_corrections.png",
        yrange   = [0, 2]
    )
    plotCorrectionsBinned(
        jsonfile = "corrections/DY_Zptbinned_chargemisID_corrections.json",
        name     = r"$\mathrm{DY\ charge\text{-}misID\ SF\ (Z\ {p_T}\text{-}binned)}$",
        outfile  = "DY_Zptbinned_chargemisID_corrections.png"
    )
    plotCorrectionsBinned(
        jsonfile = "corrections/DY_Zptbinned_corrections.json",
        name     = r"$\mathrm{DY\ SF\ (Z\ {p_T}\text{-}binned)}$",
        outfile  = "DY_Zptbinned_corrections.png"
    )
    plotGlobalCorrections(
        jsonfile = "corrections/QCD_global_corrections.json",
        name     = "QCD SF",
        outfile  = "QCD_global_corrections.png",
        yrange   = [0, 1.2]
    )
    plotGlobalCorrections(
        jsonfile = "corrections/Wjets_global_corrections.json",
        name     = r"$\mathrm{W{+}jets/\gamma\ SF}$",
        outfile  = "Wjets_global_corrections.png",
        yrange   = [0, 2]
    )
    
#____________________________________________________________________________________________________
#____________________________________________________________________________________________________

def plotGlobalCorrections(jsonfile, name, outfile, yrange=None, moretext=None):
    
    with open(jsonfile) as f: data = json.load(f)
    channels = [r'$\mu\mu$', r'$\mu e$', r'$e\mu$', r'$ee$']
    x = list(range(len(channels)))

    outdir = f'corrections/plots/'
    os.makedirs(outdir, exist_ok=True)

    fig, ax = plt.subplots(figsize=(6, 3))
    ax.axhline(1, color='gray', linestyle=':', linewidth=1)
    all_vals = []
    x_offset = 0.1
    for index, (key, props) in enumerate(campaign_dict.items()):
        if key not in data: continue
        values = data[key]

        x_offset_values, y, yerr = [], [], []
        for i in range(len(channels)):
            val = values.get(str(i))
            if val is None: continue
            x_offset_values.append(x[i] + x_offset * index)
            y.append(val[0])
            yerr.append(val[1])
            all_vals.append(val[0])

        if not y: continue
        ax.errorbar(
            x_offset_values, y, yerr=yerr, capsize=3, markersize=4,
            fmt=props["style"], label=props["name"], color=props["color"]
        )
        
    ax.set_xticks(x)
    ax.set_xticklabels(channels, fontsize=11)
    ax.set_ylabel(name, fontsize=12)
    ax.set_xlabel("Channel", fontsize=12)
    ax.tick_params(axis='both', which='both', top=True, right=True)
    #ax.text(0.03, 0.86, 'CMS', transform=ax.transAxes, fontsize=22, fontweight='bold', family='sans-serif')
    ax.text(0, 1.02, 'CMS', transform=ax.transAxes, fontsize=22, fontweight='bold', family='sans-serif', ha='left', va='bottom')
    if moretext: ax.text(1, 1.02, moretext, transform=ax.transAxes, fontsize=10, family='sans-serif', ha='right', va='bottom')
    ax.set_xlim(-0.5, len(channels) + x_offset)
    #ax.set_ylim(0, 1)
    if yrange is not None: ax.set_ylim(yrange[0], yrange[1])
    
    handles, labels = ax.get_legend_handles_labels()
    ncol = 1 if len(labels) <= 4 else 2
    ax.legend(handles, labels, fontsize=8, frameon=False, ncol=ncol, loc='best')

    if not outfile.endswith('png'): outfile = f"{outfile}.png"
    fullname = os.path.join(outdir, outfile)
    plt.tight_layout()
    plt.savefig(fullname, dpi=150, bbox_inches='tight', pad_inches=0.1)
    print(f'Created: {fullname}')
    plt.close(fig)
    #plt.show()
#____________________________________________________________________________________________________
#____________________________________________________________________________________________________

def plotCorrectionsBinned(jsonfile, name, outfile, maxval=500, yrange=None, moretext=None):

    #if 'TTBar' not in jsonfile: return
    
    with open(jsonfile) as f: data = json.load(f)
    channels = [r'$\mu\mu$', r'$\mu e$', r'$e\mu$', r'$ee$']
    nch = len(channels)

    outdir = 'corrections/plots/'
    os.makedirs(outdir, exist_ok=True)

    fig, ax = plt.subplots(figsize=(10, 4))

    xticks = []
    xticklabels = []
    spacing = maxval + 40
    active_offsets = []

    for key, props in campaign_dict.items():
        #if 'Run3' in key: continue
        if key not in data: continue

        added_label = False

        for i in range(nch):
            bins = data[key].get(str(i), [])
            if not bins: continue

            offset = i * spacing
            if offset not in active_offsets: active_offsets.append(offset)

            xvals = []
            yvals = []
            yerrs = []
            xerrs = []

            for b in bins:
                low = float(b["low"])
                high = maxval if b["high"] == "inf" else float(b["high"])
                center = (low + high) / 2
                xerr = (high - low) / 2
                scale, err = b["scale"]

                x = offset + center
                xvals.append(x)
                yvals.append(scale)
                yerrs.append(err)
                xerrs.append(xerr)

                xticks.append(offset + low)
                xticks.append(offset + high)

            label = props["name"] if not added_label else ""
            ax.errorbar(xvals, yvals, xerr=xerrs, yerr=yerrs, markersize=4, capsize=3,
                        fmt=props["style"], color=props["color"], label=label)
            added_label = True

            if xvals:
                xmid = (min(xvals) + max(xvals)) / 2
                ax.text(xmid, 1.05, channels[i], ha='center', va='bottom', fontsize=10, transform=ax.get_xaxis_transform())

    xticks = sorted(set(xticks))
    xticklabels = [str(int(x % spacing)) if x % spacing != maxval else '∞' for x in xticks]

    if yrange: ymin, ymax =yrange[0], yrange[1]
    else:
        ymin = min(b["scale"][0] for v in data.values() for ch in v.values() for b in ch)
        ymax = max(b["scale"][0] for v in data.values() for ch in v.values() for b in ch)
    center = 1.0
    max_deviation = max(abs(center - ymin), abs(ymax - center))
    pad = max_deviation * 0.1
    ymin = center - max_deviation - pad
    ymax = center + max_deviation + pad

    ax.axhline(1.0, color='black', linestyle=':', linewidth=1)

    for i in range(1, nch):
        left = (i - 1) * spacing
        right = i * spacing
        if left in active_offsets and right in active_offsets:
            ax.axvline(right, color='gray', linestyle='--', linewidth=0.8, alpha=0.3)

    ax.set_xticks(xticks)
    ax.set_xticklabels(xticklabels, rotation=90, ha='center', fontsize=10)
    ax.set_ylabel(name, fontsize=12)
    ax.set_ylim(ymin, ymax)

    handles, labels = ax.get_legend_handles_labels()
    ncol = 1 if len(labels) <= 4 else 2
    ax.legend(handles, labels, fontsize=9, frameon=False, ncol=ncol, framealpha=1.0)
    ax.tick_params(axis='both', which='both', top=True, right=True)
    #ax.text(0.015, 0.89, 'CMS', transform=ax.transAxes, fontsize=22, fontweight='bold', family='sans-serif')
    ax.text(0, 1.02, 'CMS', transform=ax.transAxes, fontsize=22, fontweight='bold', family='sans-serif', ha='left', va='bottom')
    if moretext: ax.text(1, 1.02, moretext, transform=ax.transAxes, fontsize=10, family='sans-serif', ha='right', va='bottom')

    if not outfile.endswith('png'): outfile = f"{outfile}.png"
    fullname = os.path.join(outdir, outfile)
    plt.tight_layout()
    plt.savefig(fullname, dpi=150, bbox_inches='tight', pad_inches=0.1)
    print(f'Created: {fullname}')
    plt.close(fig)
    
if __name__=="__main__": main()
