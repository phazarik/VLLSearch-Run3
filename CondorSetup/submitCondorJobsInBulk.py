#----------------------------------------------------------------------------------------------------
# This takes all the Physics processes mentioned in the json file and submits condor job for them
# using createCondorJob.py.
# exampleL python3 scriptname.py --jobname blah --test yes --dryrun yes
#----------------------------------------------------------------------------------------------------

import sys
import os,argparse
import json
import time
start_time = time.time()

### Picking the arguments and flags:
parser=argparse.ArgumentParser()
parser.add_argument('--jobname',type=str, required=True, help='AnalysisName: Such as VLL2018_Mar1_v0')
parser.add_argument('--dryrun' ,type=bool,required=False,help='Check If everything is correct before submitting')
parser.add_argument('--test'   ,type=bool,required=False,help='Check one job')
parser.add_argument('--debug'  ,type=bool,required=False,help='For printing information', default=False)
args=parser.parse_args()

dryrun = args.dryrun #Don't submit any job
jobname= args.jobname
test   = args.test  #submit from only one folder.
debug  = args.debug #for debugging the condor-script

#--------------------
# Global settings
#--------------------

campaign = "2016postVFP_UL"       ### Options: 2016preVFP_UL, 2016postVFP_UL, 2017_UL, 2018_UL
mode = "hist"              ### Options: 'hist', 'skim', 'tree'
file_type = 'normal'      ### Options:  'normal', 'skimmed'

condorsamples = ["DYJetsToLL", "ZGamma", "HTbinnedWJets", "QCD_MuEnriched", "QCD_EMEnriched", "SingleTop", "TTBar", "TTW", "TTZ", "WW", "WZ", "ZZ", "Rare", "WWZ", "WZZ", "ZZZ", "Higgs", "WpWp", "WWW", "WGamma", "VLLS_ele", "VLLS_mu", "VLLD_ele", "VLLS_tau", "VLLD_mu"]
condorsamples.extend(["SingleMuon", "SingleElectron"])
#condorsamples = ["SingleMuon", "SingleElectron"]
#_____________________________________________________________
#
#                     DO NOT TOUCH BELOW
#_____________________________________________________________

#-----------------------
# Setting up parameters
#-----------------------

dumpdir = "/home/work/phazarik1/work/CondorDump"
if dryrun == False: os.system('mkdir -p '+dumpdir)

### Year
year = None
if '2016'   in campaign: year=2016
elif '2017' in campaign: year=2017
elif '2018' in campaign: year=2018
elif '22'   in campaign: year=2022
elif '23'   in campaign: year=2023
elif '24'   in campaign: year=2024

### Lumi
lumi = None
if '2016preVFP'  in campaign: lumi= 36000
if '2016postVFP' in campaign: lumi= 36000
elif '2017'      in campaign: lumi= 44000
elif '2018'      in campaign: lumi= 59800

### jsonfile
jsonfile = None
samplelist = None
if '2016preVFP'  in campaign: jsonfile='../InputJsons/lumidata_2016preVFP.json'
if '2016postVFP' in campaign: jsonfile='../InputJsons/lumidata_2016postVFP.json'
elif '2017'      in campaign: jsonfile='../InputJsons/lumidata_2017.json'
elif '2018'      in campaign: jsonfile='../InputJsons/lumidata_2018.json'
with open(jsonfile,'r') as infile: samplelist = json.load(infile)

### Base input directory
nanoAOD_path = None
if file_type == 'skimmed' :
    if "2018" in campaign:          nanoAOD_path = "/home/work/phazarik1/work/CondorDump/output/skim_2LSS_2018UL_Oct15"
    elif "2017" in campaign:        nanoAOD_path = "/home/work/phazarik1/work/CondorDump/output/skim_2LSS_2017UL_Nov01"
    elif "2016preVFP"  in campaign: nanoAOD_path = "/home/work/phazarik1/work/CondorDump/output/skim_2LSS_2016preVFPUL_Nov06"
    elif "2016postVFP" in campaign: nanoAOD_path = "/home/work/phazarik1/work/CondorDump/output/skim_2LSS_2016postVFPUL_Nov07"
else:
    if "2018" in campaign:   nanoAOD_path = "/home/work/alaha1/public/RunII_ULSamples/2018"
    elif "2017" in campaign: nanoAOD_path = "/home/work/alaha1/public/RunII_ULSamples/2017"
    elif "2016" in campaign: nanoAOD_path = "/home/work/alaha1/public/RunII_ULSamples/2016"

### Codedir
codedir = None
if   mode == "hist" : codedir = "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/HistMaker"
elif mode == "skim" : codedir = "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/Skimmer"
elif mode == "tree" : codedir = "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/TreeMaker"

#-----------------------
# Looping over samples
#-----------------------

njobs=0
list_processed=[]
list_failed=[]

print(f'\n\033[33mSubmitting condor jobs ...\033[0m')

for sample, subs in samplelist.items():
    for item in condorsamples:
        if sample != item: continue ### Skipping missing items from the list

        ### sample loop begins.
        list_processed.append(sample)
        print('\n----------------------------------------------')
        print(f'\033[93m Submitting jobs for {sample} \033[0m')
        print('----------------------------------------------')

        for subsample, val in subs.items():
            
            ### sub-sample loop begins.
            ### Initializing default paremeters.
            data = 0                
            lep = 'mu'
            flag = 'flag'
            lumi = val
            mcwt = -1
            input_path = nanoAOD_path
            print(file_type, sample, campaign)

            #---------------------------------
            # Picking the correct input path:
            #---------------------------------

            if file_type == "normal" :
                input_path = os.path.join(input_path, sample)
                 
                ### Path exceptions for data:
                if year in [2016, 2017, 2018] and sample in ['SingleMuon', 'EGamma', 'SingleElectron']:
                    input_path = os.path.join(nanoAOD_path, f'UL{year}Data', sample)
                #if year in [2016] and sample in ['SingleMuon']:
                #    input_path = os.path.join(nanoAOD_path, sample)

                ### Path exceptions for signal:
                if sample in ['VLLS_ele', 'VLLD_ele', 'VLLS_mu', 'VLLD_mu']:
                    input_path = f"/home/work/ykumar1/Work/VLLAnalysis_e-muLike/Samples/Signal/2018/{sample.split('_')[0]}/{sample.split('_')[1]}"
                if sample in ['VLLS_tau']:
                    input_path = "/home/work/alaha1/public/RunII_ULSamples/2018/VLL"
                if sample in ['VLLS_legacy']:
                    input_path = "/home/work/phazarik1/public/RunII_LegacySamples/2016/VLLS"

                ### Path exceptions for preVFP/postVFP
                if not 'VLL' in sample:
                    if 'preVFP'  in campaign: input_path = os.path.join(input_path, 'preVFP')
                    if 'postVFP' in campaign: input_path = os.path.join(input_path, 'postVFP')

            ### skimmed files Don't need any additional correction.

            #----------------------------------------------
            # Correction for naming inconsistencies:
            # (needs to be done before checking for files)
            #----------------------------------------------

            #1) EGamma -> Egamma
            if sample == "EGamma":
                if not any(subsample in s for s in list_dirs) :
                    print(f"Error with {sample}_{subsample} : files not found!")
                    print('replacing G with g in EGamma ..')
                    subsample = subsample.replace('G', 'g')
            #2) M10to50 -> M10To50 in case of DrellYan 2016 samples  
            if sample == 'DYJetsToLL' and '2016' in campaign:
                print(f'replacing subsample from {subsample}', end=' ')
                subsample = subsample.replace("to", "To")
                print(f'to {subsample}')

            #------------------------------------------
            # Find directories and files in input_path.
            # If not found, skip.
            #------------------------------------------
            
            indir = None
            list_dirs = None
            
            try: list_dirs = os.listdir(input_path)
            except:
                print(f'\033[91mWarning : {sample} does not exist in this path: {input_path}\033[0m')
                list_failed.append(f'{sample}')
                continue
            
            if file_type == 'skimmed':
                try: indir = input_path + "/"+ next((f for f in list_dirs if sample in f and subsample in f), None)
                except :
                    print(f'\033[91mWarning : {sample}_{subsample} not found.\033[0m')
                    list_failed.append(f'{sample}_{subsample}')
                    continue
            else:
                try : indir = input_path + "/"+ next((f for f in list_dirs if subsample in f), None)
                except :
                    print(f'\033[91mWarning : {sample}_{subsample} not found.\033[0m')
                    list_failed.append(f'{sample}_{subsample}')
                    continue
                
            ### Skip empty directories:
            exists = os.path.exists(indir)
            empty  = len(os.listdir(indir)) == 0
            if empty or not exists:
                print(f'\033[91mWarning : Skipping empty folder : {indir}\033[0m')
                continue

            #---------------------------------------
            # Setting parameters for each subsample:
            #---------------------------------------
            ### data
            if sample in ['SingleMuon', 'EGamma', 'SingleElectron']: data = 1

            ### flag
            if 'VLLD' in sample: flag = 'doublet'
            if 'QCD'  in sample: flag = 'qcd'
            if 'DY'   in sample: flag = 'dy'
            if 'EGamma' in sample: flag = 'egamma'
            if 'SingleElectron' in sample: flag = 'egamma'

            #-------------
            # Processline:
            #-------------
            
            arguments = f'{jobname} {indir} {dumpdir} {sample}_{subsample} {data} {campaign} {lep} {flag} {codedir} {mode} {debug} {lumi} {file_type}'
            processline = 'python3 createCondorJob.py '+arguments

            nfiles = len(os.listdir(indir))
            njobs += nfiles
            print(f'\033[33mProcessing {nfiles} files...\033[0m')
            
            if dryrun: print(processline)
            else: os.system(processline)

            if test: break ### sub-sample loop ends.

    if test: break #sample loop ends.


end_time = time.time()
time_taken = end_time-start_time

print('\n\033[93mDone!\033[0m\n')
print('------------------------------------------------------------------------------')
print(f'\033[33m', end='')
print(f'Summary:')
print(f'Samples : {list_processed}')
print(f'jobname : {jobname}')
print(f'Output directory: {dumpdir}/output/{jobname}')
print(f'time taken : {time_taken:.2f} seconds.')
print(f'number of jobs : {njobs}', end='')
print(f'\033[0m', end='\n')
if len(list_failed) > 0:
    print(f'\033[31m', end='')
    print('Warning: The following samples were skipped.')
    print(list_failed)
    print(f'\033[0m', end='\n')
print('-------------------------------------------------------------------------------\n')
