#-----------------------------------------------------------------------------------------------------
# This takes all the Physics processes mentioned in the lumi-json and runs compile_and_run for them.
# example usage: python3 processAllSamples.py --jobname test --dryrun --debug
#-----------------------------------------------------------------------------------------------------

import os, sys, argparse, glob
import json
import time
from datetime import timedelta
from rich.console import Console
console = Console(highlight=False)
print = console.print
start_time = time.time()

### Picking the arguments and flags:
parser = argparse.ArgumentParser()
parser.add_argument('--jobname', type=str, required=True, help='Example: hist_2LSS_2018UL_baseline')
parser.add_argument('--dryrun', action='store_true', help='Check if everything is correct before submitting')
parser.add_argument('--test', action='store_true', help='Check one job')
parser.add_argument('--debug', action='store_true', help='For printing information')
args=parser.parse_args()

dryrun  = args.dryrun  ### Just run this script without submitting any jobs.
jobname = args.jobname ### Example: hist_2LSS_2018UL_baseline
test    = args.test    ### Process only one sample.
debug   = args.debug   ### For debug statements.

#---------------------------------------------
#  SET GLOBAL PARAMETERS BEFORE RUNNING
#---------------------------------------------

campaign = "2018_UL"
### Options for Run2: 2016preVFP_UL, 2016postVFP_UL, 2017_UL, 2018_UL
### Options for Run3: Run3Summer22, Run3Summer22EE, Run3Summer23, Run3Summer23BPix

mode = "TreeMaker"
nanoAODv = 11

samples_to_run = ["DYGToLLG", "DYto2L", "Higgs", "QCDEM", "QCDMu", "RareTop", "ST", "TT", "TTV", "TW", "VV", "VVSS", "VVV", "WGtoLNuG", "WtoLNu", "ZGamma"]
if "Summer23" in campaign: samples_to_run.extend(["Muon0", "Muon1", "EGamma0", "EGamma1"])
else: samples_to_run.extend(["Muon", "EGamma"])
samples_to_run.extend(["VLLD-ele", "VLLD_mu"])

#if nanoAODv==11:
    #samples_to_run=["Muon", "EGamma", "RareTop", "VLLS_ele", "VLLS_mu", "VLLD_ele", "VLLD_mu"]
    #samples_to_run=["RareTop"]
    #samples_to_run=["VLLS_ele", "VLLS_mu", "VLLD_ele", "VLLD_mu"]
    #samples_to_run=["QCD_MuEnriched", "QCD_EMEnriched"] 
    
### Absolute paths:
dumpdir = "ROOT_FILES/trees/"
if mode == "HistMaker": dumpdir = "ROOT_FILES/hists/"
nanoAODpath = f"/mnt/d/work/MC_samples_skimmed/skimmed_2LSS_{campaign}"
codedir = "/mnt/d/work/GitHub/VLLSearch-Run3/AnalysisScripts"

#---------------------------------------------
# DON'T TOUCH BELOW
#---------------------------------------------

## Cleaning up:
clean = f'python3 {codedir}/{mode}/cleanup.py'
if test and not dryrun:
    print("Cleaning the code directory ..")
    os.system(clean)

print("Code directory ready.")

jsonfilepath = 'LumiJsons'
jsonfile = os.path.join(jsonfilepath, f'lumidata_{campaign}.json')
if not os.path.exists(jsonfilepath): print(f'Error: File not found: {jsonfile}', style='red')
with open(jsonfile,'r') as jsondata: samplelist = json.load(jsondata)
if debug: print(jsondata)

#-----------------------
# Looping over samples
#-----------------------

njobs=0
nfiles=0
list_processed=[]
list_failed=[]

for item in samples_to_run:
    for sample, subs in samplelist.items():
        
        if sample != item: continue
        #if 'DYG' not in sample: continue
        #if sample not in ['QCDEM', 'QCDMu', 'VVSS', 'WGtoLNuG', 'WtoLNu', 'ZGamma']: continue
        #if 'VLL' not in sample: continue
        #if 'RareTop' not in sample: continue
        #if not ('Muon' in sample or 'EGamma' in sample): continue
        
        print('\n----------------------------------------------')
        print(f'Submitting jobs for {sample}', style='yellow bold')
        print('----------------------------------------------')

        flag = 'mc'
        if 'Muon' in sample:    flag='muon'
        if 'EGamma' in sample:  flag='egamma'
        if sample == 'QCDMu' :  flag='qcd'
        if sample == 'QCDEle' : flag='qcdele'
        if sample == 'DYto2L':  flag='dy'
        if sample == 'TT':      flag='ttbar'
        if sample == 'VLLD':    flag='doublet'

        sample_time_start = time.time()
        for subsample, val in subs.items():

            samplename = f'{sample}_{subsample}'
            print(f'\nProcessing {samplename} ... ', style='cyan')
            
            ### Exception for nanoAOD version in Run3Summer22:
            if campaign == "Run3Summer22":
                if nanoAODv==12:
                    if 'RareTop' in sample and 'TTWZ' in subsample:
                        print(f'Skipping {samplename} because of nanoAOD version-{nanoAODv}', style='red')
                        continue
                if nanoAODv==11:
                    if 'RareTop' in sample and 'TTWZ' not in subsample:
                        print(f'Skipping {samplename} because of nanoAOD version-{nanoAODv}', style='red')
                        continue

            #------------------------------------------
            # Find directories and files in input_path.
            # If not found, skip.
            #------------------------------------------

            inpath = os.path.join(nanoAODpath, sample, subsample)
            if not os.path.exists(inpath):
                print(f"Warning: Path does not exist: {inpath}", style='yellow')
                pattern = os.path.join(nanoAODpath, f"{sample}_{subsample}_*")
                print(f"Searching for pattern: {pattern}")
                matches = glob.glob(pattern)
                if not matches:
                    raise FileNotFoundError(f"No match found for {pattern}")
                    list_failed.append(samplename)
                    continue
                print(f'Found: {matches[0]}')
                inpath = matches[0]

            filelist = os.listdir(inpath)
            filelist = [f for f in os.listdir(inpath) if f.endswith('.root')]
            print(f'Found {len(filelist)} files.')
            if len(filelist) == 0:
                print(f"Error: Files don't exist in: {inpath}", style='red')
                list_failed.append(samplename)
                continue

            nfiles += len(filelist)
            infiles = inpath+'/*.root'

            prefix = ''
            if mode == 'TreeMaker': prefix = 'tree'
            if mode == 'Skimmer':   prefix = 'skim'
            if mode == 'HistMaker': prefix = 'hst'
            outfile = f'{prefix}_{samplename}.root'

            outdir = os.path.join(dumpdir, jobname)
            if not dryrun: os.makedirs(outdir, exist_ok=True)
            outfile = os.path.join(outdir, outfile)
            
            if debug:
                print("Parameters are ready.")
                print("mode = "+mode)
                print("infile = "+infiles)
                print("outfile = "+outfile)
                print("campaign = "+campaign)
                print("flag = "+flag)

            rootmacro = f'{codedir}/compile_and_run.C'
            arguments = f'"{mode}", "{infiles}", "{outfile}", "{campaign}", "{samplename}", "{flag}"'
            command = f"root -q -b -l '{rootmacro}({arguments})'"
            if debug: print(command, style='dim italic')

            if not dryrun: os.system(command)

            njobs += 1
            sample_time_end=time.time()
            sample_time = sample_time_end-sample_time_start
            print(f'\nJob succeeded for {samplename}')
            print(f'Time taken for this sample = {int(sample_time)} seconds.')
            list_processed.append(samplename)
            sample_time_start = sample_time_end
            
            if test: break ### subsample
        if test: break ### sample
    if test: break ###list


end_time = time.time()
time_taken = end_time-start_time
time_fmt = str(timedelta(seconds=int(time_taken)))

print('\n'+'-'*50, style='yellow bold')
print('Summary', style='yellow bold')
print(f'Samples processed: {list_processed}')
print(f'Output directory: [yellow bold]{os.path.join(dumpdir, jobname)}[/yellow bold]')
print(f'Total time taken : [bold yellow]{time_fmt}[/bold yellow]')
print(f'Number of jobs : {njobs}')
print(f'Number of files : {nfiles}')
if len(list_failed) > 0:
    print('Warning: The following samples were skipped.', style='red')
    print(list_failed)
print('-'*50+'\n\n', style='yellow bold')
