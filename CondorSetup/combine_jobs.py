
###################################################################################################
# This script is used to combine 2016 preVFP and postVFP into a single format
# Just give the jobnames as a list and set the output jobname
###################################################################################################
#Make sure to use python3 for this.

#import uproot (only works with native python, not with CMSSW)
import sys
import os,argparse
import json
import time #for calculating how much time taken
start_time = time.time()

parser=argparse.ArgumentParser()
parser.add_argument('--dryrun' ,type=bool,required=False,help='Check If everything is correct before running')
parser.add_argument('--test'   ,type=bool,required=False,help='Check one example')
args=parser.parse_args()

dryrun = args.dryrun #Don't run anything, just print output
test   = args.test   #Run for only one example

######################################################################
#Settings:

joblist = ["hist_Apr29_2muSS_qcdregionv2", "hist_Apr30_2muSS_qcdVR"]
outname = "hist_test_combined"

######################################################################
# DO NOT CHNAGE ANYTHING BELOW

#-------------------------------------------------------------------------
#Functions:

'''
def if_files_have_same_structure(files):

    # Open the first file and get its structure
    first_file = uproot.open(files[0])
    first_keys = set(first_file.keys())

    # Compare the keys in other files
    for filename in files[1:]:
        current_file = uproot.open(filename)
        current_keys = set(current_file.keys())

        # Compare the current file's keys with the first file's keys
        if current_keys != first_keys:
            print(f"Files do not have the same structure: {filename}")
            return False

    return True
'''

def display_text(text, color):
    # ANSI COLOR CODES:
    # Black   = 30
    # Red     = 31
    # Green   = 32
    # Yellow  = 33
    # Blue    = 34
    # Magenta = 35
    # Cyan    = 36
    # White   = 37
    set_color = "\033[" + str(color) + "m"
    print(set_color + text + "\033[0m")  # 0=default

#-------------------------------------------------------------------------

#Managing output:
indir = '/home/work/phazarik1/work/CondorDump/hadded'
DUMP = indir+'/'+outname
if not dryrun : os.system(f'mkdir -p {DUMP}')

jsonfile = '../InputJsons/lumidata_2018.json'
with open(jsonfile,'r') as infile:  samplelist = json.load(infile)

#initializing a list for summary:
list_processed=[]
count = 0

for sample, subdict in samplelist.items():
    for subsample, lumi in subdict.items():
        count = count + 1
        files_to_hadd = []
        print(f'Adding {sample} {subsample}')
        for jobname in joblist:
            filename = f'{indir}/{jobname}/hst_{sample}_{subsample}.root'
            files_to_hadd.append(filename)

        proceed = True #Assuming that all input files have the same structure.
        #if count == 1 : proceed = if_files_have_same_structure_in(files_to_hadd) #Check this only for the first set of files
        #else :          proceed = True
        if len(files_to_hadd) == 1:
            display_text('Error: File missing for {sample} {subsample}, skipping ...', 31)
            continue

        if proceed:
            #hadd them
            outfilename = f'hst_{sample}_{subsample}.root'
            outfile = f'{DUMP}/{outfilename}'
            processline = f'hadd -fk {outfile} '+' '.join(files_to_hadd)
            if dryrun: print(processline+'\n')
            else : os.system(processline)
        else:
            display_text('Error: Files have different structure. Aborting ...', 31)
            break
        
        if test : break #for one subsample
    if test : break #for one sample

#Summary:
end_time = time.time()
time_taken = end_time-start_time
text_to_print = f'Time taken = {str(int(time_taken))} seconds.'
display_text(text_to_print, 33)

