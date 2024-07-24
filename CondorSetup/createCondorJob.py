###############################################################################################
# This is a python script that creates a condor job for only one sample (eg. DYJetsToLL_M50)
# It uses runana.C, which is kept in the same directory as the condor scripts.
# It has to be run in a For loop to create condor jobs for all the samples
# This is done using 'submitCondorJobsInBulk.py'
###############################################################################################

# Note : full paths are denoted by all caps.

import sys
import os,argparse
from datetime import datetime

timestamp = datetime.now().strftime("%Y-%m-%d")

#input arguments:
jobname    = sys.argv[1] 
samplepath = sys.argv[2] # input path
dumpdir    = sys.argv[3]
samplename = sys.argv[4] # input name (eg: DYJetsToLL_M50)
data       = sys.argv[5] # "0" = mC, "1" = data
campaign   = sys.argv[6] 
lep        = sys.argv[7] #"el" = electron dataset, "mu" = muon dataset
flag       = sys.argv[8] # customizable string flag
codedir    = sys.argv[9]
mode       = sys.argv[10]
debug_str  = sys.argv[11]
lumi       = sys.argv[12]
file_type  = sys.argv[13]
#For the old setup:
#era = 'Z'
#mc = 'wz'

#_____________________________________________________________
#
#                     DO NOT TOUCH BELOW
#_____________________________________________________________

scriptname = "/home/work/phazarik1/work/Analysis-Run3/CondorSetup/runana.C"

if debug_str == "True" :   debug = True
elif debug_str == "False": debug = False
else : print("Error: Give correct argument to the debug string!")

libdir = None
outputtag = None
if   mode == "hist": outputtag = 'hst'
elif mode == "skim": outputtag = 'skim'
elif mode == "tree": outputtag = 'tree'
else : print("Error while selecting mode! Options: 'hist', 'skim', 'tree'.")

if debug == True :
    print('going in the createCondorJob.py script ...')
    print('debug = '+str(debug))
    print('mode = '+mode)
    print('flag = '+flag)

#Setting output directories:
INDIR = samplepath
CMSDIR="/home/work/phazarik1/work/VLLanalysis/CMSSW_10_3_1/src"
DUMP = dumpdir
OUTDIR=DUMP+"/output/"+jobname+"/"+samplename+"_"+timestamp
LOGDIR=DUMP+"/log/"+jobname+"/"+samplename+"_"+timestamp
if debug == False:
    os.system("mkdir -p "+OUTDIR)
    os.system("mkdir -p "+LOGDIR)
    os.system("chmod -R 777 "+OUTDIR)
    os.system("chmod -R 777 "+LOGDIR)


########################################################################################
# Preapring the shell script containing the root command and the arguments
# This shell script is called for each file in the input directory via the .condor file.
########################################################################################

SCRIPTDIR = os.getcwd()            #Where the condor scripts are located
os.system('cd ..')                 #go back to the main directory
MAINDIR = os.getcwd()              #path of the main directory
USER = os.environ.get('USER')

runScript=SCRIPTDIR+"/runJobsCondor_simulation.sh"
#os.system(f"[ -f +"runScript+" ] && rm -rf "+runScript) #if the file already exists, remove it
os.system("touch "+runScript)
os.system("chmod a+x "+runScript)

#arguments = r'\(\"$1\",\"$2\",\"$3\",\"$4\",\"$5\",\"$6\"\)' #raw string
#arguments = r'\(\"$1\",\"$2\",\"$3\",\"$4\",\"$5\",\"$6\",\"$7\"\)' #with codedir
#arguments = r'\(\"$1\",\"$2\",\"$3\",\"$4\",\"$5\",\"$6\",\"$7\",\"$8\"\)' #with codedir, lumi
arguments = r'\(\"$1\",\"$2\",\"$3\",\"$4\",\"$5\",\"$6\",\"$7\",\"$8\",\"$9\"\)' #with codedir, lumi, samplename

processline = 'root -q -b -l '+scriptname+arguments

list_of_instructions=[
    "#!/bin/bash",
    "export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch",
    "export COIN_FULL_INDIRECT_RENDERING=1",
    "echo $VO_CMS_SW_DIR",
    "source $VO_CMS_SW_DIR/cmsset_default.sh",
    "cd "+CMSDIR,
    "eval `scramv1 runtime -sh`",
    processline
]

with open(runScript, 'w') as file:
    for string in list_of_instructions:
        file.write(string + '\n')

        
##################################################################
#Preparing the condor file that goes as an argument to 'condor_q'
##################################################################

condorfile=SCRIPTDIR+"/submitJobsCondor_"+USER+"_"+samplename+"_"+timestamp+".condor"
#os.system(f"[ -f +"condorfile+" ] && rm -rf "+condorfile) #if the file already exists, remove it
os.system("touch "+condorfile)
os.system("chmod a+x "+condorfile)

#The following information goes in the beginning of the condor file.
condorfile_info=[
    "universe = vanilla",
    'Requirements = (Arch == "X86_64")',
    "Executable = "+runScript,
    "should_transfer_files = YES",
    "Notification=never",
    "",
    ""
]

#We need to add the runana.C script and the  input parameters for each files in this condor file.
#For that, we need to loop over all files in the input directory.
input_files = os.listdir(INDIR)

if len(input_files) == 0: print("Empty directory: "+INDIR)

filecount=0
for filename in input_files:
    if filename.endswith(".root"):
        filecount+=1

        #I am picking the output file name from the directory in which they are kept.
        out_samplename = INDIR.split("/")[-2]
        out_subsamplename = INDIR.split("/")[-1]

        if "VLL" in samplename:
            if not 'tau' in samplename:
                out_samplename = INDIR.split("/")[-3]
                out_subsamplename = INDIR.split("/")[-1].split("_")[-2]+INDIR.split("/")[-1].split("_")[-1]
            if 'tau' in samplename and file_type == 'normal':
                out_samplename = samplename.split('_')[0]
                out_subsamplename = samplename.split('_')[1]+samplename.split('_')[2]

        #print('test: sample    = '+out_samplename)
        #print('test: subsample = '+out_subsamplename)
        
        #Naming Scheme:
        #outputtag_sample_subsample_filecount.root.
        #Example: hst_SingleMuon_SingleMuonA_1.root
        
        #Exceptions: (removing extra "_" from the output names)
        out_samplename = out_samplename.replace("_", "")
        out_subsamplename = out_subsamplename.replace("_", "")
        
        ofile = outputtag+"_"+campaign+"_"+out_samplename+"_"+out_subsamplename+"_"+str(filecount)+".root"
        #print(ofile)
        
        if debug == True :
            print('file no : '+str(filecount))
            print('Input = '+INDIR+'/'+filename)
            print('Output = '+OUTDIR+"//$(Cluster)_"+ofile)
            print('Code = '+codedir+"\n")

        #The following list (for each file) needs to be added to the condor file.
        info_for_each_file = [
            "output = "+LOGDIR+"//$(Cluster)_data_"+filename+".out",
            "error = "+LOGDIR+"//$(Cluster)_data_"+filename+".err",
            "log = "+LOGDIR+"//$(Cluster)_data_"+filename+".log",
            "arguments = "+INDIR+"/"+filename+" "+OUTDIR+"//$(Cluster)_"+ofile+"_data.root "+str(data)+" "+campaign+" "+str(lep)+" "+str(flag)+" "+codedir+" "+str(lumi)+" "+samplename,
            "queue",
            ""
        ]
        condorfile_info += info_for_each_file
        if debug == True : break

#Let's write this list to the condor file:
with open(condorfile, 'w') as file:
    for string in condorfile_info:
        file.write(string + '\n')


        
#####################
# Submitting the job
#####################

#Once the condor file is written properly, the following line will submit the job.
condorfile = "submitJobsCondor_"+USER+"_"+samplename+"_"+timestamp+".condor"
main_processline = "condor_submit "+condorfile

print("\n______"+samplename+"______")
print("Input directory : "+INDIR)
print("Output directory: "+OUTDIR)
print("Code directory: "+codedir)
print("runana Script :"+scriptname)

if debug == True : print('main processline = '+main_processline)
elif debug == False : os.system("eval "+main_processline)
else : print("Error: Give correct argument to the debug string!")
    
######################
#cleaning up the mess
######################

#All the condor files are dumped in the following folder after they are used.
dumpjobs = "previous_jobs/"+timestamp+"/"+jobname+"/"+samplename
print('condor dump : '+dumpjobs+"/"+condorfile)
os.system("mkdir -p "+dumpjobs)
os.system("mv "+condorfile+" "+dumpjobs+"/.")
if debug == True : print('... done with the condor script.\n')
