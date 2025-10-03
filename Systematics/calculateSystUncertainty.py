import os
import pandas as pd
import json
import math

# ---------- Global variables --------------
campaigns = [
    "2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
    "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix",
    "Run2", "Run3", "FullDataset"
]

channels  = ["mm", "me", "em", "ee", "combined"]
basedir = "yields/2025-09-25/" ## Assuming everything to be in the same directory
tag = "sr"
# ------------------------------------------

def main():
    count = 0
    for camp in campaigns:
        for ch in channels:
            ## Exception:
            if (camp=="Run2" or camp=="Run3") and ch != "combined": continue
            count +=1
            print("\n"+"-"*50+f"\n\033[33m[{count}] processing {camp}, {ch} channel\033[0m\n"+"-"*50)
            process(camp, ch)
            #break #channel
        #break#campaign
    print("\nDone!")

# ------------------ combining logic ---------------------
def combine_syst(systematics):
    ## Find first bin-by-bin systematic
    first_key = None
    for k in systematics:
        if k != "combined" and any(str(b).isdigit() for b in systematics[k].keys()):
            first_key = k
            break
    if first_key is None: return
    
    bins = [b for b in systematics[first_key].keys() if str(b).isdigit()]
    systematics["combined"] = {}
    
    for b in bins:
        up_sq = 0
        down_sq = 0
        for s in systematics:
            if s == "combined": continue
            ## Global systematics
            if "global" in systematics[s]:
                syst_val_up = systematics[s]["global"]["systup"] - 1
                syst_val_down = 1 - systematics[s]["global"]["systdown"]
                up_sq += syst_val_up ** 2
                down_sq += syst_val_down ** 2
            ## Bin-by-bin systematics
            elif b in systematics[s]:
                syst_val_up = systematics[s][b]["systup"] - 1
                syst_val_down = 1 - systematics[s][b]["systdown"]
                up_sq += syst_val_up ** 2
                down_sq += syst_val_down ** 2
        
        ## Ensure combined factors are >0 and percentages are positive
        systup_comb = 1 + math.sqrt(up_sq)
        systdown_comb = 1 - math.sqrt(down_sq)
        systematics["combined"][b] = {
            "systup": systup_comb,
            "systdown": systdown_comb
        }

# ------------------ syst calculation --------------------
def process(campaign, channel):
    csvfile_nom = os.path.join(basedir, tag, f"yields_{tag}_{campaign}_{channel}.csv")
    df_nom = csv_into_df(csvfile_nom)
    if df_nom is None: return

    ## Initializing the dictionary
    systematics = {"lep":{}, "trig":{}, "pileup":{}, "bjet":{},
                   "dy":{}, "qcd":{}, "ttbar":{}, "jec":{}, "jer":{}} ## Sensitive to naming conventions

    ## Extract nominal yields for each bin
    nom_central = extract_central(df_nom).get("TotalBkg", pd.Series([0]*len(df_nom)))
    nominal_yields = {}
    
    for idx, nbin in enumerate(df_nom["nbin"]):
        if str(nbin).lower() == "total": continue
        nominal_yields[int(nbin)] = float(nom_central.iloc[idx])

    for syst in systematics.keys():
        tagup = f"{tag}_{syst}-systup"
        csvfile_up = os.path.join(basedir, tagup, f"yields_{tagup}_{campaign}_{channel}.csv")
        df_up = csv_into_df(csvfile_up)
        if df_up is None: continue

        tagdown = f"{tag}_{syst}-systdown"
        csvfile_down = os.path.join(basedir, tagdown, f"yields_{tagdown}_{campaign}_{channel}.csv")
        df_down = csv_into_df(csvfile_down)
        if df_down is None: continue

        up_central    = extract_central(df_up).get("TotalBkg", pd.Series([0]*len(df_nom)))
        down_central  = extract_central(df_down).get("TotalBkg", pd.Series([0]*len(df_nom)))

        ## safe fractions
        den = nom_central.replace(0, float("inf"))
        syst_up_fac   = 1 + (up_central - nom_central) / den
        syst_down_fac = 1 + (nom_central - down_central) / den

        ## Handle bins with zero nominal separately
        syst_up_fac   = syst_up_fac.where(nom_central != 0, 1 + up_central)
        syst_down_fac = syst_down_fac.where(nom_central != 0, 1 + down_central)

        ## Clean NaNs just in case
        syst_up_fac   = syst_up_fac.fillna(1)
        syst_down_fac = syst_down_fac.fillna(1)

        for idx, nbin in enumerate(df_nom["nbin"]):
            if str(nbin).lower() == "total": continue
            bin_num = int(nbin)
            up_val = float(syst_up_fac.iloc[idx])
            down_val = float(syst_down_fac.iloc[idx])
            nom_val = nominal_yields[bin_num]
            systematics[syst][bin_num] = {
                "systup": up_val,
                "systdown": down_val,
                "nomYield": nom_val
            }

    ## Adding global luminosity systematic
    add_global_systematic(systematics, "lumi", 1.025)
    #add_global_systematic(systematics, "xsec", 1.005)
    combine_syst(systematics)
    systematics = format_floats(systematics, 6)

    ## print on screen
    name_length = max(len(syst) for syst in systematics.keys())
    print(f"Systematics (+up-down) for {campaign}, {channel}:")
    for syst, bins in systematics.items():
        parts = []
        if "global" in bins:
            # Handle global systematics
            vals = bins["global"]
            parts.append(f"+{vals['systup']:.6f}-{vals['systdown']:.6f} (global)")
        else:
            # Handle bin-by-bin systematics
            for b, vals in bins.items():
                if str(b).isdigit():  # Only numeric bins
                    parts.append(f"+{vals['systup']:.6f}-{vals['systdown']:.6f}")
        print(f"\033[33m{syst.ljust(name_length)}\033[0m: " + ", ".join(parts))
    
    outdir = "systematic_uncertainties"
    os.makedirs(outdir, exist_ok=True)

    ## write JSON
    json_str = format_json(systematics)
    outfile  = os.path.join(outdir, f"{campaign}_{channel}.json")
    with open(outfile, "w") as f: f.write(json_str)
    print(f"\033[1;33m\nSaved JSON:\033[0m {outfile}")

# ------------------ utility functions --------------------

def csv_into_df(csvfile):
    if not os.path.exists(csvfile):
        print(f"\033[31mFile not found:\033[0m {csvfile}, skipping ...")
        return None
    df = pd.read_csv(csvfile)
    df = df[[c for c in df.columns if "VLL" not in c and "Data" not in c]]
    #print(df)
    return df

def extract_central(df):
    df_central = df.copy()
    for col in df_central.columns:
        if df_central[col].dtype == object:
            ## split at ± and take the first piece
            df_central[col] = df_central[col].str.split("±").str[0].str.strip()
            ## convert only if it's numeric
            df_central[col] = pd.to_numeric(df_central[col], errors="coerce")
    return df_central

def add_global_systematic(systematics, name, value):
    """Add a global systematic that applies the same value to all bins"""
    systematics[name] = {
        "global": {
            "systup": value,
            "systdown": value
        }
    }

def format_floats(obj, precision=6):
    if isinstance(obj, float): return float(f"{obj:.{precision}f}")
    if isinstance(obj, dict):  return {k: format_floats(v, precision) for k, v in obj.items()}
    if isinstance(obj, list):  return [format_floats(v, precision) for v in obj]
    return obj

def format_json(obj, precision=6):
    lines = ["{"]
    for i, (syst, bins) in enumerate(obj.items()):
        lines.append(f'  "{syst}": {{')
        bin_lines = []
        for b, vals in bins.items():
            if "nomYield" in vals:
                ## Bin-by-bin systematics with nominal yield
                bin_lines.append(
                    f'    "{b}": {{"systup": {vals["systup"]:.{precision}f},"systdown": {vals["systdown"]:.{precision}f},"nomYield": {vals["nomYield"]:.{precision}f}}}'
                )
            else:
                ## Global systematics or combined (without nominal yield)
                bin_lines.append(
                    f'    "{b}": {{"systup": {vals["systup"]:.{precision}f},"systdown": {vals["systdown"]:.{precision}f}}}'
                )
        lines.append(",\n".join(bin_lines))
        lines.append("  }" + ("," if i < len(obj) - 1 else ""))
    lines.append("}")
    return "\n".join(lines)

# -------- EXECUTION ---------
if __name__ == "__main__": main()
