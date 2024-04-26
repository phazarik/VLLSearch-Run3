#Organizing the tree files. hadding the same samples.

import sys
import os,argparse
import json
import time
from datetime import datetime

start_time = time.time()
datestamp = datetime.today().strftime('%Y-%m-%d')

parser=argparse.ArgumentParser()
parser.add_argument('--jobname',type=str, required=True, help='Example : tree_2LSS_Dec05')
parser.add_argument('--test'   ,type=bool,required=False, default=False, help='Check one job')
parser.add_argument('--dryrun' ,type=bool,required=False, default=False, help='Print statement only')

args=parser.parse_args()
jobname= args.jobname #Example : 'tree_2LSS_Dec05'
test   = args.test
dryrun = args.dryrun

INDIR = '/home/work/phazarik1/work/CondorDump/output/'+jobname
DUMP = '/home/work/phazarik1/work/CondorDump/trees/'+jobname

if not dryrun:
    print('Creating/cleaning-up the dump diretory:')
    os.system(f'mkdir -p {DUMP}')
    os.system(f'rm -rf {DUMP}/*')

jsonfile = '../InputJsons/lumidata_2018.json'
with open(jsonfile,'r') as infile:
    sampledict = json.load(infile)


listdirs = os.listdir(INDIR)
list_processed = []
count = 0

for sample, subs in sampledict.items():
    for subsample in subs:

        if any((sample in s) or (subsample in s) for s in listdirs) : list_processed.append(sample+"_"+subsample)

        subfolder = next((s for s in listdirs if (sample in s) and (subsample in s)), None)
        if subfolder == None :
            print(f'\033[31\mFile not found for {sample} {subsample}\033[0m')
            continue
        
        folder = INDIR + '/' + subfolder 
        files = os.listdir(folder)

        if len(files) == 0 :
            print(f'\033[31mWarning: Empty folder for {sample} {subsample}\033[0m')
            continue
        
        processline = f'hadd -fk {DUMP}/tree_{sample}_{subsample}.root {folder}/*.root'
        print('Executing : ' +processline)

        if not dryrun: os.system(processline)
        print(f'\033[33mTree created for {sample} {subsample}\033[0m\n')
        count = count +1

        if test : break #only one subsample
    if test : break #only one sample

end_time = time.time()
time_taken = end_time-start_time
print('\n\033[93mDone!\033[0m\n')
print('------------------------------------------------------------------------------')
print('\033[93mSummary:\033[0m')
print(f'\033[93mSamples: {list_processed}\033[0m')
print(f'\033[93mNo of files generated: {count}\033[0m')
print(f'\033[93mOutput directory: {DUMP}\033[0m')
print(f'\033[93mtime taken: {time_taken:.2f} seconds\033[0m')
print('-------------------------------------------------------------------------------\n')
