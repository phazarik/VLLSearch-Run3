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
basedir = "../Systematics/yields/2025-09-18/" ## Assuming all yields to be in the same directory.
dump = "shapes"
#-------------------------------------------------

campaigns = [
    "2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
    "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix",
]
#campaigns.extend(["Run2", "Run3", "FullDataset"])
channels  = ["mm", "me", "em", "ee"]
#channels.extend(["combined"])
tag = "sr"
sigdict = {
    "VLLD": {
        "ele": [100, 200, 300, 400, 600, 800, 1000],
        "mu": [100, 200, 300, 400, 600, 800, 1000],
        "tau": []
    },
    "VLLS":{
    }
}
variable = {
    "name": "LTplusMET",
    "bins": {
        "1":(0, 25),
        "2":(25, 50),
        "3":(50, 100),
        "4":(100, 200),
        "5":(200, 300),
        "6":(300, 400),
        "7":(400, 500)
    }
}

#-------------------------------------------------
def main():
    time_start = time.time()
    count = 0
    for camp in campaigns:
        for ch in channels:
            if (camp=="Run2" or camp=="Run3") and ch != "combined": continue
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

    ## Systematics
    dfs_syst = {}
    for syst in systematics:
        for direction in ["Up", "Down"]:
            subtag = f"{tag}_{syst}-syst{direction}"
            path_sys = os.path.join(basedir, subtag)
            file_sys = os.path.join(path_sys, f"yields_{subtag}_{campaign}_{channel}.csv")
            if os.path.exists(file_sys):
                dfs_syst[(syst, direction)] = csv_into_df(file_sys)
                #print(f"{syst}_{direction}", dfs_syst[(syst, direction)].shape)
                #print(dfs_syst[(syst, direction)])
            else: print(f"\033[31m[Warning] file not found: {file_sys}\033[0m")

    ## Loop over signals defined in sigdict
    for sigtype, flavs in sigdict.items():
        for flav, masses in flavs.items():
            for m in masses:
                signal_name = f"{sigtype}_{flav}_{m}"
                outname = f"shapes_{campaign}_{channel}_{signal_name}.root"
                dump_subdir = f"{dump}/{campaign}_{channel}"
                outfile = os.path.join(dump_subdir, outname)
                os.makedirs(dump_subdir, exist_ok=True)
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
    "VLLD-ele_100": "VLLD_ele_100",
    "VLLD-ele_200": "VLLD_ele_200",
    "VLLD-ele_300": "VLLD_ele_300",
    "VLLD-ele_400": "VLLD_ele_400",
    "VLLD-ele_600": "VLLD_ele_600",
    "VLLD-ele_800": "VLLD_ele_800",
    "VLLD-ele_1000": "VLLD_ele_1000",
}

bin_alias = {
    ("2016preVFP_UL","mm") : "sr16premm",
    ("2016preVFP_UL","me") : "sr16preme",
    ("2016preVFP_UL","em") : "sr16preem",
    ("2016preVFP_UL","ee") : "sr16preee",
    ("2016postVFP_UL","mm"): "sr16postmm",
    ("2016postVFP_UL","me"): "sr16postme",
    ("2016postVFP_UL","em"): "sr16postem",
    ("2016postVFP_UL","ee"): "sr16postee",
    ("2017_UL","mm")       : "sr17mm",
    ("2017_UL","me")       : "sr17me",
    ("2017_UL","em")       : "sr17em",
    ("2017_UL","ee")       : "sr17ee",
    ("2018_UL","mm")       : "sr18mm",
    ("2018_UL","me")       : "sr18me",
    ("2018_UL","em")       : "sr18em",
    ("2018_UL","ee")       : "sr18ee",
    ("Run3Summer22","mm")  : "sr22mm",
    ("Run3Summer22","me")  : "sr22me",
    ("Run3Summer22","em")  : "sr22em",
    ("Run3Summer22","ee")  : "sr22ee",
    ("Run3Summer22EE","mm"): "sr22EEmm",
    ("Run3Summer22EE","me"): "sr22EEme",
    ("Run3Summer22EE","em"): "sr22EEem",
    ("Run3Summer22EE","ee"): "sr22EEee",
    ("Run3Summer23","mm")  : "sr23mm",
    ("Run3Summer23","me")  : "sr23me",
    ("Run3Summer23","em")  : "sr23em",
    ("Run3Summer23","ee")  : "sr23ee",
    ("Run3Summer23BPix","mm") : "sr23BPixmm",
    ("Run3Summer23BPix","me") : "sr23BPixme",
    ("Run3Summer23BPix","em") : "sr23BPixem",
    ("Run3Summer23BPix","ee") : "sr23BPixee"
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
        h_nom.Write()

        ## Skip signal/data for systematics
        if "VLL" in proc: continue
        if proc == "data_obs": continue
        
        ## Systematics
        for syst, direction in dfs_syst.keys():
            df = dfs_syst[(syst, direction)]
            if proc not in df.columns:
                print(f"\033[31m[ERROR] Missing process '{proc}' in systematic '{syst}{direction}' for {outfile}\033[0m")
                fout.Close()
                return

            h_syst_name = f"{variable['name']}_{chname}_{proc}_{syst}{direction}"
            h_syst = ROOT.TH1F(h_syst_name, proc, nbins, edges_arr)
            for i in range(1, nbins + 1):
                val, err = parse_valerr(df.iloc[i - 1][proc])
                h_syst.SetBinContent(i, val)
                h_syst.SetBinError(i, err)
            h_syst.Write()

    fout.Close()

# --------- EXECUTION -----------
if __name__ == "__main__": main()


