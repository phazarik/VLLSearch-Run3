import time
import re
from tqdm import tqdm
import os, argparse
from datetime import timedelta

parser = argparse.ArgumentParser()
parser.add_argument("--test", action="store_true")
parser.add_argument("--dryrun", action="store_true")
args = parser.parse_args()

## CONFIG ##
basedir = "../ROOT_FILES/hists/2025-07-01"
campaigns = ["Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix"]
channels = ["ee", "em", "me", "mm"]
flag = "dycr_corrected"
dump = f"hist_Run3_{flag}_combined"

## DON'T TOUCH BELOW ##
outdir = os.path.join(basedir, dump)
if not args.dryrun: os.makedirs(outdir, exist_ok=True)

folders = []
for camp in campaigns:
    for ch in channels:
        folder = f"hist_{camp}_{flag}_{ch}"
        fullpath = os.path.join(basedir, folder)
        if os.path.isdir(fullpath):
            folders.append(fullpath)

all_files = {}
for folder in folders:
    for fname in os.listdir(folder):
        if fname.endswith(".root"):
            all_files.setdefault(fname, []).append(os.path.join(folder, fname))


print("\n\033[33;1mInput folders:\033[0m")
for f in folders: print(f"- {f}")
print()

start_time = time.time()

count = 1
iterator_var = sorted(all_files.items())
wrapper = (
    tqdm(iterator_var,total=len(all_files),unit="file",desc="Combining",colour="green",ncols=100,leave=True,
        bar_format="{l_bar}{bar}| [{elapsed} < {remaining}, {n_fmt}/{total_fmt}]")
    if not args.dryrun else iterator_var
)

for fname, paths in wrapper:
    outpath = os.path.join(outdir, fname)

    if len(paths) == 1: cmd = f"cp {paths[0]} {outpath}"
    else: cmd = f"hadd -f {outpath} " + " ".join(paths)

    if args.dryrun:
        print(f"\n\033[36m({count}/{len(all_files)}) Combining files into \033[33;1m{outpath}:\033[0m")
        for p in paths: print(f"- {p}")
    
    if not args.dryrun: os.system(f"{cmd} > /dev/null 2>&1")
    count += 1
    if args.test: break

end_time = time.time()
time_taken = end_time - start_time

print("\nDone!")
print(f"\n\033[33;1mOutput: {outdir}\033[0m")
print(f"Time taken = {str(timedelta(seconds=int(time_taken)))}\n")
