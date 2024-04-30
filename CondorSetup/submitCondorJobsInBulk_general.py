###################################################################################################
# This takes all the Physics processes mentioned in the list and submits condor job for them
# using createCondorJob.py. The way I am reading the input files from the specific directories
# depends on how they are kept. So, adjust your json file accordingly.
# Make sure to make chnages to runana.C also, if you want to use codes from a different directory.
###################################################################################################
#Make sure to use python3 for this.

import sys
import os,argparse
import json
import time #for calculating how much time taken
start_time = time.time()

#Picking the arguments and flags:
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

#################
#Global settings:
#################
year = 2018
lumi = 59800 #pb^{-1}
#process_signal = True
dumpdir = "/home/work/phazarik1/work/CondorDump"
mode = "hist"             #Options: 'hist', 'skim', 'tree'. Edit the runana file accordingly.
file_type = 'skimmed'     #Options: 'normal', 'skimmed'

#################################
# Select which samples to run on:
#################################
condorsamples = ["DYJetsToLL", "HTbinnedWJets", "QCD_MuEnriched", "QCD_EMEnriched", "SingleTop", "TTBar", "TTW", "TTZ", "WW", "WZ", "ZZ", "VLLS_ele", "VLLS_mu", "VLLD_ele", "VLLD_mu", "SingleMuon", "EGamma"]
#condorsamples = ["VLLS_ele", "VLLS_mu", "VLLD_ele", "SingleMuon"]
#condorsamples = ["DYJetsToLL"]

#_____________________________________________________________
#
#                     DO NOT TOUCH BELOW
#_____________________________________________________________

#jsonfile = '../InputJsons/sample_database.json'
jsonfile = '../InputJsons/lumidata_2018.json'

#if file_type == 'skimmed' : nanoAOD_path = "/home/work/phazarik1/work/CondorDump/output/skim_2muSS_Mar05_Baseline"
if file_type == 'skimmed' : nanoAOD_path = "/home/work/phazarik1/work/CondorDump/output/skim_2LSS_2018UL_Apr27"
else : nanoAOD_path = "/home/work/alaha1/public/RunII_ULSamples/2018"

codedir = None
if   mode == "hist" : codedir = "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/HistMaker"
elif mode == "skim" : codedir = "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/Skimmer"
elif mode == "tree" : codedir = "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/TreeMaker"
else : print("Error while selecting mode! Options: 'hist', 'skim', 'tree'.")
#Note: You can manually set the code directory here.

if dryrun == False: os.system('mkdir -p '+dumpdir)

#initializing a list for summary:
list_processed=[]

with open(jsonfile,'r') as infile:
    samplelist = json.load(infile)

list_processed=[]
print(f'\n\033[93mSubmitting condor jobs ...\033[0m')


#Creating condor jobs for each process mentioned in the list:
for sample, subs in samplelist.items():
    for item in condorsamples:
        #print(f'Checking ... {item} and {sample}')
        if sample == item: #The entry from the list must be identical to the key in the json file.
            #print(f'Match found : {sample}')

            list_processed.append(sample)
            print('\n----------------------------------------------')
            print(f'\033[91m Submitting jobs for {sample} \033[0m')
            print('----------------------------------------------')

            #The following loop runs for individual sub-samples.
            #For example, sample = 'DYJetsToLL', sub-sample = 'M50' 
            #It runs 'createCondorJobs.py' for each sub-sample
            for subsample, val in subs.items():

                #Dafult values of the parameters:
                data = 0                
                lep = 'mu'
                flag = 'flag'
                lumi = val
                
                input_path = nanoAOD_path
                
                #Corrections for the nanoAOD sample storage scheme in our cluster:
                if file_type != "skimmed" :
                    input_path = input_path + '/' + sample
                    if   sample == 'SingleMuon': input_path = nanoAOD_path + "/UL2018Data/SingleMuon"
                    elif sample == 'EGamma':     input_path = nanoAOD_path + "/UL2018Data/EGamma"
                    elif 'VLLS' in sample or 'VLLD' in sample:
                        input_path = f"/home/work/ykumar1/Work/VLLAnalysis_e-muLike/Samples/Signal/2018/{sample.split('_')[0]}/{sample.split('_')[1]}"

                list_dirs = os.listdir(input_path)
                list_dirs = [s for s in list_dirs if '.tar' not in s] #filtering the list for .tar objects

                #Idiot proofing, in case the directory does not contain the sample/subsample
                if not any(subsample in s for s in list_dirs) :
                    print(f"Error with {sample}_{subsample} : files not found!")
                    print('replacing G with g in EGamma ..')
                    subsample = subsample.replace('G', 'g')

                #Setting the input directory
                #The input directory is more fragmented in case of the regular nanoAOD files. 
                indir = None
                if file_type == 'skimmed' :
                    try: indir = input_path + "/"+ next((f for f in list_dirs if sample in f and subsample in f), None)
                    except :
                        print(f'\033[91mWarning : {sample}_{subsample} not found.\033[0m')
                        continue
                else :
                    try : indir = input_path + "/"+ next((f for f in list_dirs if subsample in f), None)
                    except :
                        print(f'\033[91mWarning : {sample}_{subsample} not found.\033[0m')
                        continue

                #Skip if indir is empty:
                exists = os.path.exists(indir)
                empty  = len(os.listdir(indir)) == 0
                if empty or not exists:
                    print(f'\033[91mWarning : SKipping empty folder : {indir}')
                    continue
                
                #Corrections on the parameters based on which sample it is:
                if sample.startswith('SingleMuon') or sample.startswith('EGamma') : data = 1
                if sample.startswith('VLLD') :    flag = 'doublet'
                if sample.startswith('QCD')  :    flag = 'qcd'
                if sample.startswith('DY')   :    flag = 'dy'
                if sample.startswith('EGamma') :  lep  = 'el'
                if sample.startswith('EGamma') :  flag = 'egamma'

                #Setting MC weights for jet-tagging:
                mcwt = -1
                

                #arguments = f'{jobname} {indir} {dumpdir} {sample}_{subsample} {data} {year} {lep} {flag} {codedir} {mode} {debug}'
                arguments = f'{jobname} {indir} {dumpdir} {sample}_{subsample} {data} {year} {lep} {flag} {codedir} {mode} {debug} {lumi}'
                processline = 'python3 createCondorJob.py '+arguments

                if dryrun == True : print(processline)
                else: os.system(processline)
            
                if test==True : break #only for one sub-sample

        if test==True : break #only for one sample


end_time = time.time()
time_taken = end_time-start_time

print('\n\033[93mDone!\033[0m\n')
print('------------------------------------------------------------------------------')
print('\033[93mSummary:\033[0m')
print(f'\033[93mSamples : {list_processed}\033[0m')
print(f'\033[93mjobname : {jobname}\033[0m')
print(f'\033[93mOutput directory: {dumpdir}/output/{jobname}\033[0m')
print(f'\033[93mtime taken : {time_taken:.2f} seconds\033[0m')
print('-------------------------------------------------------------------------------\n')
