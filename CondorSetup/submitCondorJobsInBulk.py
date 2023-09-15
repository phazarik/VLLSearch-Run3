###############################################################################################
# This takes all the Physics processes mentioned in the list and submits condor job for them
# using createCondorJob.py. The way I am reading the input files from the specific directories
# depends on how they are kept. So, adjust your json file accordingly.
###############################################################################################
#Make sure to use python3 for this.

import sys
import os,argparse
import json
import time #for calculating how much time taken
start_time = time.time()

#jsonfile = '../InputJsons/sample_2018_skimmed_2LSS.json'
jsonfile = '/home/work/phazarik1/work/VLLanalysis/VLLAnalysisUL/inputs/sample_all.json' #for skimmer
with open(jsonfile,'r') as infile:
    samplelist = json.load(infile)

#samplelist contains all samples that are there.
#However, the script only submits codor jobs for the following.

#condorsamples = ["DYJetsToLL","HTbinnedWJets","SingleTop","TTBar", "TTW","WW","WZ","ZZ","QCD_MuEnriched","SingleMuon","EGamma"] #OR
condorsamples = ["SingleMuon", "EGamma"]
#condorsamples = ["DYJetsToLL"]
#condorsamples = ["HTbinnedWJets","SingleTop","WW","WZ","ZZ","TTW"]

#_____________________________________________________________
#
#                     DO NOT TOUCH BELOW
#_____________________________________________________________

#Picking the arguments:
parser=argparse.ArgumentParser()
parser.add_argument('--jobname',type=str, required=True, help='AnalysisName: Such as VLL2018_Mar1_v0')
parser.add_argument('--dryrun' ,type=bool,required=False,help='Check If everything is correct before submitting')
parser.add_argument('--test'   ,type=bool,required=False,help='Check one job')
parser.add_argument('--debug'  ,type=bool,required=False,help='For printing information', default=False)
args=parser.parse_args()

dryrun = args.dryrun
jobname= args.jobname
test   = args.test
debug  = args.debug #for debugging the condor-script

#initializing a list for summary:
list_processed=[]

print(f'\n\033[93mSubmitting condor jobs ...\033[0m')

#Creating condor jobs for each process mentioned in the list:
for item in condorsamples:
    for samplegroup, groupvalue in samplelist.items():
        if item == samplegroup :
            list_processed.append(samplegroup)
            print('\n----------------------------------------------')
            print(f'\033[91m Submitting jobs for {samplegroup} \033[0m')
            print('----------------------------------------------')

            #The following loop runs for individual samples.
            #For example, samplegroup = 'DYJetsToLL', sample = 'M50' 
            #It runs 'createCondorJobs.py' for each sub-sample
            for sample, val in groupvalue.items():
                indir = val['samplepath']
                data = str(val['data'])
                year = str(val['year'])
                lep = 'mu'
                if samplegroup == 'EGamma' : lep = 'el'

                #correction for single muon dataset:
                if samplegroup == 'SingleMuon' or samplegroup == 'EGamma':
                    sample = sample.split("_")[1]+sample.split("_")[2]
                
                arguments = f'{jobname} {indir} {sample} {data} {year} {lep} {debug}'
                processline = 'python3 createCondorJob.py '+arguments

                if dryrun==True: print(processline)
                else: os.system(processline)
                if test==True : break #only for one subsample+

        if test==True : break #only for one sample
                

end_time = time.time()
time_taken = end_time-start_time

print('\n\033[93mDone!\033[0m\n')
print('------------------------------------------------------------------------------')
print('\033[93mSummary:\033[0m')
print(f'\033[93mSamples : {list_processed}\033[0m')
print(f'\033[93mjobname : {jobname}\033[0m')
print(f'\033[93mOutput directory (hst files): /home/work/phazarik1/work/VLLanalysis/CondorOutput/output/{jobname}\033[0m')
print(f'\033[93mOutput directory (for skim) : /home/work/phazarik1/work/VLLanalysis/SkimmedSamples/output/{jobname}\033[0m')
print(f'\033[93mtime taken : {time_taken:.2f} seconds\033[0m')
print('-------------------------------------------------------------------------------\n')
