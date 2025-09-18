import os, json
import math

# ---------- Global variables --------------
campaign = "FullDataset"
channel  = "combined" 
basedir = "systematic_uncertainties"
# ------------------------------------------

jsonfile = os.path.join(basedir, f"{campaign}_{channel}.json")
with open(jsonfile) as f: data = json.load(f)

namedict = {
    "lep": "Lepton SF",
    "trig":"Trigger eff",
    "bjet": "b-tagging eff",
    "pileup":"PileUp weight",
    "dy":  "DY norm",
    "qcd":  "QCD norm",
    "ttbar": "tt+x norm",
    "jec": "JEC",
    "jer": "JER",
    "lumi": "Luminosity"
}

totals = {}
for syst, bins in data.items():
    if syst == "combined": continue

    if "global" in bins:
        global_val = bins["global"]
        up = global_val["systup"]
        down = global_val["systdown"]
    else:
        # Quadrature combination over bins
        up_sq = sum(((v["systup"] - 1)**2) for v in bins.values())
        down_sq = sum(((1 - v["systdown"])**2) for v in bins.values())
        up = 1 + math.sqrt(up_sq)
        down = 1 - math.sqrt(down_sq)
    totals[syst] = {
        "up": up,
        "down": down,
        "up_pct": abs(up - 1)*100,
        "down_pct": abs(1 - down)*100
    }

# Compute combined uncertainty in quadrature
combined_up_sq = sum((v["up"] - 1)**2 for v in totals.values())
combined_down_sq = sum((1 - v["down"])**2 for v in totals.values())
combined_up = 1 + math.sqrt(combined_up_sq)
combined_down = 1 - math.sqrt(combined_down_sq)
totals["combined"] = {
    "up": combined_up,
    "down": combined_down,
    "up_pct": (combined_up - 1)*100,
    "down_pct": (1 - combined_down)*100
}

# Optional: maximum envelope (less extreme than linear sum)
max_up = max(v["up"] for v in totals.values())
max_down = min(v["down"] for v in totals.values())
totals["envelope"] = {
    "up": max_up,
    "down": max_down,
    "up_pct": (max_up - 1)*100,
    "down_pct": (1 - max_down)*100
}

# Print results
print("\n"+"-"*32)
print(f"Systematic uncertainties")
print(f"{campaign}, {channel} (up/down):")
print("-"*32)
for syst, values in totals.items():
    name = namedict.get(syst, syst)
    print(f"{name:15}: {values['up_pct']:.2f}% / {values['down_pct']:.2f}%")
print("-"*32+"\n")
