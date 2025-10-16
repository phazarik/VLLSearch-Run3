import os
import argparse
import json
import matplotlib.pyplot as plt
from rich.console import Console

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

channel_map = {0: r'$\mu\mu$', 1: r'$\mu e$', 2: r'$e\mu$', 3: r'$ee$'}

#____________________________________________________________________________________________________
def main():
    plotCorrectionsBinned(
        jsonfile="corrections/2LOS_DY_Zptbinned_corrections.json",
        name=r"$\mathrm{DY\ SF\ (Z\ {p_T}\text{-}binned)}$",
        outfile="2LOS_DY_Zptbinned_corrections.png"
    )
    plotCorrectionsBinned(
        jsonfile="corrections/2LOS_TTBar_HTbinned_corrections.json",
        name=r"$\mathrm{t\bar{t}+X\ SF\ (H_T\text{-}binned)}$",
        outfile="2LOS_TTbar_HTbinned_corrections.png"
    )

#____________________________________________________________________________________________________
def plotGlobalCorrections(jsonfile, name, outfile, yrange=None, moretext=None):
    with open(jsonfile) as f: data = json.load(f)
    if not data: return

    channels_present = sorted({int(k) for v in data.values() for k in v.keys()})
    nch = len(channels_present)
    outdir = 'corrections/plots/'
    os.makedirs(outdir, exist_ok=True)

    spacing = 1.0  # normalized x-axis for global plots
    ch_offsets = {ch: i*spacing for i, ch in enumerate(channels_present)}

    fig, ax = plt.subplots(figsize=(6 if nch>2 else 4, 3))
    ax.axhline(1, color='gray', linestyle=':', linewidth=1)

    campaign_added = set()
    for key, props in campaign_dict.items():
        if key not in data: continue
        values = data[key]

        xvals, yvals, yerrs = [], [], []
        for ch_index in channels_present:
            val = values.get(str(ch_index))
            if val is None or (val[0], val[1]) in [(0,0),(1,1)]: continue

            xvals.append(ch_offsets[ch_index])
            yvals.append(val[0])
            yerrs.append(val[1])

        if not yvals: continue
        label = props["name"] if key not in campaign_added else ""
        ax.errorbar(xvals, yvals, yerr=yerrs, capsize=3, markersize=4,
                    fmt=props["style"], label=label, color=props["color"])
        campaign_added.add(key)

    for ch_index in channels_present:
        ax.text(ch_offsets[ch_index], 1.05, channel_map[ch_index], ha='center', va='bottom', transform=ax.transAxes, fontsize=10)

    ax.set_xticks([])
    ax.set_ylabel(name, fontsize=12)
    ax.tick_params(axis='both', which='both', top=True, right=True)
    ax.text(0, 1.02, 'CMS', transform=ax.transAxes, fontsize=22, fontweight='bold', family='sans-serif', ha='left', va='bottom')
    if moretext: ax.text(1, 1.02, moretext, transform=ax.transAxes, fontsize=10, family='sans-serif', ha='right', va='bottom')
    if yrange is not None: ax.set_ylim(yrange[0], yrange[1])

    handles, labels = ax.get_legend_handles_labels()
    ncol = 1 if len(labels) <= 4 else 2
    ax.legend(handles, labels, fontsize=8, frameon=False, ncol=ncol, loc='best')

    fullname = os.path.join(outdir, outfile if outfile.endswith('png') else f"{outfile}.png")
    plt.tight_layout()
    plt.savefig(fullname, dpi=150, bbox_inches='tight', pad_inches=0.1)
    print(f'Created: {fullname}')
    plt.close(fig)

#____________________________________________________________________________________________________
def plotCorrectionsBinned(jsonfile, name, outfile, maxval=500, yrange=None, moretext=None):
    with open(jsonfile) as f: data = json.load(f)
    if not data: return

    channels_present = sorted({int(k) for v in data.values() for k in v.keys()})
    nch = len(channels_present)
    outdir = 'corrections/plots/'
    os.makedirs(outdir, exist_ok=True)

    fig, ax = plt.subplots(figsize=(10 if nch>2 else 6, 4))

    #========================
    # Compute dynamic channel offsets
    ch_offsets = {}
    channel_maxedges = {}
    current_offset = 0
    padding = 20
    for ch_index in channels_present:
        max_edge = 0
        for key in campaign_dict:
            if key not in data: continue
            bins = data[key].get(str(ch_index), [])
            if not bins: continue
            for b in bins:
                high = float(b["high"]) if b["high"] != "inf" else maxval
                if high > max_edge: max_edge = high
        channel_maxedges[ch_index] = max_edge
        ch_offsets[ch_index] = current_offset
        current_offset += max_edge + padding

    #========================
    # Plotting campaigns
    campaign_added = set()
    for key, props in campaign_dict.items():
        if key not in data: continue

        for ch_index in channels_present:
            bins = data[key].get(str(ch_index), [])
            if not bins: continue

            xvals, yvals, xerrs, yerrs = [], [], [], []
            for b in bins:
                low = float(b["low"])
                high = float(b["high"]) if b["high"] != "inf" else maxval
                center = (low + high)/2
                xerr = (high - low)/2
                scale, err = b["scale"]
                if (scale, err) in [(0,0),(1,1)]: continue

                xvals.append(ch_offsets[ch_index] + center)
                yvals.append(scale)
                yerrs.append(abs(err))
                xerrs.append(xerr)

            if not xvals: continue
            label = props["name"] if key not in campaign_added else ""
            ax.errorbar(xvals, yvals, xerr=xerrs, yerr=yerrs, markersize=4, capsize=3,
                        fmt=props["style"], color=props["color"], label=label)
            campaign_added.add(key)

    #========================
    # Channel labels centered
    for ch_index in channels_present:
        offset = ch_offsets[ch_index]
        center = offset + channel_maxedges[ch_index]/2
        ax.text(center, 1.05, channel_map[ch_index], ha='center', va='bottom',
                fontsize=10, transform=ax.get_xaxis_transform())

    #========================
    # X-axis ticks for bin edges
    xticks = []
    xticklabels = []
    for ch_index in channels_present:
        offset = ch_offsets[ch_index]
        edges = set()
        for key in campaign_dict:
            if key not in data: continue
            bins = data[key].get(str(ch_index), [])
            if not bins: continue
            for b in bins:
                low = float(b["low"])
                high = float(b["high"]) if b["high"] != "inf" else maxval
                edges.add(low)
                edges.add(high)
        edges = sorted(edges)
        for e in edges:
            xticks.append(offset + e)
            xticklabels.append('∞' if e == maxval else str(int(e)))

    ax.set_xticks(xticks)
    ax.set_xticklabels(xticklabels, rotation=90, ha='center', fontsize=10)

    #========================
    # Y-axis limits
    if yrange: ymin, ymax = yrange
    else:
        scales = [b["scale"][0] for v in data.values() for ch in v.values() for b in ch]
        ymin, ymax = min(scales), max(scales)
    center = 1.0
    pad = max(abs(ymax-center), abs(center-ymin))*0.1
    ax.set_ylim(center - max(abs(center-ymin), abs(ymax-center)) - pad,
                center + max(abs(center-ymin), abs(ymax-center)) + pad)

    ax.axhline(1.0, color='black', linestyle=':', linewidth=1)
    ax.tick_params(axis='both', which='both', top=True, right=True)
    ax.text(0, 1.02, 'CMS', transform=ax.transAxes, fontsize=22, fontweight='bold',
            family='sans-serif', ha='left', va='bottom')
    if moretext: ax.text(1, 1.02, moretext, transform=ax.transAxes, fontsize=10,
                          family='sans-serif', ha='right', va='bottom')

    handles, labels = ax.get_legend_handles_labels()
    ncol = 1 if len(labels) <= 4 else 2
    ax.legend(handles, labels, fontsize=9, frameon=False, ncol=ncol, framealpha=1.0)

    fullname = os.path.join(outdir, outfile if outfile.endswith('png') else f"{outfile}.png")
    plt.tight_layout()
    plt.savefig(fullname, dpi=150, bbox_inches='tight', pad_inches=0.1)
    print(f'Created: {fullname}')
    plt.close(fig)

#____________________________________________________________________________________________________
if __name__=="__main__":
    main()
