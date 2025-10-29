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

#----------------------------------- config ----------------------------------------
finalstate = "2LSS"
campaigns = ["2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
             "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix"]
channels  = ["mm", "ee"]
if finalstate == "2LSS": channels.extend(["me", "em"])
basedir   = f"{finalstate}_sr"
tag       = basedir #f"{finalstate}_killdy"
tagout    = basedir #f"{finalstate}_killdy" ## Shows up in hist directory names
jobdict = {}
for camp in campaigns:
    #if "Run3Summer22EE" not in camp: continue
    #if 'Run3' in camp: continue
    key = f"{basedir}/tree_{tag}_{camp}"
    jobdict[key] = {"dump": f"hist_{tagout}_{camp}", "campaign": camp}
#-----------------------------------------------------------------------------------

today = date.today().strftime('%Y-%m-%d')
start_time = time.time()
jobcount = 0
processed = []

for jobname, info in jobdict.items():
    for channel in channels:
        #if channel not in ["ee"]: continue ## for DY cr
        
        if finalstate == "2LOS":
            if channel not in ["ee", "mm"]: continue
        
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
    
