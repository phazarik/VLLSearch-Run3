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

'''
jobdict = {
    "baseline/tree_2016preVFP_UL_baseline":{
        "dump":"hist_2016preVFP_UL_baseline",
        "campaign":"2016preVFP_UL"
    },
    "baseline/tree_2016postVFP_UL_baseline":{
        "dump":"hist_2016postVFP_UL_baseline",
        "campaign":"2016postVFP_UL"
    },
    "baseline/tree_2017_UL_baseline":{
        "dump":"hist_2017_UL_baseline",
        "campaign":"2017_UL"
    },
    "baseline/tree_2018_UL_baseline":{
        "dump":"hist_2018_UL_baseline",
        "campaign":"2018_UL"
    }
}
'''

jobdict = {
    "baseline/tree_Run3Summer22_baseline":{
        "dump":"hist_Run3Summer22_baseline",
        "campaign":"Run3Summer22"
    },
    "baseline/tree_Run3Summer22EE_baseline":{
        "dump":"hist_Run3Summer22EE_baseline",
        "campaign":"Run3Summer22EE"
    }
}

#____________________________________________________________________________________________________
#____________________________________________________________________________________________________

today = date.today().strftime('%Y-%m-%d')
start_time = time.time()
jobcount = 0
processed = []

for jobname, info in jobdict.items():
    #if 'EE' not in jobname: continue
    for channel in channels:
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
    
