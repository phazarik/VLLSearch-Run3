import os, sys
import time
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
default = True

basedir = '../ROOT_FILES/treesWithNN/'

jobdict = {
    "baseline/tree_2016preVFP_UL_baseline":{
        "outjob":"sigregion/tree_2016preVFP_UL_sigregion",
        "campaign":"2016preVFP_UL",
        "nnscore":"nnscore_qcd_vlld_2016preVFP"
    },
    "baseline/tree_2016postVFP_UL_baseline":{
        "outjob":"sigregion/tree_2016postVFP_UL_sigregion",
        "campaign":"2016postVFP_UL",
        "nnscore":"nnscore_qcd_vlld_2016postVFP"
    },
    "baseline/tree_2017_UL_baseline":{
        "outjob":"sigregion/tree_2017_UL_sigregion",
        "campaign":"2017_UL",
        "nnscore":"nnscore_qcd_vlld_2017"
    },
    "baseline/tree_2018_UL_baseline":{
        "outjob":"sigregion/tree_2018_UL_sigregion",
        "campaign":"2018_UL",
        "nnscore":"nnscore_qcd_vlld_2018"
    },
}

jobdict = {
    "baseline/tree_Run3Summer22_baseline":{
        "outjob":"qcdcr/tree_Run3Summer22_qcdcr",
        "campaign":"Run3Summer22",
        "nnscore":"nnscore_qcd_vlld_2022"
    },
    "baseline/tree_Run3Summer22EE_baseline":{
        "outjob":"qcdcr/tree_Run3Summer22EE_qcdcr",
        "campaign":"Run3Summer22EE",
        "nnscore":"nnscore_qcd_vlld_2022EE"
    }
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
    nnscore = info["nnscore"]

    print(f"\n({jobcount}/{len(list(jobdict.items()))}) injob = {injob}, outjob = {outjob}, campaign = {campaign}, nnscore = {nnscore}", style="yellow")
    
    indir = os.path.join(basedir, injob)
    outdir = os.path.join(basedir, outjob)
    if not dryrun: os.makedirs(outdir, exist_ok=True)

    list_of_files = os.listdir(indir)
    list_empty_input = []
    list_empty_output = []

    count = 0
    for f in tqdm(list_of_files, desc="Processing files", unit="file",
                  colour='green', dynamic_ncols=False, mininterval=0.5):

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

        #Step1: Controlling QCD:
        dy_veto = 'not (channel == 3 and 76 < dilep_mass < 106)'
        qcd_cr = f'{dy_veto} and {nnscore}<0.30 and 0.02<lep0_iso<0.15 and lep0_sip3d>5'
        qcd_vr = f'{dy_veto} and {nnscore}<0.30 and 0.02<lep0_iso<0.15 and lep0_sip3d<5'
        
        #Step2: Controlling Drell-Yan:
        dycr  = f'76<dilep_mass<106  and dilep_ptratio > 0.7'

        #Step3: Filter bad events:
        tight_sip3d =  'lep0_sip3d<5 and lep1_sip3d<10'
        good_events = f'{tight_sip3d} and {dy_veto} and {nnscore}>0.30 and HT>50' 
        
        #Step4: Controlling TTbar:
        top_cr = f'{good_events} and {nnscore}>0.70 and nbjet>0'    
        
        #Step5: Validation:
        val_region = f'{good_events} and 0.50<{nnscore}<0.70'
        
        #Step6: Signal regions:
        sr = f'{good_events} and {nnscore}>0.70 and nbjet==0'
        
        #------------------------------
        # Final event selection:
        event_selection = qcd_cr
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
time_taken = end_time - start_time
hours, rem = divmod(time_taken, 3600)
minutes, seconds = divmod(rem, 60)

print("\nDone!", style='yellow bold')
print(f"Time taken = {int(hours):02d}h {int(minutes):02d}m {int(seconds):02d}s\n", style='yellow bold')
print(f"Time taken = {time_taken:.2f} seconds.", style='yellow bold')
print(f"Total number of jobs  = {jobcount}",     style='yellow bold')
print(f"Total number of files = {filecount}\n",  style='yellow bold')
