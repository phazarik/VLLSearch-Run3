import os, sys
import argparse
import time
from datetime import date
from datetime import timedelta
from tqdm import tqdm
from rich.console import Console
console = Console(highlight=False)
print = console.print

parser = argparse.ArgumentParser()
parser.add_argument('--test', action='store_true')
parser.add_argument('--dryrun', action='store_true')
args = parser.parse_args()
test   = args.test
dryrun = args.dryrun

if test:   print('[WARNING]: test mode',   style="red")
if dryrun: print('[WARNING]: dryrun mode', style="red")
print()

channels = ["mm", "me", "em", "ee"]

jobdict = {
    "val/tree_val_2016preVFP_UL":{
        "dump":"hist_val_2016preVFP_UL",
        "campaign":"2016preVFP_UL"
    },
    "val/tree_val_2016postVFP_UL":{
        "dump":"hist_val_2016postVFP_UL",
        "campaign":"2016postVFP_UL"
    },
    "val/tree_val_2017_UL":{
        "dump":"hist_val_2017_UL",
        "campaign":"2017_UL"
    },
    "val/tree_val_2018_UL":{
        "dump":"hist_val_2018_UL",
        "campaign":"2018_UL"
    },
    "val/tree_val_Run3Summer22":{
        "dump":"hist_val_Run3Summer22",
        "campaign":"Run3Summer22"
    },
    "val/tree_val_Run3Summer22EE":{
        "dump":"hist_val_Run3Summer22EE",
        "campaign":"Run3Summer22EE"
    },
    "val/tree_val_Run3Summer23":{
        "dump":"hist_val_Run3Summer23",
        "campaign":"Run3Summer23"
    },
    "val/tree_val_Run3Summer23BPix":{
        "dump":"hist_val_Run3Summer23BPix",
        "campaign":"Run3Summer23BPix"
    }
}

#____________________________________________________________________________________________________
#____________________________________________________________________________________________________

today = date.today().strftime('%Y-%m-%d')
start_time = time.time()
jobcount = 0
processed = []

for jobname, info in jobdict.items():
    if 'Run3' in jobname: continue

    for channel in channels:
        #if channel != "ee" : continue
        
        jobcount += 1
        campaign = info["campaign"]
        dump = info["dump"]+"_"+channel

        arguments = [
            f'"{jobname}"',
            f'"{dump}"',
            f'"{campaign}"',
            f'"{channel}"'
        ]
        if test: arguments.append('true')
        if dryrun: arguments.append('true')
        arguments = ', '.join(arguments)

        command = f"root -q -b -l 'extractHistsFromTrees.C({arguments})'"
        print(f"\nJob #{jobcount}: {command}", style="yellow bold")

        if not dryrun:
            os.system(command)
            processed.append(dump)
        
        if test: break ##channel
    if test: break ##job

end_time = time.time()
time_taken = end_time - start_time

print("\nDone!", style='yellow bold')
if processed:
    print("Histograms are created in the following directories:")
    for name in processed: print(f"../ROOT_FILES/hists/{today}/{name}")

print(f"Total time taken = {str(timedelta(seconds=int(time_taken)))}\n", style='yellow bold')
    
