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
basedir = "yields/2025-09-09/"
tag = "sr"
# ------------------------------------------

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
            # split at ± and take the first piece
            df_central[col] = df_central[col].str.split("±").str[0].str.strip()
            # convert only if it's numeric
            df_central[col] = pd.to_numeric(df_central[col], errors="coerce")
    return df_central

def add_global_systematic(systematics, df_nom, name, value):
    systematics[name] = {}
    for nbin in df_nom["nbin"]:
        if str(nbin).lower() == "total": continue
        systematics[name][int(nbin)] = {
            "systup": value,
            "systdown": value,
            "average": value
        }

def combine_syst(systematics):
    # get the bins from any existing systematic (e.g., the first one)
    first_key = next(k for k in systematics if k != "combined")
    bins = systematics[first_key].keys()

    systematics["combined"] = {}
    for b in bins:
        # sum only over existing keys excluding "combined"
        up_sq   = sum((systematics[s][b]["systup"] - 1)**2 for s in systematics if s != "combined")
        down_sq = sum((systematics[s][b]["systdown"] - 1)**2 for s in systematics if s != "combined")
        systematics["combined"][b] = {
            "systup": 1 + math.sqrt(up_sq),
            "systdown": 1 + math.sqrt(down_sq),
            "average": 1 + 0.5 * (math.sqrt(up_sq) + math.sqrt(down_sq))
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
            bin_lines.append(
                f'    "{b}": {{"systup": {vals["systup"]:.{precision}f},"systdown": {vals["systdown"]:.{precision}f},"average": {vals["average"]:.{precision}f}}}'
            )
        lines.append(",\n".join(bin_lines))
        lines.append("  }" + ("," if i < len(obj) - 1 else ""))
    lines.append("}")
    return "\n".join(lines)

def process(campaign, channel):
    csvfile_nom = os.path.join(basedir, tag, f"yields_{tag}_{campaign}_{channel}.csv")
    df_nom = csv_into_df(csvfile_nom)
    if df_nom is None: return

    systematics = {"lep": {}, "trig": {}}
    
    for syst in systematics.keys():
        tagup = f"{tag}_{syst}-systup"
        csvfile_up = os.path.join(basedir, tagup, f"yields_{tagup}_{campaign}_{channel}.csv")
        df_up = csv_into_df(csvfile_up)
        if df_up is None: continue

        tagdown = f"{tag}_{syst}-systdown"
        csvfile_down = os.path.join(basedir, tagdown, f"yields_{tagdown}_{campaign}_{channel}.csv")
        df_down = csv_into_df(csvfile_down)
        if df_down is None: continue

        nom_central   = extract_central(df_nom)["TotalBkg"]
        up_central    = extract_central(df_up)["TotalBkg"]
        down_central  = extract_central(df_down)["TotalBkg"]

        syst_up_fac   = 1 + (up_central - nom_central) / nom_central.replace(0, float("inf"))
        syst_down_fac = 1 + (nom_central - down_central) / nom_central.replace(0, float("inf"))

        ## Fix NaNs with default value 1
        syst_up_fac   = syst_up_fac.where(nom_central != 0, 1).fillna(1)  
        syst_down_fac = syst_down_fac.where(nom_central != 0, 1).fillna(1)

        for idx, nbin in enumerate(df_nom["nbin"]):
            if str(nbin).lower() == "total": continue
            up_val = float(syst_up_fac.iloc[idx])
            down_val = float(syst_down_fac.iloc[idx])
            avg_val = (up_val + down_val) / 2.0
            # ensure zeros become 1
            #if up_val == 0.0: up_val = 1.0
            #if down_val == 0.0: down_val = 1.0
            avg_val = (up_val + down_val) / 2.0
            systematics[syst][int(nbin)] = {
                "systup": up_val,
                "systdown": down_val,
                "average": avg_val
            }


    ## Adding global systematics and add a "total" key.
    add_global_systematic(systematics, df_nom, "lumi", 1.025)
    combine_syst(systematics)
    systematics = format_floats(systematics, 6)

    ## print on screen
    name_length = max(len(syst) for syst in systematics.keys())
    print(f"Systematics (+up-down) for {campaign}, {channel}:")
    for syst, bins in systematics.items():
        parts = []
        for b, vals in bins.items(): parts.append(f"+{vals['systup']:.6f}-{vals['systdown']:.6f}")
        print(f"\033[33m{syst.ljust(name_length)}\033[0m: " + ", ".join(parts))
    
    outdir = "systematic_uncertainties"
    os.makedirs(outdir, exist_ok=True)

    ## write JSON
    json_str = format_json(systematics)
    outfile  = os.path.join(outdir, f"{campaign}_{channel}.json")
    with open(outfile, "w") as f: f.write(json_str)
    #with open(outfile_json, "w") as f: json.dump(systematics, f, separators=(",", ": "))
    print(f"\033[1;33m\nSaved JSON:\033[0m {outfile}")
    
def main():
    count = 0
    for camp in campaigns:
        for ch in channels:
            #Excpetion:
            if (camp=="Run2" or camp=="Run3") and ch != "combined": continue
            count +=1
            print("\n"+"-"*50+f"\n\033[33m[{count}] processing {camp}, {ch} channel\033[0m\n"+"-"*50)
            process(camp, ch)
            #break #channel
        #break#campaign
    print("\nDone!")

if __name__ == "__main__": main()
