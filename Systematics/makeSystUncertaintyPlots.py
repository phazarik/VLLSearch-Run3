import os
import json
import matplotlib.pyplot as plt
import numpy as np
from rich.console import Console
console = Console(highlight=False)
print = console.print

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
    "lep":      {"color": "blue",   "label": "leptop SF"},
    "trig":     {"color": "red",    "label": "trigger eff"},
    "lumi":     {"color": "orange", "label": "luminosity"},
    "combined": {"color": "black",  "label": "combined"}
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

def make_plot(campaign, channel):
    jsonfile = os.path.join(basedir, f"{campaign}_{channel}.json")
    if not os.path.exists(jsonfile):
        print(f"[red]File not found:[/red] {jsonfile}")
        return

    with open(jsonfile) as f: data = json.load(f)

    x = np.array(binedges)
    fig_size = (8, 3)
    fig, ax = plt.subplots(figsize=fig_size)

    ## Draw combine gray band first
    y_up = np.array([data["combined"][str(i+1)]["systup"] for i in range(len(x)-1)])
    y_down = np.array([2 - data["combined"][str(i+1)]["systdown"] for i in range(len(x)-1)])
    y_up = np.append(y_up, y_up[-1])
    y_down = np.append(y_down, y_down[-1])
    ax.fill_between(x, y_down, y_up, step='post', color='gray', alpha=0.3)

    ## Draw central reference line y=1
    ax.axhline(1.0, color='gray', linestyle='-', linewidth=1)

    ## Draw the rest
    for syst, props in plotdict.items():
        y_up = np.array([data[syst][str(i+1)]["systup"] for i in range(len(x)-1)])
        y_down = np.array([2 - data[syst][str(i+1)]["systdown"] for i in range(len(x)-1)])
        y_up = np.append(y_up, y_up[-1])
        y_down = np.append(y_down, y_down[-1])
        ax.step(x, y_up, where='post', color=props["color"], linestyle='-', label=f"{props['label']}")
        ax.step(x, y_down, where='post', color=props["color"], linestyle='--', label='_nolegend_') #only up labels

    ## Decorations
    ax.set_xlabel(xtitle, fontsize=12)
    ax.set_ylabel(r"$1 + \delta$", fontsize=12)
    #ax.set_xlim(0, 500)
    ax.set_ylim(0.85, 1.15)
    ax.tick_params(axis='both', which='both', top=True, right=True)
    ax.legend(fontsize=8, frameon=True, facecolor='white', framealpha=1.0, edgecolor='white', ncol=2)
    
    ## Additional texts
    ax.text(0.0, 1.02, 'CMS',
            transform=ax.transAxes, fontsize=16, fontweight='bold',
            ha='left', va='bottom')
    ax.text(0.10, 1.03, "Systematic Uncertainty",
            transform=ax.transAxes, fontsize=10, family='sans-serif',
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

def main():
    count = 0
    for camp in campaigns:
        for ch, chname in channels.items():
            if (camp=="Run2" or camp=="Run3") and ch != "combined": continue
            count += 1
            print(f"\n[yellow][{count}] processing {camp}, {ch} channel[/yellow]")
            make_plot(camp, ch)
            #break ## channel
        #break ##campaign

if __name__ == "__main__": main()
