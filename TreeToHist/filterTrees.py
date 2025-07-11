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
#default = True

basedir = '../ROOT_FILES/treesWithNN/'

jobdict = {
    "baseline/tree_2016preVFP_UL_baseline":{
        "outjob":"topcr/tree_2016preVFP_UL_topcr",
        "campaign":"2016preVFP_UL"
    },
    "baseline/tree_2016postVFP_UL_baseline":{
        "outjob":"topcr/tree_2016postVFP_UL_topcr",
        "campaign":"2016postVFP_UL"
    },
    "baseline/tree_2017_UL_baseline":{
        "outjob":"topcr/tree_2017_UL_topcr",
        "campaign":"2017_UL"
    },
    "baseline/tree_2018_UL_baseline":{
        "outjob":"topcr/tree_2018_UL_topcr",
        "campaign":"2018_UL"
    },
    "baseline/tree_Run3Summer22_baseline":{
        "outjob":"topcr/tree_Run3Summer22_topcr",
        "campaign":"Run3Summer22"
    },
    "baseline/tree_Run3Summer22EE_baseline":{
        "outjob":"topcr/tree_Run3Summer22EE_topcr",
        "campaign":"Run3Summer22EE"
    },
    "baseline/tree_Run3Summer23_baseline":{
        "outjob":"topcr/tree_Run3Summer23_topcr",
        "campaign":"Run3Summer23"
    },
    "baseline/tree_Run3Summer23BPix_baseline":{
        "outjob":"topcr/tree_Run3Summer23BPix_topcr",
        "campaign":"Run3Summer23BPix"
    },
}

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

    jobcount += 1
    outjob = info["outjob"]
    campaign = info["campaign"]

    ### Picking the right DNN score for event selection:
    nnscore_qcd   = "nnscore_Run3_vlld_qcd"
    nnscore_ttbar = "nnscore_Run3_vlld_ttbar"
    nnscore_wjets = "nnscore_Run3_vlld_wjets"
    if 'Run3' not in campaign:
        nnscore_qcd   = "nnscore_Run2_vlld_qcd"
        nnscore_ttbar = "nnscore_Run2_vlld_ttbar"
        nnscore_wjets = "nnscore_Run2_vlld_wjets"

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

        dy_veto = 'not (channel == 3 and 76 < dilep_mass < 106)'
        
        ## Step1: Controlling QCD:
        qcd_cr = f'{dy_veto} and {nnscore_qcd}<0.30 and 0.02<lep0_iso<0.15 and lep0_sip3d>5'
        qcd_vr = f'{dy_veto} and {nnscore_qcd}<0.30 and 0.02<lep0_iso<0.15 and lep0_sip3d<5'
        tight_sip3d = 'lep0_sip3d<5 and lep1_sip3d<10'
        qcd_veto    = f'{tight_sip3d} and {dy_veto} and {nnscore_qcd}>0.30 and HT>50'
        
        ## Step2: Controlling Drell-Yan:
        dy_cr  = f'76<dilep_mass<106  and dilep_ptratio > 0.7'

        ## Step3: Controlling WJets:
        wjets_cr    = f'{qcd_veto} and {nnscore_wjets}<0.50 and nbjet==0'
        wjets_veto  = f'{qcd_veto} and {nnscore_wjets}>0.50' 
        
        ## Step4: Controlling TTbar:
        top_cr = f'{wjets_veto} and {nnscore_qcd}>0.70 and nbjet>0'
        
        ## Step5: Validation:
        val_region = f'{dy_veto} and 0.50<{nnscore_qcd}<0.70 and {nnscore_wjets}>0.50'
        val_region = val_region+f'and HT>50 and {tight_sip3d}'
        
        ## Step6: Signal regions:
        presr = f'{wjets_veto} and {nnscore_qcd}>0.70 and nbjet==0'
        sr = f'{presr} and ST>300'
        
        #------------------------------
        # Final event selection:
        event_selection = top_cr
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
