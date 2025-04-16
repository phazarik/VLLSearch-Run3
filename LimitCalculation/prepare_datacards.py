#-----------------------------------------------------------------------------------------------------------------
# This reads from the yield .txt files from the StackPlotMaker directory and prepares datacards for cms-combine.
# https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/latest/tutorial_stat_routines/stat_routines/
#-----------------------------------------------------------------------------------------------------------------

import os, sys, argparse
import json
import numpy as np
import pandas as pd
from tabulate import tabulate
from rich.console import Console
console = Console(highlight=False)
print = console.print
from datetime import datetime
import ROOT

parser = argparse.ArgumentParser()
parser.add_argument("--test", action="store_true", help="Run in test mode (print the command but don't execute)")
args = parser.parse_args()
test = args.test

def main(test=False):

    basedir = '../StackedPlotMaker/signalyields/2025-04-16/'
    jobs_to_combine = ['yields_2017_UL_em', 'yields_2017_UL_me', 'yields_2017_UL_mm', 'yields_2018_UL_em', 'yields_2018_UL_me', 'yields_2018_UL_mm']
    #jobs_to_combine = ['yields_2017_UL_mm']
    outtag = "_combined_2017_2018"
    #outtag = "_"+jobs_to_combine[0].split('yields_')[1]
    
    ## signals:
    signal_ele = {
        'VLLD_ele': {
            'M100': {'mass': 100, 'xsec': 16.9,       'ngen': 110871, 'scale':10},
            'M200': {'mass': 200, 'xsec': 1.36,       'ngen': 73730 , 'scale':1},
            'M300': {'mass': 300, 'xsec': 0.291,      'ngen': 24753 , 'scale':1},
            'M400': {'mass': 400, 'xsec': 0.0907,     'ngen': 24491 , 'scale':1},
            'M600': {'mass': 600, 'xsec': 0.0149,     'ngen': 24611 , 'scale':1},
            'M800': {'mass': 800, 'xsec': 0.00347,    'ngen': 23680 , 'scale':1},
            'M1000': {'mass': 1000, 'xsec': 0.000971, 'ngen': 24286 , 'scale':1}
        }
    }
    signal_mu = {
        'VLLD_mu': {
            'M100': {'mass': 100, 'xsec': 16.9,    'ngen': 111926, 'scale':10},
            'M200': {'mass': 200, 'xsec': 1.36,    'ngen': 73908,  'scale':1},
            'M300': {'mass': 300, 'xsec': 0.291,   'ngen': 25022,  'scale':1},
            'M400': {'mass': 400, 'xsec': 0.0907,  'ngen': 24299 , 'scale':1},
            'M600': {'mass': 600, 'xsec': 0.0149,  'ngen': 24890,  'scale':1},
            'M800': {'mass': 800, 'xsec': 0.00347, 'ngen': 24763,  'scale':1}
        }
    }

    prepare_datacard(signal_mu,  jobs_to_combine, basedir, outtag, filtersig = 0.0001)
    prepare_datacard(signal_ele, jobs_to_combine, basedir, outtag, filtersig = 0.0001)
    print("\nDone!\n", style="yellow bold")
    
def prepare_datacard(sigdict, joblist, baseinputdir, tag="", filtersig=0):

    signame = list(sigdict.keys())[0]
    print(f'\n{"-"*35}\nPreparing datacards for {signame}\n{"-"*35}', style="bold yellow")
    outputname = f'datacards_{signame}{tag}'
    today = datetime.now().strftime('%Y-%m-%d')
    outdir = f'datacards/{today}/{outputname}'

    ### Read text files into dataframes:
    datadict = {}

    print(f'Reading job(s):', end=' ')
    for jobname in joblist:
        print(jobname, end=' ')
        dict_job = return_dict(jobname, baseinputdir, sigdict, signame)

        for sample, subdict in dict_job.items():
            if sample not in datadict: datadict[sample] = {}
            for subsample, columns in subdict.items():
                if subsample not in datadict[sample]:
                    datadict[sample][subsample] = {
                        'bin': np.array([]),
                        'sig': np.array([]),
                        'obs': np.array([]),
                        'exp': np.array([]),
                        'experr': np.array([]),
                        'S/sqrtB': np.array([]),
                        'dbkg': np.array([]),
                    }
                for col in columns:
                    datadict[sample][subsample][col] = np.append(datadict[sample][subsample][col], columns[col])
                    
    print('\nData ready.')

    ### Use the dataframe from each mass point to create a text file.
    count = 0
    processed = []
    for sample, subs in sigdict.items():
        if sample not in datadict: continue
        for subsample, val in subs.items():
            if subsample not in datadict[sample]: continue

            count+= 1
            sampleyield = datadict[sample][subsample]
            sample_df = pd.DataFrame(sampleyield)
            sample_df['bin'] = sample_df['bin'].astype(int)
            sample_df['obs'] = sample_df['obs'].astype(int)
            sample_df = sample_df.sort_values(by='S/sqrtB', ascending=False).reset_index(drop=True)

            # Filtering the dataframe.
            if filtersig > 0: sample_df = sample_df[sample_df['sig']>filtersig]
            if "400" in subsample:
                print(f'Table for {sample} {subsample}:', style="yellow")
                print(tabulate(sample_df, headers='keys', tablefmt='psql'))
            if filtersig > 0:
                print(f"Filter: nsig > {filtersig} for {sample} {subsample}")
            
            datacard_name = f'datacard_{sample}_{subsample}.txt'
            datacard_path = os.path.join(outdir, datacard_name)

            if not test:
                newfile = write_datacard(sample_df, datacard_path)
                processed.append(newfile)

    if len(processed) > 0:
        print(f"\n{len(processed)} files created:")
        for index, filename in enumerate(processed): print(f"{index+1}. {filename}", style="yellow")

#______________________________________________________________________________________________________

#                                      DO NOT TOUCH BELOW
#______________________________________________________________________________________________________

def return_dict(jobname, baseinputdir, sigdict, signame):
    indir = os.path.join(baseinputdir, jobname)
    df = {}

    for sample, subdict in sigdict.items():
        if sample not in signame: continue
        if sample not in df: df[sample] = {}
            
        for subsample, val in subdict.items():
            filename = f'yield_{sample}_{subsample}.txt'
            filepath = os.path.join(indir, filename)

            if not os.path.exists(filepath):
                print(f'\033[33mWarning: File not found: {filepath}\033[0m')
                continue

            try:
                temp_df = pd.read_csv(
                    filepath,
                    sep=r'\s+',  # Split by one or more spaces
                    names=['bin', 'sig', 'obs', 'exp', 'experr', 'S/sqrtB', 'dbkg'],
                )
                #initialize numpy arrays for the columns
                if subsample not in df[sample]:
                    df[sample][subsample] = {
                        'bin': np.array([]),
                        'sig': np.array([]),
                        'obs': np.array([]),
                        'exp': np.array([]),
                        'experr': np.array([]),
                        'S/sqrtB': np.array([]),
                        'dbkg': np.array([]),
                    }
                # Append data from temp_df to the numpy arrays
                for col in temp_df.columns: df[sample][subsample][col] = np.append(df[sample][subsample][col], temp_df[col].values)

            except Exception as e: print(f'Could not read into df: {filepath}', style="red")

            #break #subsample
        #break  #sample
    return df

def write_datacard(df, datacard):
    df = df.reset_index(drop=True)
    num_bins = len(df)  # Total number of bins
    
    if num_bins == 0:
        print(f'Warning: Zero bins detected! Skipping file {datacard}', style="red")
        return ""

    os.makedirs(os.path.dirname(datacard), exist_ok=True)
    with open(datacard, 'w') as f:
        # Header information
        f.write(f"imax {num_bins}                          # number of channels\n")
        f.write(f"jmax 1                           # number of backgrounds\n")
        f.write(f"kmax {num_bins}                          # number of nuisance parameters\n")
        f.write("------------\n")
        
        # Bin section
        f.write(f"{'bin':<16}")
        line = ""
        for i in range(num_bins): line += f"bin{i + 1}\t"
        line = line[:-1]
        f.write(line + "\n")
        
        # Observation section
        f.write(f"{'observation':<16}")
        line = ""
        for i in range(num_bins): line += f"{int(df['obs'].iat[i])}\t"
        line = line[:-1]
        f.write(line + "\n")
        f.write("------------\n")

        # Bin-Bin section
        f.write(f"{'bin':<16}")
        line = ""
        for i in range(num_bins): line += f"bin{i + 1}\tbin{i + 1}\t"
        line = line[:-1]
        f.write(line + "\n")
        
        # Process section
        f.write(f"{'process':<16}")
        line = ""
        for i in range(num_bins): line += "sig\tbkg\t"
        line = line[:-1]
        f.write(line + "\n")

        # Process ID section:
        f.write(f"{'process':<16}")
        line = ""
        for i in range(num_bins): line += f"{-1*(i + 1)}\t{(i + 1)}\t"
        line = line[:-1]
        f.write(line + "\n")

        # Rate section
        f.write(f"{'rate':<16}")
        line = ""
        for i in range(num_bins): line += f"{df['sig'].iat[i]:.2f}\t{df['exp'].iat[i]:.2f}\t"
        line = line[:-1]
        f.write(line + "\n")
        f.write("------------\n")

        # Uncertainty:
        for i in range(num_bins):
            uncertainty_line = f"xs{i + 1:<6}lnN\t"
            values = []
            for j in range(num_bins):
                if j == i: # Diagonal element
                    values.append("-")  # Signal uncertainty
                    uncertainty_value = df['dbkg'].iat[i]
                    values.append(f"{uncertainty_value:.5f}") # Background uncertainty
                else:
                    values.append("-") # Signal uncertainty
                    values.append("-") # Background uncertainty
            uncertainty_line += "\t".join(values)
            f.write(uncertainty_line + "\n")
            
    return datacard
    
if __name__ == "__main__" : main()
