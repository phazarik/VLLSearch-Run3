#--------------------------------------------------------------------------------------------------------
# This takes all the Physics processes mentioned in the lumi json file and runs compile_and_run for them.
# example usage: python3 processAllSamples.py --jobname test --dryrun --debug
#--------------------------------------------------------------------------------------------------------

import os, sys, argparse
import json
import time
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

campaign = "Run3Summer22"
### Options for Run2: 2016preVFP_UL, 2016postVFP_UL, 2017_UL, 2018_UL
### Options for Run3: Run3Summer22, Run3Summer22EE, Run3Summer23, Run3Summer23BPix

mode = "TreeMaker"
nanoAODv = 12

samples_to_run = ["DYto2L", "Higgs", "QCDEM", "QCDMu", "RareTop", "ST", "TT", "TTV", "TW", "VV", "VVSS", "VVV", "WGtoLNuG", "WtoLNu", "ZGamma"]
if nanoAODv==11: samples_to_run=["Muon", "EGamma", "RareTop", "VLLS_ele", "VLLS_mu", "VLLD_ele", "VLLD_mu"]

### Absolute paths:
dumpdir = "ROOT_FILES/trees/"
nanoAODpath = "/mnt/d/work/skimmed_2LSS_Run3Summer22"
codedir = "/mnt/d/work/GitHub/VLLSearch-Run3/AnalysisScripts"

#---------------------------------------------
# DON'T TOUCH BELOW
#---------------------------------------------

def warning(text, color=31, bold=0): print(f'\033[{bold};{color}m{text}\033[0m')

jsonfilepath = 'LumiJsons'
jsonfile = os.path.join(jsonfilepath, f'lumidata_{campaign}.json')
if not os.path.exists(jsonfilepath): warning(f'Error: File not found: {jsonfile}')
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
        #if sample not in ['QCDEM', 'QCDMu', 'VVSS', 'WGtoLNuG', 'WtoLNu', 'ZGamma']: continue
        #if 'VLL' not in sample: continue
        #if 'RareTop' not in sample: continue

        print('\n----------------------------------------------')
        warning(f'Submitting jobs for {sample}', 93)
        print('----------------------------------------------')

        flag = 'flag'
        if sample == 'Muon':    flag='muon'
        if sample == 'EGamma':  flag='egamma'
        if sample == 'QCDMu' :  flag='qcd'
        if sample == 'QCDEle' : flag='qcdele'
        if sample == 'DYto2L':  flag='dy'
        if sample == 'TT':      flag='ttbar'
        if sample == 'VLLD':    flag='doublet'

        sample_time_start = time.time()
        for subsample, val in subs.items():

            samplename = f'{sample}_{subsample}'
            warning(f'\nProcessing {samplename} ... ', 94)
            
            ### Exception for nanoAOD version:
            if nanoAODv==12:
                if 'RareTop' in sample and 'TTWZ' in subsample:
                    warning(f'Skipping {samplename} because of nanoAOD version-{nanoAODv}', 91)
                    continue
            if nanoAODv==11:
                if 'RareTop' in sample and 'TTWZ' not in subsample:
                    warning(f'Skipping {samplename} because of nanoAOD version-{nanoAODv}', 91)
                    continue

            #------------------------------------------
            # Find directories and files in input_path.
            # If not found, skip.
            #------------------------------------------

            inpath = os.path.join(nanoAODpath, sample, subsample)
            if not os.path.exists(inpath):
                warning(f'Error: Path does not exist: {inpath}')
                list_failed.append(samplename)
                continue

            filelist = os.listdir(inpath)
            filelist = [f for f in os.listdir(inpath) if f.endswith('.root')]
            print(f'Found {len(filelist)} files.')
            if len(filelist) == 0:
                warning(f"Error: Files don't exist in: {inpath}")
                list_failed.append(samplename)
                continue

            nfiles += len(filelist)
            infiles = inpath+'/*.root'

            prefix = ''
            if mode == 'TreeMaker': prefix = 'tree'
            if mode == 'Skimmer':   prefix = 'skim'
            if mode == 'HistMaker': prefix = 'hist'
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
            if debug: warning(command, 33)

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
warning('\n'+'-'*50, 93)
warning('Summary', 93)
print(f'Samples processed: {list_processed}')
print(f'Output directory: \033[093m{os.path.join(dumpdir, jobname)}\033[0m')
print(f'Total time taken : \033[093m{time_taken:.2f} seconds\033[0m.')
print(f'Number of jobs : {njobs}')
print(f'Number of files : {nfiles}')
if len(list_failed) > 0:
    warning('Warning: The following samples were skipped.')
    print(list_failed)
warning('-'*50+'\n\n', 93)
