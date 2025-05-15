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
from matplotlib.lines import Line2D

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
    plotObsExp(
        jsonfile = "jsons/ttbar_uncorrected.json",
        outfile  = "ttbar_uncorrected.png"
    )
    plotObsExp(
        jsonfile = "jsons/qcd_validation.json",
        outfile  = "qcd_validation.png"
    )
    plotObsExp(
        jsonfile = "jsons/signal_significance.json",
        outfile  = "signal_significance.png",
        name     = "S/sqrt{B}",
        signal   = True
    )

#____________________________________________________________________________________________________
#____________________________________________________________________________________________________

def plotObsExp(jsonfile, outfile, name="Obs/Exp", signal=False):

    #if 'qcd' not in jsonfile: return

    with open(jsonfile) as f: data = json.load(f)
    channels = [r'$\mu\mu$', r'$\mu e$', r'$e\mu$', r'$ee$']
    x = list(range(len(channels)))

    outdir = f'plots/'
    os.makedirs(outdir, exist_ok=True)

    fig_size = (6, 3)
    fig, ax = plt.subplots(figsize=fig_size)    
    if not signal: ax.axhspan(0.95, 1.05, color='green', alpha=0.2)

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
    if not signal: ax.axhline(1.0, color='black', linestyle=':', linewidth=1)
    ax.set_xlim(-0.5, len(channels) - 0.5 + x_offset)

    ymin, ymax = 0, 1.5
    if not signal:
        max_dev = max(abs(v - 1.0) for v in all_vals if v is not None)
        delta = max(max_dev * 1.2, 0.05)
        ymin, ymax = 1 - delta, 1 + delta
    ax.set_ylim(ymin, ymax)
    
    ncol = 1 if len([k for k in campaign_dict if k in data]) <= 5 else 2
    ax.legend(fontsize=8, frameon=False, ncol=ncol, loc='best')

    if not outfile.endswith('png'): outfile = f"{outfile}.png"
    fullname = os.path.join(outdir, outfile)
    fig.set_size_inches(fig_size, forward=True)
    fig.subplots_adjust(left=0.12, right=0.98, top=0.95, bottom=0.2)
    #plt.tight_layout()
    plt.savefig(fullname, dpi=150)
    print(f'Created: {fullname}')
    plt.close(fig)

if __name__=="__main__": main()
