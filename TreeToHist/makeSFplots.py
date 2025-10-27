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
#channel_map = {0: r'$\mu\mu$', 1: r'$\mu e$', 2: r'$e\mu$', 3: r'$ee$', 4: 'combined'}

#____________________________________________________________________________________________________
def main():
    plotGlobalCorrections(
        jsonfile="corrections/2L_QCD_global_corrections.json",
        name=r"QCD global SF",
        yrange = [0, 1.2],
        #moretext = "QCD control region",
        outfile="2L_qcd_SF_global.png"
    )
    plotCorrectionsBinned(
        jsonfile="corrections/2LOS_dycr_SF_inBins_LTplusMET.json",
        name=r"DY SF (binned in LT+MET)",
        yrange = [0, 2],
        #moretext = "DY control region",
        outfile="2LOS_dycr_SF_inBins_LTplusMET.png"
    )
    plotCorrectionsBinned(
        jsonfile="corrections/2LOS_topcr_SF_inBins_LTplusMET.json",
        name=r"$\mathrm{t\bar{t}+X\ SF\ (LT+MET\text{-}binned)}$",
        #moretext = r"$t\bar{t}+X$ control region",
        yrange = [0, 2],
        outfile="2LOS_topcr_SF_inBins_LTplusMET.png"
    )

#____________________________________________________________________________________________________
def plotGlobalCorrections(jsonfile, name, outfile, yrange=None, moretext=None):
    with open(jsonfile) as f: data = json.load(f)
    if not data: return

    channels_present = sorted({int(k) for v in data.values() for k in v.keys()})
    nch = len(channels_present)
    outdir = 'corrections/plots/'
    os.makedirs(outdir, exist_ok=True)

    x = list(range(len(channels_present)))
    fig_size = (2.5 * nch, 4)
    fig, ax = plt.subplots(figsize=fig_size)

    ## Reference line
    ax.axhline(1.0, color='gray', linestyle=':', linewidth=1)

    plotted_campaigns = set()
    x_offset_per_point = 0.08

    ## Smart per-channel offsets
    for i, ch_index in enumerate(channels_present):
        if ch_index not in channel_map: continue
        campaigns_present = [k for k in campaign_dict if k in data and str(ch_index) in data[k] and data[k][str(ch_index)] is not None]
        n_campaigns_present = len(campaigns_present)

        for j, key in enumerate(campaigns_present):
            val = data[key][str(ch_index)]
            if val is None or (val[0], val[1]) in [(0, 0), (1, 1)]: continue

            props = campaign_dict[key]
            x_offset = x[i] + (j - (n_campaigns_present - 1) / 2) * x_offset_per_point

            label = props["name"] if key not in plotted_campaigns else None
            ax.errorbar(
                x_offset, val[0], yerr=val[1],
                fmt=props["style"], color=props["color"],
                markersize=4, capsize=3, label=label
            )
            plotted_campaigns.add(key)

    ## X-axis
    ax.set_xticks(x)
    ax.set_xticklabels([channel_map[ch] for ch in channels_present], fontsize=10)

    ## Vertical separators
    for i in range(len(channels_present) - 1):
        ax.axvline(x[i] + 0.5, color='gray', linestyle='--', linewidth=0.5, alpha=0.5)

    ## Labels
    ax.set_ylabel(name, fontsize=12)
    ax.tick_params(axis='both', which='both', top=True, right=True)
    ax.text(0, 1.02, 'CMS', transform=ax.transAxes, fontsize=22, fontweight='bold',
            family='sans-serif', ha='left', va='bottom')
    if moretext:
        ax.text(1, 1.02, moretext, transform=ax.transAxes, fontsize=10,
                family='sans-serif', ha='right', va='bottom')
    if yrange: ax.set_ylim(yrange[0], yrange[1])

    ## Legend
    legend_items, legend_labels = ax.get_legend_handles_labels()
    nitems = len(plotted_campaigns)
    ncol = (nitems + 3) // 4
    ax.legend(legend_items, legend_labels, fontsize=8, ncol=ncol, loc='best',
              frameon=True, facecolor='white', framealpha=1.0, edgecolor='white')

    ## Save
    if not outfile.endswith('png'): outfile = f"{outfile}.png"
    fullname = os.path.join(outdir, outfile)
    fig.set_size_inches(fig_size, forward=True)
    fig.subplots_adjust(left=0.12, right=0.98, top=0.95, bottom=0.2)
    plt.savefig(fullname, dpi=150, bbox_inches='tight', pad_inches=0.1)
    print(f'Created: {fullname}')
    plt.close(fig)
    
#____________________________________________________________________________________________________
def plotCorrectionsBinned(jsonfile, name, outfile, maxval=500, yrange=None, moretext=None):
    with open(jsonfile) as f: data = json.load(f)
    if not data: return

    channels_present = sorted({int(k) for v in data.values() for k in v.keys() if int(k) in channel_map})
    nch = len(channels_present)
    outdir = 'corrections/plots/'
    os.makedirs(outdir, exist_ok=True)

    fig, ax = plt.subplots(figsize=(4 + 2*nch, 4))

    #========================
    ## Compute dynamic channel offsets
    ch_offsets = {}
    channel_maxedges = {}
    current_offset = 0
    padding = 20

    for ch_index in channels_present:
        if ch_index not in channel_map: continue
        max_edge = 0
        for key in campaign_dict:
            if key not in data: continue
            bins = data[key].get(str(ch_index), [])
            if not bins: continue
            for b in bins:
                if b["high"] != "inf":
                    high = float(b["high"])
                else:
                    continue
                if high > max_edge: max_edge = high
        if max_edge == 0: max_edge = 500

        for key in campaign_dict:
            if key not in data: continue
            for b in data[key].get(str(ch_index), []):
                if b["high"] == "inf":
                    b["high"] = str(max_edge)

        channel_maxedges[ch_index] = max_edge
        ch_offsets[ch_index] = current_offset
        current_offset += max_edge + padding

    #========================
    ## Plotting campaigns
    campaign_added = set()
    for key, props in campaign_dict.items():
        if key not in data: continue

        for ch_index in channels_present:
            if ch_index not in ch_offsets: continue
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

    #=====================================
    ## Channel labels centered + separator
    for ch_index in channels_present:
        if ch_index not in channel_map or ch_index not in ch_offsets: continue
        offset = ch_offsets[ch_index]
        center = offset + channel_maxedges[ch_index]/2
        ax.text(center, 1.05, channel_map[ch_index], ha='center', va='bottom',
                fontsize=10, transform=ax.get_xaxis_transform())

    for i in range(len(channels_present) - 1):
        if channels_present[i] not in ch_offsets or channels_present[i+1] not in ch_offsets: continue
        xsep = ch_offsets[channels_present[i]] + channel_maxedges[channels_present[i]] + padding/2
        ax.axvline(xsep, color='gray', linestyle='--', linewidth=0.5, alpha=0.5)

    #============================
    ## X-axis ticks for bin edges
    xticks = []
    xticklabels = []
    for ch_index in channels_present:
        if ch_index not in ch_offsets: continue
        offset = ch_offsets[ch_index]
        edges = []
        for key in campaign_dict:
            if key not in data: continue
            bins = data[key].get(str(ch_index), [])
            if not bins: continue
            for b in bins:
                edges.append(b["low"])
                edges.append(b["high"])
        ## remove duplicates while preserving order
        seen = set()
        edges_unique = []
        for e in edges:
            if e not in seen:
                edges_unique.append(e)
                seen.add(e)
        for e in edges_unique:
            if e == "inf":
                xticks.append(offset + maxval)
                xticklabels.append('∞')
            else:
                xticks.append(offset + float(e))
                xticklabels.append(str(int(float(e))))

    ax.set_xticks(xticks)
    ax.set_xticklabels(xticklabels, rotation=90, ha='center', fontsize=10)
                    
    #========================
    ## Y-axis limits
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
if __name__=="__main__": main()
