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
    "2016preVFP_UL":    {"name": "2016-preVFP",  "color": "xkcd:royal blue"},
    "2016postVFP_UL":   {"name": "2016-postVFP", "color": "xkcd:deep sky blue"},
    "2017_UL":          {"name": "2017",         "color": "xkcd:medium green"},
    "2018_UL":          {"name": "2018",         "color": "xkcd:tomato"},
    "Run3Summer22":     {"name": "2022",     "color": "xkcd:purple"},
    "Run3Summer22EE":   {"name": "2022EE",   "color": "xkcd:magenta"},
    "Run3Summer23":     {"name": "2023",     "color": "xkcd:burnt orange"},
    "Run3Summer23BPix": {"name": "2023BPix", "color": "xkcd:teal"},
}

def main():

    plotCorrectionsBinned(
        jsonfile = "corrections/TTBar_HTbinned_corrections.json",
        name     = "TTBar SF (HT-binned)",
        outfile  = "TTBar_HTbinned_corrections.png"
    )
    plotCorrectionsBinned(
        jsonfile = "corrections/DY_Zptbinned_chargemisID_corrections.json",
        name     = "DY charge-misID SF (Zpt binned)",
        outfile  = "DY_Zptbinned_chargemisID_corrections.png"
    )
    plotCorrectionsBinned(
        jsonfile = "corrections/DY_Zptbinned_corrections.json",
        name     = "DY SF (Zpt binned)",
        outfile  = "DY_Zptbinned_corrections.png"
    )
    plotGlobalCorrections(
        jsonfile = "corrections/QCD_global_corrections.json",
        name     = "QCD SF",
        outfile  = "QCD_global_corrections.png"
    )
    
#____________________________________________________________________________________________________
#____________________________________________________________________________________________________

def plotGlobalCorrections(jsonfile, name, outfile):
    
    with open(jsonfile) as f: data = json.load(f)
    channels = [r'$\mu\mu$', r'$\mu e$', r'$e\mu$', r'$ee$']
    x = list(range(len(channels)))

    outdir = f'corrections/plots/'
    os.makedirs(outdir, exist_ok=True)

    fig, ax = plt.subplots(figsize=(6, 3))
    all_vals = []
    x_offset = 0.1
    for index, (key, props) in enumerate(campaign_dict.items()):
        #if 'Run3' in key: continue
        if key not in data: continue
        values = data[key]
        
        y, yerr = [], []
        for i in range(4):
            val = values.get(str(i), None)
            if val is not None:
                y.append(val[0])
                yerr.append(val[1])
                all_vals.append(val[0])
            else:
                y.append(None)
                yerr.append(0)

        x_offset_values = []
        for i in range(len(channels)): x_offset_values.append(x[i] + x_offset * (index))
        ax.errorbar(x_offset_values, y, yerr=yerr, fmt='o', label=props["name"], color=props["color"], capsize=3)

    ax.set_xticks(x)
    ax.set_xticklabels(channels, fontsize=11)
    ax.set_ylabel(name, fontsize=12)
    ax.set_xlabel("Channel", fontsize=12)
    ax.tick_params(axis='both', which='both', top=True, right=True)
    ax.text(0.03, 0.86, 'CMS', transform=ax.transAxes, fontsize=22, fontweight='bold', family='sans-serif')
    ax.set_xlim(-1.0, len(channels) - 0.5 + x_offset)
    
    handles, labels = ax.get_legend_handles_labels()
    ncol = 1 if len(labels) <= 4 else 2
    ax.legend(handles, labels, fontsize=8, frameon=False, ncol=ncol, loc='lower left')

    if not outfile.endswith('png'): outfile = f"{outfile}.png"
    fullname = os.path.join(outdir, outfile)
    plt.tight_layout()
    plt.savefig(fullname, dpi=150)
    print(f'Created: {fullname}')
    plt.close(fig)

#____________________________________________________________________________________________________
#____________________________________________________________________________________________________

def plotCorrectionsBinned(jsonfile, name, outfile, maxval=500):

    if 'TTBar' not in jsonfile: return
    
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
            ax.errorbar(xvals, yvals, xerr=xerrs, yerr=yerrs, fmt='o', capsize=3,
                        color=props["color"], label=label)
            added_label = True

            if xvals:
                xmid = (min(xvals) + max(xvals)) / 2
                ax.text(xmid, 1.05, channels[i], ha='center', va='bottom', fontsize=10, transform=ax.get_xaxis_transform())

    xticks = sorted(set(xticks))
    xticklabels = [str(int(x % spacing)) if x % spacing != maxval else '∞' for x in xticks]

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
    ax.legend(handles, labels, fontsize=9, frameon=True, ncol=ncol)
    ax.tick_params(axis='both', which='both', top=True, right=True)
    ax.text(0.015, 0.89, 'CMS', transform=ax.transAxes, fontsize=22, fontweight='bold', family='sans-serif')

    if not outfile.endswith('png'): outfile = f"{outfile}.png"
    fullname = os.path.join(outdir, outfile)
    plt.tight_layout()
    plt.savefig(fullname, dpi=150)
    print(f'Created: {fullname}')
    plt.close(fig)
    
if __name__=="__main__": main()
