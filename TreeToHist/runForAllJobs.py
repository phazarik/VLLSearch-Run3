import os, sys
import argparse
import time
from datetime import date
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
    "ttbarcr/tree_2016preVFP_UL_topcr":{
        "dump":"hist_2016preVFP_UL_topcr",
        "campaign":"2016preVFP_UL"
    },
    "ttbarcr/tree_2016postVFP_UL_topcr":{
        "dump":"hist_2016postVFP_UL_topcr",
        "campaign":"2016postVFP_UL"
    },
    "ttbarcr/tree_2017_UL_topcr":{
        "dump":"hist_2017_UL_topcr",
        "campaign":"2017_UL"
    },
    "ttbarcr/tree_2018_UL_topcr":{
        "dump":"hist_2018_UL_topcr",
        "campaign":"2018_UL"
    }
}

#____________________________________________________________________________________________________
#____________________________________________________________________________________________________

today = date.today().strftime('%Y-%m-%d')
start_time = time.time()
jobcount = 0
processed = []

for jobname, info in jobdict.items():
    #if 'post' not in jobname: continue
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
        print("\n"+command, style="yellow bold")

        if not dryrun:
            os.system(command)
            processed.append(dump)
        
        if test: break ##channel
    if test: break ##job

end_time = time.time()
time_taken = end_time - start_time
hours, rem = divmod(time_taken, 3600)
minutes, seconds = divmod(rem, 60)

print("\nDone!", style='yellow bold')
if processed:
    print("Histograms are created in the following directories:")
    for name in processed: print(f"../ROOT_FILES/hists/{today}/{name}")
print(f"Total time taken = {int(hours):02d}h {int(minutes):02d}m {int(seconds):02d}s\n", style='yellow bold')
    
