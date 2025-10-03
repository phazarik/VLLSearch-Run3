import os, argparse
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import ROOT

def rootrgb(root_color):
    r, g, b = 0.8, 0.8, 0.8
    col = ROOT.gROOT.GetColor(root_color)
    if col:
        r, g, b = col.GetRed(), col.GetGreen(), col.GetBlue()
        print(f"Extracted colors: r={r:.6f} g={g:.6f} b={b:6f}")
    else:
        print(f"\033[0m[Warning] No color found for {root_color}")
    return (r, g, b)

# ---------------- global map ----------------
decor = {
    "DY":        {"name": "Drell-Yan",   "color": rootrgb(ROOT.kRed-7)},
    "Higgs":     {"name": "Higgs",       "color": rootrgb(ROOT.kMagenta)},
    "QCD":       {"name": "QCD",         "color": rootrgb(ROOT.kYellow)},
    "multi-top": {"name": "multi-top",   "color": rootrgb(ROOT.kCyan-2)},
    "tX":        {"name": "tX",          "color": rootrgb(ROOT.kCyan-7)},
    "Single t":  {"name": "Single top",  "color": rootrgb(ROOT.kCyan-7)},
    "ttbar":     {"name": r"$t\bar{t}$", "color": rootrgb(ROOT.kAzure+1)},
    "t#bar{t}":  {"name": r"$t\bar{t}$", "color": rootrgb(ROOT.kAzure+1)},
    "t#bar{t}+x":{"name": r"$t\bar{t}$+X","color": rootrgb(ROOT.kAzure+1)},
    "t#bar{t}V":{"name": r"$t\bar{t}$V", "color": rootrgb(ROOT.kAzure+2)},
    "tW":        {"name": "tW",          "color": rootrgb(ROOT.kBlue-3)},
    "VV":        {"name": "VV",          "color": rootrgb(ROOT.kGreen+1)},
    "WW":        {"name": "WW",          "color": rootrgb(ROOT.kGreen+1)},
    "WZ":        {"name": "WZ",          "color": rootrgb(ROOT.kGreen+2)},
    "ZZ":        {"name": "ZZ",          "color": rootrgb(ROOT.kGreen+1)},
    "WWss":      {"name": "WWss",        "color": rootrgb(ROOT.kGreen-5)},
    "VVV":       {"name": "VVV",         "color": rootrgb(ROOT.kGreen+3)},
    "WWW":       {"name": "WWW",         "color": rootrgb(ROOT.kGreen+3)},
    "WWZ":       {"name": "WWZ",         "color": rootrgb(ROOT.kGreen+3)},
    "WZZ":       {"name": "WZZ",         "color": rootrgb(ROOT.kGreen+3)},
    "ZZZ":       {"name": "ZZZ",         "color": rootrgb(ROOT.kGreen+3)},
    "W#gamma":   {"name": r"W+$\gamma$", "color": rootrgb(ROOT.kGray+2)},
    "W+jets":    {"name": "W+jets",      "color": rootrgb(ROOT.kGray+1)},
    "W+jets/#gamma":{"name": r"W+jets/$\gamma$","color": rootrgb(ROOT.kGray+2)},
    "Z#gamma":   {"name": r"Z+$\gamma$", "color": rootrgb(ROOT.kRed-9)},
    "MisID":     {"name": "MisID",       "color": rootrgb(ROOT.kOrange+1)},
}

# ---------------- main ----------------
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--infile", required=False, help="Input CSV file")
    parser.add_argument("--indir",  required=False, help="Input folder with CSV files")
    parser.add_argument("--title",  required=False, default="", help="Plot title")
    args = parser.parse_args()

    if args.infile:   files = [args.infile]
    elif args.indir:  files = [os.path.join(args.indir, f) for f in os.listdir(args.indir) if f.endswith(".csv")]
    else:
        print("Error: Please provide --infile or --indir")
        return

    os.makedirs("figures/", exist_ok=True)

    for idx, infile in enumerate(files):
        print(f"\n\033[033m({(idx+1)}/{len(files)}) Processing {infile}\033[0m")
        title = "" if args.indir else args.title
        outfile = os.path.join("figures", os.path.basename(infile).replace(".csv", ".png"))
        values, colors, labels = prepare_pie_data(infile)
        make_pie(values, colors, labels, title, outfile)

# ---------------- utilities ----------------
def prepare_pie_data(csv_file):
    df = pd.read_csv(csv_file)
    #print("\n=== CSV Data ===")
    #print(df)
    #print("================\n")

    if len(df) == 1:
        values_row = df.iloc[0].to_dict()
        #print("\nSingle row detected:")
        #print(values_row)
    else:
        if "Total" in df.iloc[:,0].values:
            total_row = df[df.iloc[:,0] == "Total"].iloc[0]
            values_row = total_row.drop(df.columns[0]).to_dict()
            #print("\nTotal row found:")
            #print(values_row)
        else:
            values_row = df.drop(df.columns[0], axis=1).sum(numeric_only=True).to_dict()
            #print("\nSummed values row:")
            #print(values_row)

    # first, remove ignored processes
    filtered_row = {}
    for col, val in values_row.items():
        col_strip = col.strip()
        if col_strip in ["nData", "TotalBkg"]: continue
        if col_strip.startswith("VLL"): continue
        filtered_row[col_strip] = val

    # compute threshold using only filtered processes
    total_sum = 0.0
    for v in filtered_row.values():
        val_str = str(v).strip()
        val = 0.0 if val_str == "" else float(val_str.split("±")[0])
        total_sum += val
    threshold = 0.05 * total_sum
    #print(f"\nThreshold for 'Others': {threshold}")

    labels, values, colors, other = [], [], [], 0.0
    for col, val in filtered_row.items():
        val_str = str(val).strip()
        val = 0.0 if val_str == "" else float(val_str.split("±")[0])
        if val < threshold:
            other += val
        else:
            labels.append(col)
            values.append(val)
            colors.append(decor.get(col, {"color": (0.8,0.8,0.8)})["color"])

    if other > 0:
        labels.append("Others")
        values.append(other)
        colors.append(mcolors.to_rgb("lightgrey"))

    latex_labels = [decor.get(lbl, {"name": lbl})["name"] for lbl in labels]

    total = sum(values)
    print("\n"+"-"*30)
    print("Entries for pie chart:")
    for lbl, val in zip(latex_labels, values):
        print(f"- {lbl}: {val} ({val/total*100:.1f}%)")
    print("-"*30+"\n")

    values_percent = [v/total*100 for v in values]
    #print(f"colors = {colors}")
    return values_percent, colors, latex_labels

def make_pie(values, colors, labels, title, figname):
    plt.figure(figsize=(6,6))
    explode_vals = [0.03]*len(values)

    wedges, texts, autotexts = plt.pie(
        values,
        labels=labels,
        colors=colors,
        autopct="%1.1f%%",
        startangle=130,
        pctdistance=0.8,
        wedgeprops={"edgecolor": None, "linewidth": 1.2},
        explode=explode_vals
    )

    for w, c in zip(wedges, colors):
        r, g, b = c
        w.set_edgecolor((r*0.6, g*0.6, b*0.6))

    for t in texts:
        t.set_fontsize(10)
        t.set_fontweight("medium")
    for at in autotexts:
        at.set_fontsize(9)
        at.set_color("black")

    plt.title(title, fontsize=14, y=1.03)
    plt.tight_layout()
    plt.savefig(figname, bbox_inches="tight", dpi=150, transparent=True)
    plt.close()
    print(f"\nFile created: \033[93m{figname}\033[0m")

if __name__ == "__main__": main()
