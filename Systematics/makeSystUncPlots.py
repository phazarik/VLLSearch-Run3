import os, sys
import json
import matplotlib.pyplot as plt
import numpy as np
from rich.console import Console
console = Console(highlight=False)
print = console.print

import ROOT ## for color
def root_color(k):
    if not ROOT.gROOT.GetColor(k): ROOT.TColor(k) ## force create the color
    cobj = ROOT.gROOT.GetColor(k)
    return (cobj.GetRed(), cobj.GetGreen(), cobj.GetBlue())

# ---------- Global variables --------------
campaigns = [
    "2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
    "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix",
    "Run2", "Run3", "FullDataset"
]
channels = {
    "mm": r"$\mu\mu$",
    "me": r"$\mu e$",
    "em": r"$e\mu$",
    "ee": r"$ee$",
    "combined": "combined"
}
basedir = "systematic_uncertainties"
binedges = [0, 25, 50, 100, 200, 300, 400, 500]
plotdict = {
    "lep":      {"color": "xkcd:green",              "label": "Lepton SF"},
    "trig":     {"color": "xkcd:red",                "label": "Trigger eff"},
    "bjet":     {"color": "xkcd:blue",               "label": "b-tagging eff"},
    "pileup":   {"color": "xkcd:hot pink",           "label": "PileUp weight"},
    "dy":       {"color": root_color(ROOT.kRed-7),   "label": "DY norm"},
    "qcd":      {"color": root_color(ROOT.kOrange-3),"label": "QCD norm"},
    "ttbar":    {"color": root_color(ROOT.kAzure+1), "label": r"$t\bar{t}+x$ norm"},
    "jec":      {"color": "xkcd:purple",             "label": "JEC"},
    "jer":      {"color": "xkcd:light purple",       "label": "JER"},
    "lumi":     {"color": "xkcd:burnt orange",       "label": "Luminosity"},
}
campaign_dict = {
    "2016preVFP_UL":    {"name": "2016-preVFP",      "color": "xkcd:royal blue"},
    "2016postVFP_UL":   {"name": "2016-postVFP",     "color": "xkcd:deep sky blue"},
    "2017_UL":          {"name": "2017",             "color": "xkcd:teal"},
    "2018_UL":          {"name": "2018",             "color": "green"},
    "Run3Summer22":     {"name": "2022-preEE",       "color": "xkcd:purple"},
    "Run3Summer22EE":   {"name": "2022-postEE",      "color": "xkcd:magenta"},
    "Run3Summer23":     {"name": "2023-preBPix",     "color": "xkcd:burnt orange"},
    "Run3Summer23BPix": {"name": "2023-postBPix",    "color": "xkcd:salmon"},
    "Run2":             {"name": "Run-2 (combined)", "color": "xkcd:grey"},
    "Run3":             {"name": "Run-3 (combined)", "color": "xkcd:slate grey"},
    "FullDataset":      {"name": "Full dataset",     "color": "xkcd:black"}
}
xtitle = r"$L_{T}+p_{T}^{\text{  miss}}$ (GeV)"
# ------------------------------------------

def main():
    count = 0
    for camp in campaigns:
        for ch, chname in channels.items():
            if (camp=="Run2" or camp=="Run3") and ch != "combined": continue
            count += 1
            print(f"\n[yellow][{count}] processing {camp}, {ch} channel[/yellow]")
            make_plot(camp, ch)

# ------- Retrieve systematic uncertainty arrays --------
def get_syst_arrays(data, syst, nbins):
    d = data.get(syst)
    if not d: return None, None
    if "global" in d:
        up = np.full(nbins, d["global"]["systup"], dtype=float)
        down = np.full(nbins, d["global"]["systdown"], dtype=float)
        return up, down
    ## normal per-bin case (but be robust if some bins missing)
    up = np.empty(nbins, dtype=float)
    down = np.empty(nbins, dtype=float)
    ## prepare fallback (first available value)
    first_key = None
    for k in d.keys():
        first_key = k
        break
    for i in range(nbins):
        key = str(i+1)
        if key in d:
            up[i] = d[key]["systup"]
            down[i] = d[key]["systdown"]
        else:
            ## fallback to first available entry
            up[i] = d[first_key]["systup"]
            down[i] = d[first_key]["systdown"]
    return up, down

# ------- Calculate the total systematic uncertainty envelope --------
def get_envelope_arrays(data, nbins):
    """Compute the per-bin envelope as raw deviations from y=1"""
    up_sq = np.zeros(nbins)
    down_sq = np.zeros(nbins)
    
    for syst, vals in data.items():
        if syst == "combined": continue
        if "global" in vals:
            dev_up   = vals["global"]["systup"] - 1
            dev_down = vals["global"]["systdown"] - 1
            up_sq += dev_up**2
            down_sq += dev_down**2
        else:
            for i in range(nbins):
                key = str(i+1)
                if key in vals:
                    dev_up   = vals[key]["systup"] - 1
                    dev_down = vals[key]["systdown"] - 1
                    up_sq[i]   += dev_up**2
                    down_sq[i] += dev_down**2

    up  = np.sqrt(up_sq)
    down = np.sqrt(down_sq)
    return up, down

# ------- Generate systematic uncertainty plot --------
def make_plot(campaign, channel):
    jsonfile = os.path.join(basedir, f"{campaign}_{channel}.json")
    if not os.path.exists(jsonfile):
        print(f"[red]File not found:[/red] {jsonfile}")
        return

    with open(jsonfile) as f: data = json.load(f)

    x = np.array(binedges)
    nbins = len(x) - 1
    fig_size = (8, 3)
    fig, ax = plt.subplots(figsize=fig_size)

    # Envelope / combined band (raw deviations from y=1)
    env_up, env_down = get_envelope_arrays(data, nbins)
    y_up   = np.append(1.0 + env_up, (1.0 + env_up)[-1])
    y_down = np.append(1.0 - env_down, (1.0 - env_down)[-1])
    ax.fill_between(x, y_down, y_up, step='post', color='gray', alpha=0.4, label="Envelope")
    ax.step(x, y_up,   where='post', color='black', linewidth=1)
    ax.step(x, y_down, where='post', color='black', linewidth=1)

    ## hline ay y=1 as a reference
    ax.axhline(1.0, color='gray', linestyle='-', linewidth=1)

    for syst, props in plotdict.items():
        up_arr, down_arr = get_syst_arrays(data, syst, nbins)
        if up_arr is None: continue
        y_up = np.append(up_arr, up_arr[-1])
        y_down = np.append(2.0 - down_arr, (2.0 - down_arr)[-1])
        ax.step(x, y_up, where='post', color=props["color"], linestyle='-', label=f"{props['label']}")
        ax.step(x, y_down, where='post', color=props["color"], linestyle=(0, (1, 1)), label='_nolegend_')
    
    ax.set_xlabel(xtitle, fontsize=12)
    ax.set_ylabel(r"$1 + \delta$", fontsize=12)
    ax.set_ylim(0.6, 1.4)
    ax.tick_params(axis='both', which='both', top=True, right=True)
    ax.legend(fontsize=8, loc="best", frameon=True, facecolor='white', framealpha=0.5, edgecolor='white', ncol=3)

    ax.text(0.0, 1.02, 'CMS',
            transform=ax.transAxes, fontsize=16, fontweight='bold',
            ha='left', va='bottom')
    ax.text(0.10, 1.03, "Systematic Uncertainty",
            transform=ax.transAxes, fontsize=12, family='sans-serif',
            ha='left', va='bottom')
    ax.text(1.0, 1.02, f"{campaign_dict.get(campaign, {}).get('name', campaign)} ({channels[channel]})",
            transform=ax.transAxes, fontsize=12,
            ha='right', va='bottom')

    outdir = "plots"
    os.makedirs(outdir, exist_ok=True)
    outfile = os.path.join(outdir, f"{campaign}_{channel}.png")
    plt.savefig(outfile, dpi=150, bbox_inches='tight', pad_inches=0.1)
    print(f"Created: {outfile}")
    plt.close(fig)

# ------- EXECUTION --------
if __name__ == "__main__": main()
