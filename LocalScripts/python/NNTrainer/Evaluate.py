import os, sys
import numpy as np
import uproot
import pandas as pd
import tensorflow as tf
import scipy.sparse as sparse #for numpy.array - pd.dataframe column conversion
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_curve,auc
from keras import layers
#from keras import Sequential
#from tensorflow.keras.layers import Dense

#Global parameters:
jobname = 'tree_2LSS_2018UL_baseline_Oct16'
indir = '../../input_trees'
outdir = f'../../input_trees_modified/tree_2LSS_2018UL_baseline_Nov12'
os.makedirs(outdir, exist_ok=True)

modeldict = {
    'qcd-vs-vlld-ele-m100-oct21'    :'nnscore_qcd_vlldele_100',
    'qcd-vs-vlld-ele-m200-800-oct21':'nnscore_qcd_vlldele_200_800',
    'qcd-vs-vlld-mu-m100-oct21'     :'nnscore_qcd_vlldmu_100',
    'qcd-vs-vlld-mu-m200-800-oct21' :'nnscore_qcd_vlldmu_200_800'
}

#-------------------------------------------
# DON'T TOUCH BELOW:

#-------------------------------------------
# Define functions:
#Given a TFile, read its branches into a dataframe.
def read_file_into_df(filepath, truth=None):

    tfile = uproot.open(filepath)
    
    ttree = tfile['myEvents']
    branches = ttree.keys()
    awkarray = ttree.arrays(branches)
    df = pd.DataFrame(awkarray.to_list())

    return df

def ApplyMinMax(X, modelname):

    min_filename = f'{modelname}/scaling_parameters_min.txt'
    max_filename = f'{modelname}/scaling_parameters_max.txt'

    # Load min values from the file
    minval = np.loadtxt(min_filename)
    # Load max values from the file
    maxval = np.loadtxt(max_filename)
    
    #print('Min from txt: ', minval)
    #print('Max from txt: ', maxval)
    
    # Calculate the difference
    diff = maxval - minval
    normed_X = X.copy()    
    # Scale the data only for non-constant columns
    nonconst = np.where(diff != 0)[0]
    normed_X[:, nonconst] = 2 * ((X[:, nonconst] - minval[nonconst]) / diff[nonconst]) - 1.0
    
    return normed_X

def write_df_into_file(df, filepath):
    if df.empty:
        data_dict = {col: np.array([], dtype=df[col].dtype) for col in df.columns}
        print(f"\033[0;31mWarning: Writing empty file: {filepath}\033[0m\n")
    else:
        data_dict = df.to_dict('list')
        
    with uproot.recreate(filepath) as file: file['myEvents'] = data_dict

print('Functions loaded.')

#-----------------------------------------
# Load models and scaling parameters once
models = {}
scaling_params = {}
for modelname, scorename in modeldict.items():
    model_filename = f'{modelname}/model_{modelname}.keras' 
    models[modelname] = tf.keras.models.load_model(model_filename)
    print(f'Model loaded: {modelname}')

print('Models loaded.')

#----------------------------------------------------------------------------------
# Evaluation beings here:
list_of_files = os.listdir(os.path.join(indir, jobname))
#train_var = ['njet', 'nbjet', 'dilep_mt', 'dilep_dR', 'HTMETllpt', 'STfrac', 'dphi_metdilep', 'dphi_metlep_max', 'dphi_metlep_min']
#train_var = ['njet', 'dilep_mt', 'dilep_dR', 'dilep_dphi', 'HTMETllpt', 'HT', 'STfrac', 'dphi_metlep0', 'dphi_metdilep', 'dphi_metlep_max', 'dphi_metlep_min']
# The list of training variables has to match with the trainning part.

print('\nStarting evaluation ... (hold on, this will take a while)\n')
import time
start_time = time.time()

list_success = []
list_failed  = []

for f in list_of_files:

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

        if modelname in ['qcd-vs-vlld-mu-m200-800-oct21', 'qcd-vs-vlld-ele-m200-800-oct21', 'qcd-vs-vlld-mu-m100-oct21', 'qcd-vs-vlld-ele-m100-oct21']:
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
        
        X= df[train_var].values
        
        print(f'Predicting score for model: {modelname}')
        X_scaled = ApplyMinMax(X, modelname)
        print(f'X is scaled with min-max values for model: {modelname}')
            
        # Predict the scores and add as a new column
        y = models[modelname].predict(X_scaled)
        df[scorename] = y

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

    
