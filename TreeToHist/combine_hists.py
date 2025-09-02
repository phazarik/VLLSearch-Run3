import time
import os, argparse
from datetime import timedelta
from tqdm import tqdm

#---------------- CONFIG ------------------#
basedir = "../ROOT_FILES/hists/"
jobname = "2025-09-02_val"
tag = "val"
campaigns_all = [
    "2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
    "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix"
]
channels = ["ee", "em", "me", "mm"]
#------------------------------------------#

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--by", type=str, default="channels",
                        choices=["channels", "campaigns", "run2", "run3", "all"])
    parser.add_argument("--test", action="store_true")
    parser.add_argument("--dryrun", action="store_true")
    return parser.parse_args()

def define_outdirs(by):
    outdirs = {}
    if by == "channels":
        for camp in campaigns_all:
            dump = f"hist_{tag}_{camp}_combined"
            outdirs[camp] = os.path.join(basedir, jobname, dump)

    elif by == "run2":
        dump = f"hist_{tag}_Run2_combined"
        outdirs["Run2"] = os.path.join(basedir, jobname, dump)

    elif by == "run3":
        dump = f"hist_{tag}_Run3_combined"
        outdirs["Run3"] = os.path.join(basedir, jobname, dump)

    elif by == "campaigns":
        for ch in channels:
            dump = f"hist_{tag}_FullDataset_{ch}"
            outdirs[ch] = os.path.join(basedir, jobname, dump)

    elif by == "all":
        dump = f"hist_{tag}_FullDataset_combined"
        outdirs["all"] = os.path.join(basedir, jobname, dump)

    return outdirs

def collect_folders(by, outdirs):
    folders_by_key = {k: [] for k in outdirs}

    camps = campaigns_all
    if by == "run2":      camps = ["2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL"]
    elif by == "run3":    camps = ["Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix"]

    for camp in camps:
        for ch in channels:
            folder = f"hist_{tag}_{camp}_{ch}"
            fullpath = os.path.join(basedir, jobname, folder)
            if os.path.isdir(fullpath):
                if by == "channels":    key = camp
                elif by == "campaigns": key = ch
                else: key = list(outdirs.keys())[0]  ## In cases where there is only one outdir
                folders_by_key[key].append(fullpath)

    return folders_by_key


def collect_files(folders):
    all_files = {}
    for folder in folders:
        for fname in os.listdir(folder):
            if fname.endswith(".root"):
                all_files.setdefault(fname, []).append(os.path.join(folder, fname))
    return all_files


def combine_files(key, folders, outdir, all_files, dryrun=False, test=False):
    if len(folders) == 0:
        print(f"\033[31m[Error] folders not found for {key}.\033[0m")
        return
    if len(all_files) == 0:
        print(f"\033[31m[Error] files not found for {key}.\033[0m")
        return

    print(f"\033[0;33mOutput folder for {key}: {outdir}\033[0m")
    print(f"\033[0;33mInput folders for {key}:\033[0m")
    for f in folders: print(f"- {f}")

    start_time = time.time()
    count = 1
    iterator_var = sorted(all_files.items())

    wrapper = (
        tqdm(iterator_var, total=len(all_files), unit="file",
             desc=f"Combining {key}", colour="green", ncols=100, leave=True,
             bar_format="{l_bar}{bar}| [{elapsed} < {remaining}, {n_fmt}/{total_fmt}]")
        if not dryrun else iterator_var
    )

    for fname, paths in wrapper:
        outpath = os.path.join(outdir, fname)

        if len(paths) == 1: cmd = f"cp {paths[0]} {outpath}"
        else:               cmd = f"hadd -f {outpath} " + " ".join(paths)

        if dryrun:
            print(f"\n\033[36m(File#{count}/{len(all_files)}) Combining files into \033[33;1m{outpath}:\033[0m")
            for p in paths: print(f"- {p}")

        if not dryrun: os.system(f"{cmd} > /dev/null 2>&1")

        count += 1
        if test: break

    end_time = time.time()
    print(f"Time taken = {str(timedelta(seconds=int(end_time - start_time)))}\n")


def main():
    args = parse_args()

    outdirs = define_outdirs(args.by)
    if not args.dryrun:
        for od in outdirs.values():
            os.makedirs(od, exist_ok=True)

    folders_by_key = collect_folders(args.by, outdirs)

    jobcount = 0
    global_start_time = time.time()
    for key, folders in folders_by_key.items():
        jobcount += 1
        print("\n"+"="*20+f"\nRunning job {jobcount}/{len(folders_by_key)} ...\n"+"="*20)
        all_files = collect_files(folders)
        combine_files(key, folders, outdirs[key], all_files, dryrun=args.dryrun, test=args.test)

    global_end_time = time.time()
    print("\nDone!")
    print(f"Total time taken = {str(timedelta(seconds=int(global_end_time - global_start_time)))}\n")

if __name__ == "__main__":  main()
    
