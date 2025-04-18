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
    "qcdcr/tree_2018_UL_qcdcr":{
        "dump":"hist_2018_UL_qcdcr",
        "campaign":"2018_UL"
    },
    "qcdvr/tree_2018_UL_qcdvr":{
        "dump":"hist_2018_UL_qcdvr",
        "campaign":"2018_UL"
    } 
}

#______________________________________________________________________________________________________________
#______________________________________________________________________________________________________________

today = date.today().strftime('%Y-%m-%d')
start_time = time.time()
jobcount = 0
processed = []

for jobname, info in jobdict.items():
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

print("\nDone!", style='yellow bold')
if processed:
    print("Histograms are created in the following directories:")
    for name in processed: print(f"../ROOT_FILES/hists/{today}/{name}")
print(f"Total time taken = {time_taken:.2f} seconds.\n", style='yellow bold')
    
