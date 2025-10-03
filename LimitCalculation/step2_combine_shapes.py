import os
import subprocess
import argparse
import time

campaigns = [
    "2016preVFP_UL","2016postVFP_UL","2017_UL","2018_UL",
    "Run3Summer22","Run3Summer22EE","Run3Summer23","Run3Summer23BPix"
]
channels  = ["mm","me","em","ee"]
sigdict = {
    "VLLD": {
        "ele": [100,200,300,400,600,800,1000],
        "mu":  [100,200,300,400,600,800,1000],
        "tau": []
    },
    "VLLS": {}
}

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--by", choices=["channels","campaigns","run2","run3","all"], default="channels")
    parser.add_argument("--shapes_dir", default="shapes")
    parser.add_argument("--out_dir", default="shapes")
    parser.add_argument("--dryrun", action="store_true")
    parser.add_argument("--test", action="store_true")
    args = parser.parse_args()

    os.makedirs(args.out_dir, exist_ok=True)

    # Collect ROOT files
    files = []
    for r, d, fs in os.walk(args.shapes_dir):
        if "_combined" in r: continue
        if "FullDataset" in r: continue
        for f in fs:
            if not f.endswith(".root"): continue
            fname = os.path.basename(f).replace("shapes_","").replace(".root","")
            files.append(os.path.join(r,f))

    file_info = [parse_file(f) for f in files]

    run2 = [c for c in campaigns if "2016" in c or "2017" in c or "2018" in c]
    run3 = [c for c in campaigns if "Run3" in c]

    if args.by == "channels":
        groups = {}
        for ch in channels: groups[ch] = (lambda ch_=ch: lambda c,ch__,s,f,m: ch__==ch_)()
    elif args.by == "campaigns":
        groups = {c: (lambda c_=c: lambda c,ch,s,f,m: c==c_)() for c in campaigns}
    elif args.by == "run2": groups = {"Run2": lambda c,ch,s,f,m: c in run2}
    elif args.by == "run3": groups = {"Run3": lambda c,ch,s,f,m: c in run3}
    elif args.by == "all": groups = {"FullDataset": lambda c,ch,s,f,m: True}

    global_start = time.time()
    combine(file_info, args.out_dir, groups, args.by, test=args.test, dryrun=args.dryrun)
    global_end = time.time()
    print(f"\nTotal elapsed time: {global_end - global_start:.1f} s")

def parse_file(f):
    fname = os.path.basename(f).replace("shapes_","").replace(".root","")    
    c = next((c for c in sorted(campaigns,key=len,reverse=True) if fname.startswith(c)), None)
    if not c: return None
    rest = fname[len(c)+1:]

    ch = next((ch for ch in channels if rest.startswith(ch+"_")), None)
    if not ch: return None

    rem = rest[len(ch)+1:].split("_")
    sig = rem[0]
    flav = rem[1] if len(rem)==3 else ""
    mass = rem[-1]

    # keep full campaign name including "_UL"
    return c, ch, sig, flav, mass, f

def combine(file_info, out_dir, groups, by_option, test=False, dryrun=False):
    tasks = []
    file_info = [x for x in file_info if x is not None]

    for name, sel in groups.items():
        for sig, masses in sigdict.items():
            for flav, masslist in masses.items():
                for mass in masslist:
                    paths = [
                        fp for c,ch,s,fla,mass_str,fp in file_info
                        if sel(c,ch,s,fla,mass_str) and s==sig and fla==flav and int(mass_str)==mass
                    ]
                    if paths:
                        # Dynamic output name
                        if by_option == "channels":
                            # Extract actual channel from input folders
                            ch_in_paths = sorted({
                                os.path.basename(os.path.dirname(p)).replace("_combined","").split("_")[-1]
                                for p in paths
                            })
                            folder_name = f"FullDataset_{ch_in_paths[0]}"  # folder has channel
                            file_name = folder_name  # file matches folder

                        elif by_option == "campaigns":
                            campaign_only = name  # name is the campaign key in groups
                            folder_name = f"{campaign_only}_combined"
                            file_name = f"{campaign_only}_combined"
                        else:
                            folder_name = name + "_combined"
                            file_name = name + "_combined"

                        odir = os.path.join(out_dir, folder_name)
                        os.makedirs(odir, exist_ok=True)
                        out = os.path.join(odir, f"shapes_{file_name}_{sig}_{flav}_{mass}.root")
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
