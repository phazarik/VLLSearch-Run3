##############################################################################################
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
jobname = sys.argv[1] 
samplepath  = sys.argv[2] # input path
samplename = sys.argv[3] # input name (eg: DYJetsToLL_M50)
data = sys.argv[4] # "0" = mC, "1" = data
year = sys.argv[5]
lep = sys.argv[6] #"el" = electron dataset, "mu" = muon dataset
#For the old setup:
era = 'Z'
mc = 'wz'
debug_string = sys.argv[7]


#Collecting some variables from the os itself:
SCRIPTDIR = os.getcwd()            #Where the condor scripts are located
os.system('cd ..')                 #go back to the main directory
MAINDIR = os.getcwd()              #path of the main directory
USER = os.environ.get('USER')

#CHOOSE THE runana SCRIPT TO RUN:
skimmode = True

if debug_string == "True" : debug = True
elif debug_string == "False": debug = False
else : print("Error: Give correct argument to the debug string!")

print('debug mode = '+str(debug))
print('skim mode = '+str(skimmode))

#scriptname = SCRIPTDIR+"/runana.C" #this file is fixed.
#For the old setup:
if skimmode==True: scriptname = '/home/work/phazarik1/work/VLLanalysis/VLLAnalysisUL/archive/prachu/skimmer/runana.C'
else : scriptname = '/home/work/phazarik1/work/VLLanalysis/VLLAnalysisUL/archive/prachu/skimmed_2LSS/runana.C'
scriptname = '/home/work/phazarik1/work/VLLanalysis/VLLAnalysisUL/archive/prachu/basic/runana.C'

#-----------------------------------------------------------------------------
#For testing without any extra script:
#samplepath = "/home/work/alaha1/public/RunII_ULSamples/2018/DYJetsToLL/M50"
#samplename = "DYJetsToLL_M50"
#data = 0
#year = 2018
#lep = "mu"
#-----------------------------------------------------------------------------

#Input and output directories:
#if skimmode==True : DUMP="/home/work/phazarik1/work/SkimmedSamples2018"
if skimmode==True : DUMP="/home/work/phazarik1/work/VLLanalysis/SkimmedSamples"
else : DUMP="/home/work/phazarik1/work/VLLanalysis/CondorOutput"
DUMP="/home/work/phazarik1/work/VLLanalysis/CondorOutput"
INDIR = samplepath
OUTDIR=DUMP+"/output/"+jobname+"/"+samplename+"_"+timestamp
LOGDIR=DUMP+"/log/"+jobname+"/"+samplename+"_"+timestamp

#Also, using the appropriate CMSSW version:
CMSDIR="/home/work/phazarik1/work/VLLanalysis/CMSSW_10_3_1/src"

#Making the output directories where the logs and outputs are kept:
#and giving them permissions:

if debug == False:
    os.system("mkdir -p "+OUTDIR)
    os.system("mkdir -p "+LOGDIR)
    os.system("chmod -R 777 "+OUTDIR)
    os.system("chmod -R 777 "+LOGDIR)

#________________________________________________________________________________________
#
# DO NOT TOUCH BELOW (unless you want to change the arguments that goes into runana.C)
#________________________________________________________________________________________


########################################################################################
# Preapring the shell script containing the root command and the arguments
# This shell script is called for each file in the input directory via the .condor file.
########################################################################################

runScript=SCRIPTDIR+"/runJobsCondor_simulation.sh"
#os.system(f"[ -f +"runScript+" ] && rm -rf "+runScript) #if the file already exists, remove it
os.system("touch "+runScript)
os.system("chmod a+x "+runScript)

#arguments = r'\(\"$1\",\"$2\",\"$3\",\"$4\",\"$5\"\)' #raw string
arguments = r'\(\"$1\",\"$2\",\"$3\",\"$4\",\"$5\",\"$6\",\"$7\"\)' #for the old setup
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
#print(INDIR)
filecount=0

#print('test')
for filename in input_files:
    if filename.endswith(".root"):
        filecount+=1

        #print(filename)
        
        if skimmode == True and samplename == 'TTW_TTWToLNu' :
            #correcting for the poorly named TTW_TTWToLNu files while skimming
            ofile = "VLL_TTW_TTWToLNu_file"
        elif samplename.startswith('SingleMuon') :
            #Correcting for data:
            ofile = "VLL_"+filename.split("_")[1]+"_"+filename.split("_")[2][-1]

        else :
            if skimmode == True : ofile = filename
            else : ofile = filename.split("_")[0]+"_"+filename.split("_")[1]+"_"+filename.split("_")[2]+"_"+filename.split("_")[3]
            #base = filename.split("_")[0] #"VLL"
            #example : VLL_DYJetsToLL_M50_10
            

        if skimmode==True: ifdir = INDIR
        else : ifdir = filename

        if debug == True :
            print('file no : '+str(filecount))
            print('Input = '+INDIR+'/'+filename)
            print('Output = '+OUTDIR+"//$(Cluster)_"+ofile+"_"+str(filecount)+".root\n")

        #The following list (for each file) needs to be added to the condor file. 
        info_for_each_file = [
            "output = "+LOGDIR+"//$(Cluster)_data_"+filename+".out",
            "error = "+LOGDIR+"//$(Cluster)_data_"+filename+".err",
            "log = "+LOGDIR+"//$(Cluster)_data_"+filename+".out",
            #"arguments = "+INDIR+"/"+filename+" "+OUTDIR+"//$(Cluster)_"+ofile+"_data.root "+str(data)+" "+str(year)+" "+str(lep),
            "arguments = "+INDIR+"/"+filename+" "+OUTDIR+"//$(Cluster)_"+ofile+"_"+str(filecount)+".root "+str(data)+" "+str(year)+" "+str(lep)+" "+str(era)+" "+str(mc),#for the old setup
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
print("Script :"+scriptname)

if debug == True : print('main processline = '+main_processline)
elif debug == False : os.system("eval "+main_processline)
else : print("Error: Give correct argument to the debug string!")
    
######################
#cleaning up the mess
######################

#All the condor files are dumped in the following folder after they are used.
dumpdir = "previous_jobs/"+timestamp+"/"+samplename
print('condor dump : '+dumpdir+"/"+condorfile+"\n")
os.system("mkdir -p "+dumpdir)
os.system("mv "+condorfile+" "+dumpdir+"/.")








