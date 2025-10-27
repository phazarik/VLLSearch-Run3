import os, sys
import pandas as pd
import math
import ROOT
from array import array
from rich.console import Console
console = Console(highlight=False)
print = console.print
import time
from datetime import timedelta

# ------------------- config ---------------------
systematics = ["lep", "trig", "pileup", "bjet", "dy", "qcd", "ttbar", "jec", "jer"] ## non-global ones
basedir = "../Systematics/yields/2025-10-27/" ## Assuming all yields to be in the same directory.
#basedir = "../Systematics/yields/2025-09-26/" ## Assuming all yields to be in the same directory.
#-------------------------------------------------

final_state = "2LOS"
dump = "shapes"
#os.system(f"rm -rf {dump}/*")

campaigns = [
    "2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
    "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix",
]
#campaigns.extend(["Run2", "Run3", "FullDataset"])
channels  = ["mm", "me", "em", "ee"]
#channels.extend(["combined"])
tag = f"{final_state}_srpre"
sigdict = {
    "VLLD": {
        "ele": [100, 200, 300, 400, 600, 800, 1000, 1200],
        "mu": [100, 200, 300, 400, 600, 800, 1000, 1200],
        "tau": []
    },
    "VLLS":{
    }
}
variable = {
    "name": "LTplusMET",
    "bins": {
        "1":(0, 200),
        "2":(200, 400),
        "3":(400, 450),
        "4":(450, 500),
        "5":(500, 550),
        "6":(550, 600)
    }
}

#-------------------------------------------------
def main():
    time_start = time.time()
    count = 0
    for camp in campaigns:
        for ch in channels:
            #if "2018_UL" not in camp:  continue ## testing one campaign
            #if ch not in ["mm", "ee"]: continue ## For 2LOS
            #if (camp=="Run2" or camp=="Run3") and ch != "combined": continue
            count += 1
            print(f"\n[yellow bold][{count}] processing {camp}, {ch} channel[/yellow bold]")
            make_shapes(camp, ch)
            #break ## channel
        #break ## campaign

    time_end = time.time()
    elapsed = timedelta(seconds=int(time_end - time_start))
    print(f"\nDone!.\nTime taken: {elapsed}")
        
def make_shapes(campaign, channel):

    ## Nominal
    path_nom = os.path.join(basedir, tag)
    file_nom = os.path.join(path_nom, f"yields_{tag}_{campaign}_{channel}.csv")
    df_nom = csv_into_df(file_nom)
    #print(df_nom)
    if df_nom is None:
        print(f"\033[31m[Error] Nominal CSV missing for {campaign}, {channel}\033[0m")
        return

    ## Systematics
    dfs_syst = {}
    for syst in systematics:
        for direction in ["Up", "Down"]:
            subtag = f"{tag}_{syst}-syst{direction}"
            path_sys = os.path.join(basedir, subtag)
            file_sys = os.path.join(path_sys, f"yields_{subtag}_{campaign}_{channel}.csv")
            if os.path.exists(file_sys):
                print(f"Opening {file_sys}")
                dfs_syst[(syst, direction)] = csv_into_df(file_sys)
                #print(f"{syst}_{direction}", dfs_syst[(syst, direction)].shape)
                #print(dfs_syst[(syst, direction)])
            else: print(f"\033[31m[Warning] file not found: {file_sys}.\033[0m {syst} systematics not included in shape file.")

    ## Loop over signals defined in sigdict
    for sigtype, flavs in sigdict.items():
        for flav, masses in flavs.items():
            for m in masses:
                signal_name = f"{sigtype}_{flav}_{m}"
                outname = f"shapes_{final_state}_{campaign}_{channel}_{signal_name}.root"
                dump_subdir = f"{dump}/{campaign}_{channel}"
                if os.path.exists(dump_subdir) and not os.path.isdir(dump_subdir): os.remove(dump_subdir)
                os.makedirs(dump_subdir, exist_ok=True)
                outfile = os.path.join(dump_subdir, outname)
                make_root_file(df_nom, dfs_syst, signal_name, campaign, channel, outfile)
                print(f"Histograms written to \033[33m{outfile}\033[0m")

# ----------------- utilities --------------------

rootname = {
    "nData": "data_obs",
    "TotalBkg": "Bkg",
    "t#bar{t}+x": "TTX",
    "multi-top": "MultiTop",
    "W+jets/#gamma": "WJetsGamma",
    "Z#gamma": "ZGamma",
    "Single t":"ST",
    "VLLD-mu_100": "VLLD_mu_100",
    "VLLD-mu_200": "VLLD_mu_200",
    "VLLD-mu_300": "VLLD_mu_300",
    "VLLD-mu_400": "VLLD_mu_400",
    "VLLD-mu_600": "VLLD_mu_600",
    "VLLD-mu_800": "VLLD_mu_800",
    "VLLD-mu_1000": "VLLD_mu_1000",
    "VLLD-mu_1200": "VLLD_mu_1200",
    "VLLD-ele_100": "VLLD_ele_100",
    "VLLD-ele_200": "VLLD_ele_200",
    "VLLD-ele_300": "VLLD_ele_300",
    "VLLD-ele_400": "VLLD_ele_400",
    "VLLD-ele_600": "VLLD_ele_600",
    "VLLD-ele_800": "VLLD_ele_800",
    "VLLD-ele_1000": "VLLD_ele_1000",
    "VLLD-ele_1200": "VLLD_ele_1200",
}

bin_alias = {
    ("2016preVFP_UL","mm") : f"sr{final_state}16premm",
    ("2016preVFP_UL","me") : f"sr{final_state}16preme",
    ("2016preVFP_UL","em") : f"sr{final_state}16preem",
    ("2016preVFP_UL","ee") : f"sr{final_state}16preee",
    ("2016postVFP_UL","mm"): f"sr{final_state}16postmm",
    ("2016postVFP_UL","me"): f"sr{final_state}16postme",
    ("2016postVFP_UL","em"): f"sr{final_state}16postem",
    ("2016postVFP_UL","ee"): f"sr{final_state}16postee",
    ("2017_UL","mm")       : f"sr{final_state}17mm",
    ("2017_UL","me")       : f"sr{final_state}17me",
    ("2017_UL","em")       : f"sr{final_state}17em",
    ("2017_UL","ee")       : f"sr{final_state}17ee",
    ("2018_UL","mm")       : f"sr{final_state}18mm",
    ("2018_UL","me")       : f"sr{final_state}18me",
    ("2018_UL","em")       : f"sr{final_state}18em",
    ("2018_UL","ee")       : f"sr{final_state}18ee",
    ("Run3Summer22","mm")  : f"sr{final_state}22mm",
    ("Run3Summer22","me")  : f"sr{final_state}22me",
    ("Run3Summer22","em")  : f"sr{final_state}22em",
    ("Run3Summer22","ee")  : f"sr{final_state}22ee",
    ("Run3Summer22EE","mm"): f"sr{final_state}22EEmm",
    ("Run3Summer22EE","me"): f"sr{final_state}22EEme",
    ("Run3Summer22EE","em"): f"sr{final_state}22EEem",
    ("Run3Summer22EE","ee"): f"sr{final_state}22EEee",
    ("Run3Summer23","mm")  : f"sr{final_state}23mm",
    ("Run3Summer23","me")  : f"sr{final_state}23me",
    ("Run3Summer23","em")  : f"sr{final_state}23em",
    ("Run3Summer23","ee")  : f"sr{final_state}23ee",
    ("Run3Summer23BPix","mm") : f"sr{final_state}23BPixmm",
    ("Run3Summer23BPix","me") : f"sr{final_state}23BPixme",
    ("Run3Summer23BPix","em") : f"sr{final_state}23BPixem",
    ("Run3Summer23BPix","ee") : f"sr{final_state}23BPixee"
}

def csv_into_df(csvfile):
    if not os.path.exists(csvfile):
        print(f"\033[31mFile not found:\033[0m {csvfile}, skipping ...")
        return None
    df = pd.read_csv(csvfile)
    df.rename(columns=rootname, inplace=True)
    #print("df loaded :"+csvfile)
    return df

def parse_valerr(entry):
    ## Parse string like '12.3 ± 4.5' or '7.0' -> (val, err).
    if isinstance(entry, str) and "±" in entry:
        val, err = entry.split("±")
        return float(val.strip()), float(err.strip())
    else:
        val = float(str(entry).strip())
        err = math.sqrt(val) if val > 0 else 0.0
        return val, err

def make_root_file(df_nom, dfs_syst, signal_name, campaign="", channel="", outfile="shapes.root"):

    bins = variable["bins"]
    edges = [low for (_, (low, high)) in sorted(bins.items(), key=lambda x: int(x[0]))]
    edges.append(list(bins.values())[-1][1])
    nbins = len(edges) - 1
    edges_arr = array('d', edges)

    fout = ROOT.TFile(outfile, "RECREATE")
    
    chname = bin_alias.get((campaign, channel))  # e.g., sr18mm; cannot start with a digit

    processes = []
    for c in df_nom.columns:
        if c == "nbin": continue
        if "VLL" in c and c != signal_name: continue
        processes.append(c)

    if signal_name not in processes:
        print(f"\033[31m[ERROR] Expected signal '{signal_name}' not found in dataframe for {outfile}\033[0m")
        fout.Close()
        return

    for proc in processes:

        if "Bkg" in proc: continue
        
        ## Nominal histogram
        h_nom_name = f"{variable['name']}_{chname}_{proc.replace('_','')}" ## remove extra '_'
        if proc == "data_obs": h_nom_name = f"{variable['name']}_{chname}_{proc}" 
        h_nom = ROOT.TH1F(h_nom_name, proc, nbins, edges_arr)
        for i in range(1, nbins + 1):
            val, err = parse_valerr(df_nom.iloc[i - 1][proc])
            h_nom.SetBinContent(i, val)
            h_nom.SetBinError(i, err)
        # --- bypass for empty signal histograms, or negative integrals ---
        if h_nom.Integral() == 0 and "VLL" in proc:
            h_nom.SetBinContent(nbins, 1e-6)
            h_nom.SetBinError(nbins, 1e-6)
        if h_nom.Integral() < 0:
            print(f"\033[31m[Warning] Negative integral {h_nom.Integral():.3f} for {h_nom.GetName()} in {outfile}, setting to 0\033[0m")
            h_nom.Reset()  # clear bins
            h_nom.SetBinContent(nbins, 1e-6)
            h_nom.SetBinError(nbins, 1e-6)
        h_nom.Write()

        ## Skip data for systematics
        #if "VLL" in proc: continue
        if proc == "data_obs": continue
        
        ## Systematics
        for syst, direction in dfs_syst.keys():
            df = dfs_syst[(syst, direction)]
            if proc not in df.columns:
                print(f"\033[31m[ERROR] Missing process '{proc}' in systematic '{syst}{direction}' for {outfile}\033[0m")
                fout.Close()
                return

            h_syst_name = f"{variable['name']}_{chname}_{proc.replace('_','')}_{syst}{direction}" ## remove extra '_'
            h_syst = ROOT.TH1F(h_syst_name, proc, nbins, edges_arr)
            for i in range(1, nbins + 1):
                val, err = parse_valerr(df.iloc[i - 1][proc])
                h_syst.SetBinContent(i, val)
                h_syst.SetBinError(i, err)
            # --- bypass for empty systematics, or negative integrals ---
            if h_syst.Integral() == 0 and "VLL" in proc:
                h_syst.SetBinContent(nbins, 1e-6)
                h_syst.SetBinError(nbins, 1e-6)
            if h_syst.Integral() < 0:
                print(f"\033[31m[Warning] Negative integral {h_syst.Integral():.3f} for {h_syst.GetName()} in {outfile}, setting to 0\033[0m")
                h_syst.Reset()
                h_syst.SetBinContent(nbins, 1e-6)
                h_syst.SetBinError(nbins, 1e-6)
            h_syst.Write()

    fout.Close()

# --------- EXECUTION -----------
if __name__ == "__main__": main()


