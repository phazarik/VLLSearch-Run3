import os
import json

# ------------------ config --------------------
test = False
campaigns = [
    "2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
    "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix",
    "Run2", "Run3", "FullDataset"
]
channels  = ["mm", "me", "em", "ee", "combined"]
final_states = ["2LSS", "2LOS", "2L"]

def main():
    count = 0
    for camp in campaigns:
        for ch in channels:
            ## Exceptions:
            #if "2018" not in camp: continue
            #if ch not in ["mm", "ee"]: continue
            #if (camp=="Run2" or camp=="Run3") and ch != "combined": continue

            for fs in final_states:
                count +=1
                print("\n"+"-"*50+f"\n\033[33m[{count}] processing {camp}, {ch} channel ({fs})\033[0m\n"+"-"*50)

                process_once(camp, ch, fs, "13TeV")
                process_once(camp, ch, fs, "13p6TeV")
                
                #break ##final state
            #break ##channel
        #break ##campaign
    print("\nDone!")

def process_once(campaign="2018_UL", channel="mm", final_state="2LOS", energy="13TeV"):

    ## Setting up input/output
    indir = "limits_rvalue"
    outdir = f"limits_sigmaB_{energy}"
    os.makedirs(outdir, exist_ok=True)

    ## Loading data
    limits_rvalue_file = os.path.join(indir, f"limits_{final_state}_{campaign}_{channel}.json")
    if not os.path.exists(limits_rvalue_file):
        print(f"\033[31m[WARNING] File not found, skipping: {limits_rvalue_file}\033[0m")
        return
    
    print(f"Processing: {limits_rvalue_file}")
    with open(limits_rvalue_file) as f: data = json.load(f)
    file_xsec_doublet = "xsec/sigmaB_VLLD_13TeV.txt"
    file_xsec_singlet = "xsec/sigmaB_VLLS_13TeV.txt"
    if energy == "13p6TeV":
        file_xsec_doublet = "xsec/sigmaB_VLLD_13p6TeV.txt"
        file_xsec_singlet = "xsec/sigmaB_VLLS_13p6TeV.txt"
    xsec_VLLD = read_xsec(file_xsec_doublet)
    xsec_VLLS = read_xsec(file_xsec_singlet)
    print(f"\n\033[33mData loaded for {campaign}, {channel} at {energy}\033[0m")
    if test: print(data)

    for model, flavors in data.items():
        if model != "VLLD": continue  ## only doublet

        for flavor, masspoints in flavors.items():
            if not masspoints: continue
            
            if test: print(f"Processing {model}, {flavor}-type.")
            outfile = os.path.join(outdir, f"sigmaB_{final_state}_{model}_{flavor}_{campaign}_{channel}.txt")

            with open(outfile, "w") as fout:
                fout.write("Mass\tObserved\tExp_2sigDn\tExp_1sigDn\tExp_nominal\tExp_1sigUp\tExp_2sigUp\ttheory\n")
                for mass_str, info in sorted(masspoints.items(), key=lambda x: int(x[0])):
                    mass = int(mass_str)
                    
                    if "limit" not in info:
                        print(f"\033[31m[WARNING] Skipping mass {mass} (no limit field)\033[0m")
                        continue
                    
                    ## Extrating r-limits for each mass point
                    r_vals = parse_rvalue_limit(info["limit"])
                    if test: print(f"\033[2m{mass}\t" + "\t".join(f"{v:.6f}" for v in r_vals.values()) + "\033[0m")
                    required = ["obs", "exp_2d", "exp_1d", "exp_nom", "exp_1u", "exp_2u"]
                    if not all(k in r_vals for k in required):
                        print(f"\033[31mSkipping mass {mass} due to missing entries in limit field.\033[0m")
                        continue

                    ## Converting them to sigmaB-limits
                    scale = 1 ## Use this when signal histograms were scaled earlier.
                    if   model == "VLLD": sigmaB_theory = xsec_VLLD[mass] 
                    elif model == "VLLS": sigmaB_theory = xsec_VLLS[mass]
                    row = [
                        mass,
                        r_vals["obs"]    *sigmaB_theory *scale,
                        r_vals["exp_2d"] *sigmaB_theory *scale,
                        r_vals["exp_1d"] *sigmaB_theory *scale,
                        r_vals["exp_nom"]*sigmaB_theory *scale,
                        r_vals["exp_1u"] *sigmaB_theory *scale,
                        r_vals["exp_2u"] *sigmaB_theory *scale,
                        sigmaB_theory
                    ]
                    fout.write("\t".join(f"{x:.6f}" if isinstance(x,float) else str(x) for x in row) + "\n")

            print(f"File created: \033[93m{outfile}\033[0m")
            os.system(f"cat {outfile}")
            if test: break ## flavor
        if test: break ##model
            
# ------- utilities --------
def read_xsec(file):
    xsec = {}
    with open(file) as f:
        for line in f:
            mass, sigma = line.strip().split()
            xsec[int(mass)] = float(sigma)
    return xsec

def parse_rvalue_limit(limit_str):
    r_vals = {}
    for part in limit_str.split(","):
        key, val = part.split(":")
        key = key.strip()
        val = float(val.strip())
        if key == "Observed Limit": r_vals['obs'] = val
        elif key.startswith("Expected"):
            pct = key.split()[1].replace("%","")
            if pct == "2.5":    r_vals['exp_2d'] = val
            elif pct == "16.0": r_vals['exp_1d'] = val
            elif pct == "50.0": r_vals['exp_nom']= val
            elif pct == "84.0": r_vals['exp_1u'] = val
            elif pct == "97.5": r_vals['exp_2u'] = val
    return r_vals

# ------- execution --------
if __name__ == "__main__": main()
