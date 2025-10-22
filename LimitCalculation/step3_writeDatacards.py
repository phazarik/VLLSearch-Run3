import os
import ROOT
import time
from datetime import timedelta

# ------------------- config ---------------------
sourcedir = "shapes"
outdir = sourcedir
campaigns = [
    "2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
    "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix",
]
campaigns.extend(["Run2", "Run3", "FullDataset"])
channels  = ["mm", "me", "em", "ee"]
channels.extend(["combined"])
bkgs = ['DY', 'Higgs', 'MultiTop', 'QCD', 'ST', 'TTX', 'VV', 'VVV', 'WJetsGamma', 'WWss', 'ZGamma']
#-------------------------------------------------

## remove existing datacards:
os.system("rm -rf shapes/*/datacard*")

def main():
    time_start = time.time()
    for campaign in campaigns:
        for channel in channels:

            #if "2018_UL" not in campaign: continue ## testing
            #if channel not in ["mm", "ee"]: continue
            
            indir = os.path.join(sourcedir, f"{campaign}_{channel}")
            if not os.path.exists(indir):
                print(f"\n\033[31mSkipping missing campaign, channel = {campaign}, {channel}\033[0m")
                continue

            files = sorted(os.listdir(indir))

            hline = "-"*25
            print(f"\n{hline}\n\033[94m{campaign} {channel}\033[0m\n{hline}")

            for fname in files:
                #if "2LSS" in fname: continue
                if not fname.endswith(".root"): continue
                final_state = fname.split("_")[1] if "_" in fname else "unknown"

                print(f"\033[33m==> Processing {fname} ...\033[0m", end="")
                fpath = os.path.join(indir, fname)
                tf = ROOT.TFile.Open(fpath)
                if not tf or tf.IsZombie():
                    print(f"Failed to open {fpath}")
                    continue

                write_datacard_from_keys(outdir, tf, campaign, channel, final_state, useSyst=False)
                tf.Close()

    time_end = time.time()
    elapsed = timedelta(seconds=int(time_end - time_start))
    print(f"\nDone!.\nTime taken: {elapsed}")

def safe_integral(h, is_signal=False):
    if not h: return 0
    val = h.Integral()
    if val==0 and is_signal: val = 1e-6
    return val

def write_datacard_from_keys(outdir, rfile, campaign, channel, final_state, useSyst=True):

    outdir = os.path.join(outdir, f"{campaign}_{channel}")
    os.makedirs(outdir, exist_ok=True)

    histodict = {}
    for key in rfile.GetListOfKeys():
        obj = key.ReadObj()
        histodict[key.GetName()] = obj

    if not histodict:
        print("\033[33m[Error] No histograms in file.\0330m")
        return

    #print("\033[36m\n[DEBUG] Histograms in ROOT file:\033[0m")
    #for k, h in histodict.items(): print(f"{k}: integral = {h.Integral() if h else 'None'}")

    first_key = next(iter(histodict.keys()))
    parts = first_key.split("_")
    variable = parts[0]

    nominal = {}
    bins_set = set()
    processes_set = set()

    ## Iterating over hists:
    for hname, hist in histodict.items():
        parts = hname.split("_")
        if len(parts) == 3: var, ch, proc = parts
        elif len(parts) == 4 and parts[2] == "data" and parts[3] == "obs": var, ch, proc = parts[0], parts[1], "data_obs"
        else: continue
    
        nominal[(ch, proc)] = hist
        bins_set.add(ch) ## Cannot start with a digit
        processes_set.add(proc)

    if not bins_set:
        print(f"\033[31m[Warning] No nominal histograms of form {var}_{ch}_{proc} found.\033[0m")
        return

    bins = sorted(list(bins_set))
    processes = sorted(list(processes_set))
    print(f"Processes = {processes}")
    
    signal_name = next((p for p in processes if "VLL" in p), None)
    if signal_name is None:
        print(f"\033[31m[Warning] Signal-name not found, taking first process as signal.\033[0m")
        signal_name = processes[0]
        
    data_name = next((p for p in processes if "data" in p.lower()), None)
    if data_name is None:
        print(f"\033[31m[Warning] Data not found.\033[0m")

    ## Missing sample check
    all_bkgs = [p for p in processes if p != signal_name and p != data_name]
    missing = [b for b in all_bkgs if b not in bkgs]
    if missing: print(f"\033[31m[Warning] Missing in bkgs: {missing}\033[0m")
    samples = [signal_name] + bkgs

    ## only include processes actually present in the ROOT file, not all bkgs.
    #all_procs = set(p for (ch, p) in nominal.keys() if p != data_name)
    #signal_name = next((p for p in all_procs if "VLL" in p), None)
    #all_bkgs = [p for p in all_procs if p != signal_name]
    #samples = [signal_name] + all_bkgs
    #print(f"Using samples for bin {bins}: {samples}")
    
    # ----- debug statements -----
    #print("Found processes:")
    #print(f"Signal = {signal_name}")
    #print(f"Backgrounds = {backgrounds}")
    #print(f"Data = {data_name}")
    # ----------------------------

    #-------------------------------------------------------------------------------
    #-------------------------------------------------------------------------------
    dcfilename = os.path.join(outdir, f"datacard_{final_state}_{campaign}_{channel}_{signal_name}_{variable}.txt")
    with open(dcfilename, "w") as dc:

        ### HEADER
        dc.write(f"imax {len(bins)}\n")
        dc.write(f"jmax {len(bkgs)}\n")
        dc.write("kmax *\n\n")

        ### HEADER FOR HISTOGRAM KEYS
        dc.write(f"shapes * * {os.path.basename(rfile.GetName())} {variable}_$CHANNEL_$PROCESS {variable}_$CHANNEL_$PROCESS_$SYSTEMATIC\n\n")

        ### OBSERVATION
        dc.write("bin      " + "  ".join(bins) + "\n")
        obs_vals = []
        obs_vals = []
        for bin_ in bins:
            h = nominal.get((bin_, "data_obs"), None)
            obs_vals.append(f"{int(safe_integral(h))}" if h else "0")
        dc.write("observation  " + "  ".join(obs_vals) + "\n\n")

        ### DEFINING PROCESS COLUMNS (INCLUDING SIGNAL)
        repeated_bins = []
        for bin_ in bins: repeated_bins.extend([bin_] * len(samples))
        dc.write("bin      " + "  ".join(repeated_bins) + "\n")

        proc_names = []
        for bin_ in bins: proc_names.extend([p.replace("_", "") for p in samples])
        dc.write("process  " + "  ".join(proc_names) + "\n")

        proc_indices = []
        for _ in bins: proc_indices.extend(str(i) for i in range(len(samples)))
        dc.write("process  " + "  ".join(proc_indices) + "\n")

        ### RATES (INTEGRAL OF EACH SHAPE IN EACH BIN)              
        rates = []
        for bin_ in bins:
            for p in samples:
                h = nominal.get((bin_, p), None)
                is_signal = (p == signal_name)
                rate = f"{safe_integral(h, is_signal):.6f}"
                if is_signal and rate == "0.000000": rate = "0.000001"
                rates.append(rate)
                #rates.append(f"{safe_integral(h):.3f}")
        dc.write("rate     " + "  ".join(rates) + "\n")
        dc.write("-" * 80 + "\n")

        #-------------------------------------------------------------------------
        ### EXTRACTING SYSTEMATIC UNCERTAINTIES
        if useSyst:
            ## Systematic uncertainty keys (sensititve to keys in shape-files)
            shape_syst_map = {
                "lep": {"procs": samples, "n": 1},
                "trig": {"procs": samples, "n": 1},
                "bjet": {"procs": samples, "n": 1},
                "pileup": {"procs": samples, "n": 1},
                "dy": {"procs": ["DY"], "n": 1},
                "qcd": {"procs": ["QCD"], "n": 1},
                "ttbar": {"procs": ["TTX"], "n": 1},
            }
            
            ## Global lnN systematics
            global_syst_map = {
                "lumi": {"procs": samples, "val": 1.025, "type": "lnN"},
                "xsec": {"procs": samples, "val": 1.050, "type": "lnN"},
            }
            
            ## Shape systematics:
            for syst, info in shape_syst_map.items():
                row = [syst, "shape"]
                for bin_ in bins:
                    for p in samples:
                        #if p == signal_name: row.append("-") ## Skipping systematics for signal
                        #else: row.append(str(info["n"]) if p in info["procs"] else "-")
                        row.append(str(info["n"]) if p in info["procs"] else "-")
                dc.write(" ".join(row) + "\n")
            ## Global systematics:
            for syst, info in global_syst_map.items():
                row = [syst, info["type"]]
                for bin_ in bins:
                    for p in samples:
                        row.append(str(info["val"]) if p in info["procs"] else "-")
                dc.write(" ".join(row) + "\n")
        #-------------------------------------------------------------------------

        dc.write("\n* autoMCStats 0 0 1\n")
    #-------------------------------------------------------------------------------
    #-------------------------------------------------------------------------------
    print(f"File created: \033[1;33m{dcfilename}\033[0m")

if __name__ == "__main__": main()
