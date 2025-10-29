#!/usr/bin/env python3
import os
import subprocess
import argparse
import time

campaigns = [
    "2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
    "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix"
]
channels  = ["mm", "me", "em", "ee"]
sigdict = {
    "VLLD": {
        "ele": [100,200,300,400,600,800,1000,1200],
        "mu":  [100,200,300,400,600,800,1000,1200],
        "tau": []
    },
    "VLLS": {}
}
final_states = ["2LSS","2LOS"]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--by", default="channels", help="Grouping mode: channels, campaigns, run2, run3, all, or run2_<ch>/run3_<ch>")
    parser.add_argument("--shapes_dir", default="shapes")
    parser.add_argument("--out_dir", default="shapes")
    parser.add_argument("--dryrun", action="store_true")
    parser.add_argument("--test", action="store_true")
    args = parser.parse_args()

    os.makedirs(args.out_dir, exist_ok=True)

    files = []
    for r, d, fs in os.walk(args.shapes_dir):
        if "_combined" in r or "FullDataset" in r: continue
        for f in fs:
            if not f.endswith(".root"): continue
            files.append(os.path.join(r, f))

    file_info = [parse_file(f) for f in files]
    run2 = [c for c in campaigns if "2016" in c or "2017" in c or "2018" in c]
    run3 = [c for c in campaigns if "Run3" in c]

    # ==========================
    # Define grouping logic
    # ==========================
    if args.by == "channels":    groups = {ch: (lambda ch_=ch: lambda c,ch__,s,f,m: ch__==ch_)() for ch in channels}
    elif args.by == "campaigns": groups = {c: (lambda c_=c: lambda c,ch,s,f,m: c==c_)() for c in campaigns}
    elif args.by == "run2":      groups = {"Run2": lambda c,ch,s,f,m: c in run2}
    elif args.by == "run3":      groups = {"Run3": lambda c,ch,s,f,m: c in run3}
    elif args.by == "all":       groups = {"FullDataset": lambda c,ch,s,f,m: True}
    elif args.by.startswith("run2_") or args.by.startswith("run3_"):
        ch_sel = args.by.split("_", 1)[1]
        if ch_sel not in channels:
            raise ValueError(f"Invalid channel in argument --by: {args.by}")
        if args.by.startswith("run2_"):
            valid_camps = [c for c in campaigns if any(y in c for y in ["2016", "2017", "2018"])]
            group_name = f"Run2_{ch_sel}"
        else:
            valid_camps = [c for c in campaigns if "Run3" in c]
            group_name = f"Run3_{ch_sel}"
        groups = {group_name: lambda c,ch,s,f,m,vc=valid_camps,ch_=ch_sel: c in vc and ch == ch_}
    else: raise ValueError(f"Invalid option for --by: {args.by}")

    global_start = time.time()
    combine(file_info, args.out_dir, groups, args.by, test=args.test, dryrun=args.dryrun)
    global_end = time.time()
    print(f"\nTotal elapsed time: {global_end - global_start:.1f} s")

def parse_file(f):
    fname = os.path.basename(f).replace(".root","")
    fs_match = next((fs for fs in final_states if fname.startswith(f"shapes_{fs}_")), None)
    if not fs_match: return None

    fname = fname[len(f"shapes_{fs_match}_"):]
    c = next((c for c in sorted(campaigns,key=len,reverse=True) if fname.startswith(c)), None)
    if not c: return None
    rest = fname[len(c)+1:]

    ch = next((ch for ch in channels if rest.startswith(ch+"_")), None)
    if not ch: return None

    rem = rest[len(ch)+1:].split("_")
    sig = rem[0]
    flav = rem[1] if len(rem)==3 else ""
    mass = rem[-1]

    return fs_match, c, ch, sig, flav, mass, f

def combine(file_info, out_dir, groups, by_option, test=False, dryrun=False):
    tasks = []
    file_info = [x for x in file_info if x is not None]

    for name, sel in groups.items():
        for sig, masses in sigdict.items():
            for flav, masslist in masses.items():
                for mass in masslist:
                    paths = [
                        fp for fs_, c,ch,s,fla,mass_str,fp in file_info
                        if sel(c,ch,s,fla,mass_str) and s==sig and fla==flav and int(mass_str)==mass
                    ]
                    if paths:
                        if by_option == "channels":
                            ch_in_paths = sorted({
                                os.path.basename(os.path.dirname(p)).replace("_combined","").split("_")[-1]
                                for p in paths
                            })
                            folder_name = f"FullDataset_{ch_in_paths[0]}"
                            file_name = folder_name
                        elif by_option == "campaigns":
                            campaign_only = name
                            folder_name = f"{campaign_only}_combined"
                            file_name = f"{campaign_only}_combined"
                        elif by_option.startswith("run2_") or by_option.startswith("run3_"):
                            folder_name = name
                            file_name = name
                        else:
                            folder_name = name + "_combined"
                            file_name = name + "_combined"

                        odir = os.path.join(out_dir, folder_name)
                        os.makedirs(odir, exist_ok=True)
                        fs_prefix = "2L" if len(final_states) == 2 else final_states[0]
                        out = os.path.join(odir, f"shapes_{fs_prefix}_{file_name}_{sig}_{flav}_{mass}.root")
                        tasks.append((out, paths))

    total = len(tasks)
    for i, (out, paths) in enumerate(tasks, 1):
        print(f"\nProcessing {i}/{total}:")
        print(f"Output: \033[93m{out}\033[0m\nInputs ({len(paths)}):")
        for f in paths: print(f" - {f}")

        cmd = ["hadd","-f",out]+paths
        if dryrun:
            print(f"[DRYRUN] Command: {' '.join(cmd)}\n")
        else:
            start = time.time()
            subprocess.run(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            end = time.time()
            print(f"Time for this job: {end - start:.1f} s")

        if test: break

if __name__=="__main__": main()
