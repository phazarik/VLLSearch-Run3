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
#from keras import Sequential
#from tensorflow.keras.layers import Dense

#Global parameters:
indir = '../ROOT_FILES/trees/'
jobname = 'tree_Run3Summer22EE_baseline_Apr25'
outdir = f'../ROOT_FILES/treesWithNN/baseline/tree_Run3Summer22EE_baseline'
os.makedirs(outdir, exist_ok=True)

modeldict = {
    'qcd-vs-vlld-comb-2016postVFP_UL-feb07' : 'nnscore_qcd_vlld_2016postVFP',
    'qcd-vs-vlld-comb-2016preVFP_UL-feb07'  : 'nnscore_qcd_vlld_2016preVFP',
    'qcd-vs-vlld-comb-2017_UL-feb07'        : 'nnscore_qcd_vlld_2017',
    'qcd-vs-vlld-comb-2018_UL-feb07'        : 'nnscore_qcd_vlld_2018',
    'qcd-vs-vlld-comb-Run3Summer22-apr08'   : 'nnscore_qcd_vlld_2022',
    'qcd-vs-vlld-comb-Run3Summer22EE-apr26' : 'nnscore_qcd_vlld_2022EE',
    'ttbar-vs-vlld-comb-feb13'              : 'nnscore_ttbar_vlld',
}

#-------------------------------------------
# DON'T TOUCH BELOW:

#-------------------------------------------
# Define functions:
#Given a TFile, read its branches into a dataframe.
'''
def read_file_into_df(filepath, truth=None):

    tfile = uproot.open(filepath)
    
    ttree = tfile['myEvents']
    branches = ttree.keys()
    awkarray = ttree.arrays(branches)
    df = pd.DataFrame(awkarray.to_list())

    return df

def write_df_into_file(df, filepath):
    if df.empty:
        data_dict = {col: np.array([], dtype=df[col].dtype) for col in df.columns}
        print(f"\033[0;31mWarning: Writing empty file: {filepath}\033[0m\n")
    else:
        data_dict = df.to_dict('list')
        
    with uproot.recreate(filepath) as file: file['myEvents'] = data_dict
'''

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

        with tqdm(total=total_batches, unit="batch",
                  bar_format="{l_bar}{bar}| [{elapsed} < {remaining}, {n_fmt}/{total_fmt}]",
                  leave=True, ncols=100, desc="Reading", colour='green') as pbar:
            for batch in ttree.iterate(ttree.keys(), step_size=step_size, library="pd"):
                dfs.append(batch)
                pbar.update(1)

    return pd.concat(dfs, ignore_index=True) if dfs else pd.DataFrame()

def write_df_into_file(df, filepath, step_size=100000):
    if df.empty:
        print(f"Warning: Attempting to write empty DataFrame to {filepath}. Skipping.")
        return

    total_entries = len(df)
    total_batches = (total_entries + step_size - 1) // step_size

    with tqdm(total=total_batches, unit="batch", 
              bar_format="{l_bar}{bar}| {percentage:3.0f}% [{elapsed} < {remaining}, {n_fmt}/{total_fmt}]",
              leave=True, ncols=100, desc="Writing", colour='green') as pbar:
        
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

    #if "DY" not in f: continue 

    #Step1: Prepare the dataframe
    print(f'\nLoading file: {f}')
    filepath = os.path.join(indir, jobname, f)
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

        #if modelname in ['qcd-vs-vlld-mu-m200-800-oct21', 'qcd-vs-vlld-ele-m200-800-oct21', 'qcd-vs-vlld-mu-m100-oct21', 'qcd-vs-vlld-ele-m100-oct21']:
        if 'qcd' in modelname:
            train_var = [
                'njet',
                'dilep_dR',
                'dilep_ptratio',
                'HT',
                'LT',
                'STfrac',
                'metpt',
                'dphi_metlep0',
                'dphi_metdilep'
            ]
        elif 'ttbar' in modelname:
            train_var = [
                'dilep_pt',
                'dilep_mt',
                'dilep_dphi',
                'dilep_ptratio',
                'LT',
                'ST',
                'STfrac',
                'dphi_metlep0',
                'dphi_metlep1',
                'dphi_metlep_min'
            ]
        else: print('Error: Pick correct training variables!')

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
            y = model.predict(X_scaled, batch_size=BATCH_SIZE)
            scores.append(y)

        df[scorename] = np.vstack(scores)
        
        del model
        tf.keras.backend.clear_session() 
        #break #model

    write_df_into_file(df, os.path.join(outdir, f))
    list_success.append(f)
    
    print(f'\033[1;33mFile written: {outfile}\033[0m')
    #break #file
    
end_time = time.time()
time_taken = end_time-start_time

print(' \n\033[33mDone!\033[0m\n')
print('------------------------------------------------------------------------------')
print( '\033[33mSummary:\033[0m')
print(f'\033[33mSuccess : {list_success}\033[0m')
print(f'\033[31mEmpty   : {list_failed}\033[0m')
print(f'\033[33mOutput directory: {outdir}\033[0m')
print(f'\033[33mtime taken : {time_taken:.2f} seconds\033[0m')
print('-------------------------------------------------------------------------------\n')

    
