import pandas as pd
import ctypes
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import os, ROOT
channelnames = {"mm": r"$\mu\mu$", "me": r"$\mu e$", "em": r"$e\mu$", "ee": r"$ee$", "combined": "combined"}

def main():
    finalstate = "2LOS"
    indir = f"yields/2025-10-13/{finalstate}_val1"
    outdir = f"pie/2025-10-13/{finalstate}_val1"
    tag = "val1"
    campaigndict = {
        "2016preVFP_UL":"2016-preVFP",
        "2016postVFP_UL":"2016-postVFP",
        "2017_UL":"2017",
        "2018_UL":"2018",
        "Run2":"Run-2",
        "Run3Summer22":"2022-preEE",
        "Run3Summer22EE":"2022-postEE",
        "Run3Summer23":"2023-preBPix",
        "Run3Summer23BPix":"2023-postBPix",
        "Run3":"2022+2023",
        "FullDataset":"Run-2+2022+2023",
    }
    channels = ["mm", "me", "em", "ee", "combined"]
    for campaign, name in campaigndict.items():
        if "2018" not in campaign: continue
        for ch in channels:
            ### Excpetion:
            if ch not in ["mm", "ee"]: continue
            #if campaign in ['Run2', 'Run3'] and ch != 'combined': continue
            csvfile = f"{indir}/yields_{finalstate}_{tag}_{campaign}_{ch}.csv"
            if not os.path.exists(csvfile):
                print(f"\033[31mWarning: Missing file {csvfile}\033[0m")
                continue
            print(f"Processing file: {csvfile}")
            plot_background_pie(csvfile, outdir, name, ch)

def get_rgb(ci):
    color = ROOT.gROOT.GetColor(ci)
    if not color: return (0.5, 0.5, 0.5)
    r, g, b = ctypes.c_float(), ctypes.c_float(), ctypes.c_float()
    color.GetRGB(r, g, b)
    return (r.value, g.value, b.value)

def plot_background_pie(csv_file, outdir, campaign, channel):
    os.makedirs(outdir, exist_ok=True)

    color_map = {
        "DY":        ROOT.kRed-7,
        "Higgs":     ROOT.kMagenta,
        "QCD":       ROOT.kYellow,
        "multi-top": ROOT.kCyan-2,
        "tX":        ROOT.kCyan-7,
        "Single t":  ROOT.kCyan-7,
        "t#bar{t}":  ROOT.kAzure+1,
        "t#bar{t}+x":ROOT.kAzure+1,
        "t#bar{t}V": ROOT.kAzure+2,
        "tW":        ROOT.kBlue-3,
        "VV":        ROOT.kGreen+1,
        "WW":        ROOT.kGreen+1,
        "WZ":        ROOT.kGreen+1,
        "ZZ":        ROOT.kGreen+1,
        "WWss":      ROOT.kGreen-5,
        "VVV":       ROOT.kGreen+3,
        "WWW":       ROOT.kGreen+3,
        "WWZ":       ROOT.kGreen+3,
        "WZZ":       ROOT.kGreen+3,
        "ZZZ":       ROOT.kGreen+3,
        "W#gamma":   ROOT.kGray+2,
        "W+jets":    ROOT.kGray+1,
        "W+jets/#gamma": ROOT.kGray+2,
        "Z#gamma":   ROOT.kRed-9,
    }

    latex_name_map = {
        "t#bar{t}":  r"$t\bar{t}$",
        "t#bar{t}+x":r"$t\bar{t}+x$",
        "t#bar{t}V": r"$t\bar{t}V$",
        "W#gamma":   r"$W\gamma$",
        "W+jets/#gamma": r"W+jets/$\gamma$",
        "Z#gamma":   r"$Z\gamma$",
    }

    df = pd.read_csv(csv_file)
    total_row = df[df["nbin"] == "Total"].iloc[0]

    cols = df.columns.tolist()
    start_idx = cols.index("TotalBkg") + 1
    sig_idx = [i for i, c in enumerate(cols) if c.startswith("VLLD")][0]
    bkg_cols = cols[start_idx:sig_idx]

    bkg_vals = [float(str(total_row[c]).split("±")[0].strip()) for c in bkg_cols]

    threshold = 0.05 * sum(bkg_vals)
    labels, values, colors, other = [], [], [], 0.0
    for col, val in zip(bkg_cols, bkg_vals):
        if val < threshold: 
            other += val
        else:
            labels.append(col)
            values.append(val)
            ci = color_map.get(col, ROOT.kGray+1)
            colors.append(get_rgb(ci))
    if other > 0:
        labels.append("Others")
        values.append(other)
        colors.append(mcolors.to_rgb("lightgrey"))

    latex_labels = [latex_name_map.get(lbl, lbl) for lbl in labels]

    plt.figure(figsize=(6,6))

    # Slightly increase explode proportionally to wedge size for uniformity
    total = sum(values)
    explode_vals = [0.03]*len(values)

    # Main pie
    wedges, texts, autotexts = plt.pie(
        values,
        labels=latex_labels,
        colors=colors,
        autopct="%1.1f%%",
        startangle=130,
        pctdistance=0.8,
        wedgeprops={"edgecolor": None, "linewidth": 1.2},
        explode=explode_vals
    )

    # Apply darkened edge individually
    for w, c in zip(wedges, colors):
        r, g, b = c
        w.set_edgecolor((r*0.6, g*0.6, b*0.6))

    for t in texts:
        t.set_fontsize(10)
        t.set_fontweight("medium")
    for at in autotexts:
        at.set_fontsize(9)
        at.set_color("black")

    # Lower the title slightly using y parameter
    plt.title(f"{campaign} ({channelnames[channel]} channel)", fontsize=14, y=1.03)
    plt.tight_layout()

    figname = f"{outdir}/{campaign}_{channel}.png"
    plt.savefig(figname, bbox_inches="tight", dpi=150, transparent=True)
    plt.close()
    print(f"\033[0;33mFile created: {figname}\033[0m")

if __name__ == "__main__": main()
