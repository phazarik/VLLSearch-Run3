import os, sys
import numpy as np
import uproot
import pandas as pd
import tensorflow as tf
import scipy.sparse as sparse #for numpy.array - pd.dataframe column conversion
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_curve,auc
from keras import layers
from tqdm  import tqdm
from datetime import timedelta
#from keras import Sequential
#from tensorflow.keras.layers import Dense
from rich.progress import Progress, BarColumn, TaskProgressColumn

import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--campaign', type=str, help='Specify the campaign!', required=True)
parser.add_argument('--dryrun', action='store_true', help='dry-run mode')  # False by default
parser.add_argument('--test', action='store_true', help='test mode')       # False by default

args = parser.parse_args()

#Global parameters:
campaign = args.campaign
indir = '../ROOT_FILES/trees/'
jobname = f'tree_baseline-JERdown_{campaign}'
outdir = f'../ROOT_FILES/treesWithNN/baseline-JERdown/tree_baseline_{campaign}'
os.makedirs(outdir, exist_ok=True)

modeldict = {
    'QCD-vs-VLLD Run2 (Aug13)'    : 'nnscore_Run2_vlld_qcd',
    'Top-vs-VLLD Run2 (Aug13)'    : 'nnscore_Run2_vlld_ttbar',
    'Wjets-vs-VLLD Run2 (Aug13)'  : 'nnscore_Run2_vlld_wjets',
    'DYjets-vs-VLLD Run2 (Aug13)' : 'nnscore_Run2_vlld_dy',
    'QCD-vs-VLLD Run3 (Aug13)'    : 'nnscore_Run3_vlld_qcd',
    'Top-vs-VLLD Run3 (Aug13)'    : 'nnscore_Run3_vlld_ttbar',
    'Wjets-vs-VLLD Run3 (Aug13)'  : 'nnscore_Run3_vlld_wjets',
    'DYjets-vs-VLLD Run3 (Aug13)' : 'nnscore_Run3_vlld_dy'
}

#-------------------------------------------
# DON'T TOUCH BELOW:
#-------------------------------------------
# Define functions:
# Given a TFile, read its branches into a dataframe.

def read_file_into_df(filepath, step_size=100000):
    with uproot.open(filepath) as tfile:
        if "myEvents" not in tfile:
            print(f"Warning: 'myEvents' tree not found in {filepath}. Skipping.")
            return pd.DataFrame()

        ttree = tfile["myEvents"]
        total_entries = ttree.num_entries

        if total_entries == 0:
            print(f"Warning: 'myEvents' tree in {filepath} is empty. Skipping.")
            return pd.DataFrame()

        total_batches = (total_entries + step_size - 1) // step_size
        dfs = []

        with Progress("[progress.description]{task.description}", BarColumn(), TaskProgressColumn()) as progress:
            task = progress.add_task("Reading", total=total_batches)
            for batch in ttree.iterate(ttree.keys(), step_size=step_size, library="pd"):
                dfs.append(batch)
                progress.update(task, advance=1)

        return pd.concat(dfs, ignore_index=True) if dfs else pd.DataFrame()
    
def write_df_into_file(df, filepath, step_size=100000):
    if df.empty:
        print(f"Warning: Attempting to write empty DataFrame to {filepath}. Skipping.")
        return

    total_entries = len(df)
    total_batches = (total_entries + step_size - 1) // step_size

    with tqdm(total=total_batches, unit="batch",
              bar_format="{l_bar}{bar}| {percentage:3.0f}% [{elapsed} < {remaining}, {n_fmt}/{total_fmt}]",
              leave=True, ncols=100, desc="Writing", colour='yellow') as pbar:
        
        with uproot.recreate(filepath) as file:
            for start in range(0, total_entries, step_size):
                chunk = df.iloc[start:start + step_size]
                if start == 0: file["myEvents"] = {col: chunk[col].to_numpy() for col in df.columns}
                else:          file["myEvents"].extend({col: chunk[col].to_numpy() for col in df.columns})
                pbar.update(1)

def ApplyMinMax(X, modelname):

    min_filename = f'trained_models/{modelname}/scaling_parameters_min.txt'
    max_filename = f'trained_models/{modelname}/scaling_parameters_max.txt'

    minval = np.loadtxt(min_filename)
    maxval = np.loadtxt(max_filename)
    diff = maxval - minval
    normed_X = X.copy()    
    # Scale the data only for non-constant columns
    nonconst = np.where(diff != 0)[0]
    normed_X[:, nonconst] = 2 * ((X[:, nonconst] - minval[nonconst]) / diff[nonconst]) - 1.0
    
    return normed_X
    
print('Functions loaded.')

#----------------------------------------------------------------------------------
# Evaluation beings here:
list_of_files = os.listdir(os.path.join(indir, jobname))

print('\nStarting evaluation ... (hold on, this will take a while)\n')
import time
start_time = time.time()

list_success = []
list_failed  = []

for f in list_of_files:

    #if "Muon" not in f: continue 

    #Step1: Prepare the dataframe
    filepath = os.path.join(indir, jobname, f)
    print(f"\n\n\033[33m==> {f.split('tree_')[1].split('.root')[0]} {'-'*90}\033[0m")
    print(f'Input file: {os.path.abspath(filepath)}')    
    sample = filepath.split("_")[1]
    subsample = "_".join(filepath.split("_")[2:])
    outfile = os.path.join(outdir, f)
    #if os.path.exists(outfile): continue
    df = read_file_into_df(filepath)
    if df.empty :
        print(f'\033[31mSkipping: File is empty: {filepath}\033[0m')
        list_failed.append(f)
        continue
    
    #Step2 Load the model:
    for modelname, scorename in modeldict.items():

        #Step 2.1: pick which variables to read, and turn those into a numpy array: 
        train_var = []
        if 'QCD' in modelname:
            train_var = [
                'njet',
                'dilep_dR',
                'dilep_ptratio',
                'LTplusMET',
                'ST',
                'STfrac',
                'metpt',
                'dphi_metdilep',
                'dphi_metlep_min'
            ]
        elif 'Wjets' in modelname:
            train_var =[
                'njet',
                'dilep_mt',
                'dilep_ptratio',
                'dilep_dR',
                'LTplusMET',
                'STfrac'
            ]
        elif 'Top' in modelname:
            train_var = [
                'nbjet',
                'dilep_ptratio',
                'LTplusMET',
                'ST',
                'STfrac',
                'dphi_metlep0',
            ]
        elif 'DY' in modelname:
            train_var = [
                'njet',
                'lep0_eta',
                'lep1_eta',
                'dilep_mt',
                'LTplusMET',
                'metpt',
            ]
        else: print('Error: Pick training variables first!')

        model_filename = f'trained_models/{modelname}/model_{modelname}.keras'
        model = tf.keras.models.load_model(model_filename)
        print(f'Model loaded: {modelname}')

        scores = []
        BATCH_SIZE = 1000000
        num_batches = (len(df) + BATCH_SIZE - 1) // BATCH_SIZE

        for i, start in enumerate(range(0, len(df), BATCH_SIZE), start=1):
            print(f'Preparing batch {i}/{num_batches} for evaluation')
            batch_df = df.iloc[start:start+BATCH_SIZE]
            X = batch_df[train_var].values.astype(np.float32)
            X_scaled = ApplyMinMax(X, modelname)
            if not args.dryrun:
                y = model.predict(X_scaled, batch_size=BATCH_SIZE)
                scores.append(y)

        if not args.dryrun: df[scorename] = np.vstack(scores)
        
        del model
        tf.keras.backend.clear_session() 
        #break #model

    if not args.dryrun: write_df_into_file(df, os.path.join(outdir, f))
    list_success.append(f)
    
    if not args.dryrun: print(f'\033[1;33mFile written: {os.path.abspath(outfile)}\033[0m')
    if args.test: break
    
end_time = time.time()
time_taken = str(timedelta(seconds=int(end_time-start_time)))

print(' \n\033[33mDone!\033[0m\n')
print('------------------------------------------------------------------------------')
print( '\033[33mSummary:\033[0m')
print(f'\033[33mSuccess : {list_success}\033[0m')
print(f'\033[31mEmpty   : {list_failed}\033[0m')
print(f'\033[33mOutput directory: {outdir}\033[0m')
print(f'\033[33mtime taken : {time_taken} seconds\033[0m')
print('-------------------------------------------------------------------------------\n')

    
