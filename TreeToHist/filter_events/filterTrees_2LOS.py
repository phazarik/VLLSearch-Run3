import os, sys
import time
from datetime import timedelta
import argparse
import numpy as np
import uproot
import pandas as pd
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

#----------------------------------- config ----------------------------------------
basedir   = '../../ROOT_FILES/treesWithNN/' ## relative path from this file.
campaigns = ["2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
             "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix"]
campaigns = ["Run3Summer22EE"]
basename  = "2LOS_baseline/tree_2LOS_baseline"
dumpdir   = "2LOS_cleanup"
tag       = "2LOS_cleanup"
#-----------------------------------------------------------------------------------

basedir = os.path.join(os.path.dirname(os.path.realpath(__file__)), basedir)
basedir = os.path.abspath(basedir)

jobdict = {}
for camp in campaigns:
    key = f"{basename}_{camp}"
    jobdict[key] = {"outjob": f"{dumpdir}/tree_{tag}_{camp}", "campaign": camp}

def read_file_into_df(filepath, step_size=100000, test=False):
    with uproot.open(filepath) as tfile:
        if "myEvents" not in tfile:
            if test: print(f"Warning: 'myEvents' tree not found in {filepath}.", style='red')
            return pd.DataFrame()
        ttree = tfile["myEvents"]
        total_entries = ttree.num_entries
        if total_entries == 0:
            if test: print(f"Warning: 'myEvents' tree in {filepath} is empty.", style='red')
            return pd.DataFrame()
        dfs = []
        for batch in ttree.iterate(ttree.keys(), step_size=step_size, library="pd"):
            dfs.append(batch)
    return pd.concat(dfs, ignore_index=True) if dfs else pd.DataFrame()

def write_df_into_file(df, filepath, step_size=100000):
    if df.empty:
        if test: print(f"Warning: Attempting to write empty DataFrame to {filepath}.", style='red')
        return
    total_entries = len(df)
    with uproot.recreate(filepath) as file:
        for start in range(0, total_entries, step_size):
            chunk = df.iloc[start:start + step_size]
            if start == 0: file["myEvents"] = {col: chunk[col].to_numpy() for col in df.columns}
            else:          file["myEvents"].extend({col: chunk[col].to_numpy() for col in df.columns})

print('Functions loaded.')

#___________________________________________________________________________________________________
#___________________________________________________________________________________________________

start_time = time.time()
jobcount = 0
filecount = 0

for injob, info in jobdict.items():

    outjob = info["outjob"]
    campaign = info["campaign"]

    jobcount += 1
    
    ### Picking the right DNN score for event selection:
    nnscore_ttbar = "nnscore_2LOS_Run3_vlld_ttbar"
    nnscore_dy    = "nnscore_2LOS_Run3_vlld_dy"
    if 'Run3' not in campaign:
        nnscore_ttbar = "nnscore_2LOS_Run2_vlld_ttbar"
        nnscore_dy    = "nnscore_2LOS_Run2_vlld_dy"

    print(f"\n({jobcount}/{len(list(jobdict.items()))}) injob = {injob}, outjob = {outjob}, campaign = {campaign}", style="yellow")
    
    indir = os.path.join(basedir, injob)
    outdir = os.path.join(basedir, outjob)
    if not dryrun: os.makedirs(outdir, exist_ok=True)

    list_of_files = os.listdir(indir)
    list_empty_input = []
    list_empty_output = []

    count = 0
    for f in tqdm(list_of_files, desc="Processing files", unit="file",
              bar_format="{l_bar}{bar}| {percentage:3.0f}% [{elapsed} < {remaining}, {n_fmt}/{total_fmt}]",
              leave=True, ncols=100, colour='blue', mininterval=0.5):

        if not f.endswith('.root'): continue
        if test: print(f'TEST: Processing file: {f}')
        filepath = os.path.join(indir, f)
        sample = f.split("_")[1]
        subsample = "_".join(f.split("_")[2:])
        outfile = os.path.join(outdir, f)
        df = read_file_into_df(filepath)
        if df.empty : 
            list_empty_input.append(filepath)
            continue

        ####################################
        #         EVENT SELECTION          #
        ####################################

        tight_iso   = 'lep0_iso<0.05 and lep1_iso<0.2'
        tight_sip3d = 'lep0_sip3d<5 and lep1_sip3d<10'
        cleanup = f"{tight_iso} and {tight_sip3d} and dilep_deta < 2.5 and HT>50"      
        
        ## Step1: control and kill DY
        dycr    = f"{cleanup} and {nnscore_dy}<0.2"
        killdy  = f"{cleanup} and {nnscore_dy}>0.8 and abs(dilep_eta)<5"

        ## Step2: ttbar CR
        topcr     = f"{killdy} and {nnscore_ttbar}<0.2"
        killttbar = f"{killdy} and {nnscore_ttbar}>0.8"

        ## Step3: Validation regions
        val1 = f"{cleanup} and     {nnscore_ttbar}<0.2 and 0.2<{nnscore_dy}<0.8" ## low tt-score
        val2 = f"{cleanup} and 0.2<{nnscore_ttbar}<0.6 and 0.2<{nnscore_dy}"     ## med tt-score
        val3 = f"{cleanup} and 0.8<{nnscore_ttbar}     and 0.2<{nnscore_dy}<0.6" ## high tt-score

        ## Step4: signal region
        srpre = f"{killttbar}"
        sr = f"{srpre} and LTplusMET>400"
        
        #------------------------------
        # Final event selection:
        event_selection = cleanup
        #------------------------------

        filecount += 1
        count += 1
        df_filtered = df.query(event_selection)
        if df_filtered.empty: list_empty_output.append(f'{sample}_{subsample}')
        nbefore = len(df)
        nafter = len(df_filtered)
        frac = 0
        if nbefore != 0: frac = nafter*100/nbefore
        #display(df_filtered)
        if not dryrun: write_df_into_file(df_filtered, outfile)
        if test:
            print(f'TEST: File written: {outfile} ({nbefore} -> {nafter}, {frac:.2f}%)')
            break ##file
         
    print(f'Summary:')
    print(f"{count} file(s) written to : {outdir}", style='yellow')
    if list_empty_input:
        print(f"Following {len(list_empty_input)} input files were empty!", style="red")
        for name in list_empty_input: print(f" - {name}")
    if list_empty_output:
        print(f"Following {len(list_empty_output)} output files are empty!", style="red")
        for name in list_empty_output: print(f" - {name}")
    if test: break ##job
    
end_time = time.time()
time_taken = timedelta(seconds=round(end_time - start_time))

print("\nDone!", style='yellow bold')
print(f"Time taken = {time_taken}",              style='yellow bold')
print(f"Total number of jobs  = {jobcount}",     style='yellow bold')
print(f"Total number of files = {filecount}\n",  style='yellow bold')
