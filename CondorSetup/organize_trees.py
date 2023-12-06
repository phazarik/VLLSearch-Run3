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

args=parser.parse_args()
jobname= args.jobname #Example : 'tree_2LSS_Dec05'
test   = args.test

INDIR = '/home/work/phazarik1/work/CondorDump/output/'+jobname
DUMP = '/home/work/phazarik1/work/Trees_2LSS/'+datestamp

if test == False: os.system(f'mkdir -p {DUMP}')

jsonfile = '../InputJsons/sample_database.json'
with open(jsonfile,'r') as infile:
    sampledict = json.load(infile)


listdirs = os.listdir(INDIR)
list_processed = []
count = 0

for sample, subs in sampledict.items():
    for subsample in subs:

        if any((sample in s) or (subsample in s) for s in listdirs) : list_processed.append(sample)
        
        folder = INDIR + '/' + next((s for s in listdirs if (sample in s) or (subsample in s)), None)
        files = os.listdir(folder)

        processline = f'hadd -f {DUMP}/tree_{sample}_{subsample}.root {folder}/*.root'
        print('\nExecuting : ' +processline)

        if test == True : break #for subsample
        else :
            os.system(processline)
            print(f'tree created for {sample} {subsample}\n')
            count = count +1

    if test == True : break #for sample

end_time = time.time()
time_taken = end_time-start_time
print('\n\033[93mDone!\033[0m\n')
print('------------------------------------------------------------------------------')
print('\033[93mSummary:\033[0m')
print(f'\033[93mSamples : {list_processed}\033[0m')
print(f'\033[93mNo of files : {count}\033[0m')
print(f'\033[93mOutput directory (hst files): {DUMP}\033[0m')
print(f'\033[93mtime taken : {time_taken:.2f} seconds\033[0m')
print('-------------------------------------------------------------------------------\n')
