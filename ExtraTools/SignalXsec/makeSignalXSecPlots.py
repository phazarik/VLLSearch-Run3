import os
import pandas as pd
import matplotlib.pyplot as plt
from rich.console import Console

console = Console(highlight=False)
print = console.print

# ------------- config ---------------
STYLES = {
    "singlet": {
        "13":   {"color": "xkcd:orange", "linestyle": "-"},
        "13p6": {"color": "xkcd:red",    "linestyle": "-"},
    },
    "doublet": {
        "13":   {"color": "xkcd:blue",        "linestyle": "-"},
        "13p6": {"color": "xkcd:blue purple", "linestyle": "-"},
    },
    "doublet_EE": {
        "13":   {"color": "xkcd:blue", "linestyle": ":"},
        "13p6": {"color": "xkcd:blue", "linestyle": "--"},
    },
    "doublet_EN": {
        "13":   {"color": "xkcd:green", "linestyle": ":"},
        "13p6": {"color": "xkcd:green", "linestyle": "--"},
    },
    "doublet_NN": {
        "13":   {"color": "xkcd:neon purple", "linestyle": ":"},
        "13p6": {"color": "xkcd:neon purple", "linestyle": "--"},
    },
}

FILES = {
    ("singlet", "13"):  "values/sigma_svll_13TeV.txt",
    ("singlet", "13p6"):"values/sigma_svll_13.6TeV.txt",
    ("doublet", "13"):  "values/sigma_dvll_13TeV.txt",
    ("doublet", "13p6"):"values/sigma_dvll_13.6TeV.txt",
}


# -------------- main ---------------
def main():
    df = load_all()

    ## 1) 13 TeV: singlet vs doublet (total + components)
    make_plot(df,
        [
            ("singlet", "13", "total", "singlet"),
            ("doublet", "13", "total", "doublet"),
            ("doublet", "13", "EE", "doublet_EE"),
            ("doublet", "13", "EN", "doublet_EN"),
            ("doublet", "13", "NN", "doublet_NN"),
        ],
        legendheader = "13 TeV",
        outfile="singlet_vs_doublet_13TeV.png",
    )

    ## 2) 13.6 TeV: singlet vs doublet (total + components)
    make_plot(df,
        [
            ("singlet", "13p6", "total", "singlet"),
            ("doublet", "13p6", "total", "doublet"),
            ("doublet", "13p6", "EE", "doublet_EE"),
            ("doublet", "13p6", "EN", "doublet_EN"),
            ("doublet", "13p6", "NN", "doublet_NN"),
        ],
        legendheader = "13.6 TeV",
        outfile="singlet_vs_doublet_13p6TeV.png",
    )

    ## 3) Singlet: 13 vs 13.6 TeV
    make_plot(df,
        [
            ("singlet", "13",   "total", "singlet"),
            ("singlet", "13p6", "total", "singlet"),
        ],
        legendheader = "VLL singlet",
        outfile="singlet_13_vs_13p6.png",
        include_energy_in_label=True,
        xlim = (50, 1050),
        ylim = (10e-6, 10)
    )

    ## 4) Doublet: 13 vs 13.6 TeV (total + components)
    make_plot(df,
        [
            ("doublet", "13",   "total", "doublet"),
            ("doublet", "13p6", "total", "doublet"),
            ("doublet", "13",   "EE",    "doublet_EE"),
            ("doublet", "13",   "EN",    "doublet_EN"),
            ("doublet", "13",   "NN",    "doublet_NN"),
            ("doublet", "13p6", "EE",    "doublet_EE"),
            ("doublet", "13p6", "EN",    "doublet_EN"),
            ("doublet", "13p6", "NN",    "doublet_NN"),
        ],
        legendheader = "VLL doublet",
        outfile="doublet_13_vs_13p6.png",
        include_energy_in_label=True,
    )

# ------------- load cross-sections into df ---------------
def read_xsec(filename, model, energy):
    print(f"[cyan]Reading:[/cyan] {filename} (model={model}, energy={energy})")
    if not os.path.exists(filename):
        print(f"[red]Missing file:[/red] {filename}")
        return None

    df = pd.read_csv(filename, comment="#", sep=r"\s+", header=None)

    if model == "doublet":
        df.columns = ["Mass", "EE", "EN", "NN"]
        df = df.astype({"Mass": float, "EE": float, "EN": float, "NN": float})
        df["total"] = df["EE"] + df["EN"] + df["NN"]
    else:
        df.columns = ["Mass", "XS"]
        df = df.astype({"Mass": float, "XS": float})
        df["total"] = df["XS"]

    df["model"] = model
    df["energy"] = energy
    print(f"  -> loaded {len(df)} rows")
    return df

def load_all():
    parts = []
    for (model, energy), path in FILES.items():
        df = read_xsec(path, model, energy)
        if df is not None:
            parts.append(df)
    if not parts:
        raise SystemExit("No input files found.")
    big = pd.concat(parts, ignore_index=True)
    big = big.sort_values(["model","energy","Mass"]).reset_index(drop=True)
    print(f"[green]Combined dataframe rows:[/green] {len(big)}")
    return big

# ------------- main plotting function ---------------
def make_plot(df, selections, outfile="test.png",
              include_energy_in_label=False, legendheader=None, xlim = (50, 2050), ylim = (10e-8, 10e2)):

    os.makedirs("plots", exist_ok=True)
    fig, ax = plt.subplots(figsize=(6, 5))

    for model, energy, comp, style_key in selections:
        sub = df[(df["model"]==model) & (df["energy"]==energy)]
        if sub.empty:
            print(f"[red]Warning:[/red] no data for {model} {energy} {comp}, skipping")
            continue

        style = STYLES[style_key][energy]

        # --- construct legend label dynamically ---
        if include_energy_in_label:
            if comp == "total": label = "13 TeV" if energy == "13" else "13.6 TeV"
            else:               label = "_nolegend_"
        else:
            if style_key == "singlet":   label = "VLL singlet" if comp == "total" else "_nolegend_"
            elif style_key == "doublet": label = "VLL doublet" if comp == "total" else "_nolegend_"
            else: label = style.get("label", comp) if comp != "total" else style.get("label", comp)

        x = sub["Mass"].values
        y = sub[comp].values
        ax.plot(x, y, label=label, color=style["color"], linestyle=style["linestyle"])

    ax.set_xlim(xlim[0], xlim[1])
    ax.set_ylim(ylim[0], ylim[1])        
    ax.set_yscale("log")
    ax.set_xlabel("Mass [GeV]", fontsize=14)
    ax.set_ylabel("Cross section [pb]", fontsize=14)

    ax.text(0.02, 0.94, "CMS", transform=ax.transAxes, fontsize=25, fontweight="bold", va="center")

    handles, labels = ax.get_legend_handles_labels()
    if handles: ax.legend(fontsize=10, loc='upper right', frameon=False, title=legendheader, title_fontsize=14)

    fullname = os.path.join("plots", outfile)
    #plt.tight_layout()
    plt.savefig(fullname, dpi=150, bbox_inches='tight', pad_inches=0.1)
    plt.close(fig)
    print(f"[green]Saved:[/green] {fullname}")

# ------- EXECUTION --------    
if __name__ == "__main__": main()
