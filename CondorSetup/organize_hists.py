import os, sys, argparse
import json
import time
import datetime
today = datetime.datetime.now()
date = today.strftime('%Y-%m-%d')

start_time = time.time()

parser=argparse.ArgumentParser()
parser.add_argument('--jobname', type=str,  required=True,  help='Condor job name: Such as hist_2LSS_Dec13')
parser.add_argument('--date',    type=str,  required=False, help='In the format : YYYY-MM-DD')
parser.add_argument('--test' ,   type=bool, required=False, help='Run for only one sample')
parser.add_argument('--dryrun',  type=bool, required=False, help='Check If everything is correct before running')
parser.add_argument('--skim',    type=bool, required=False, help='hadd skimmed files')
parser.add_argument('--tree',    type=bool, required=False, help='hadd trees')
args=parser.parse_args()

jobname = args.jobname
if args.date: date = args.date
test    = args.test
dryrun  = args.dryrun
DUMP    = '/home/work/phazarik1/work/CondorDump/hadded/'+jobname
skimmed = args.skim
trees   = args.tree

if not dryrun : os.system(f'mkdir -p {DUMP}')

with open('../InputJsons/lumidata_2018.json', 'r') as file: json_data = json.load(file)
#with open('../InputJsons/lumidata_legacy_2016.json', 'r') as file: json_data = json.load(file)
indict = json_data

for sample, subdict in indict.items():
    for subsample, lumi in subdict.items():
        print(sample, subsample, lumi)
    
        indir = f'/home/work/phazarik1/work/CondorDump/output/{jobname}/{sample}_{subsample}_{date}'
        pathexists = os.path.exists(indir)
        if not pathexists: continue
        list_of_files = os.listdir(indir)
        nonempty = len(list_of_files) > 0
        if not nonempty:
            print(f'Skipping empty directory: {indir}')
            continue
        #os.system(f'ls {ifname}')

        prefix = 'hst'
        if trees:   prefix = 'tree'
        if skimmed: prefix = 'skim'

        ofname = f'{DUMP}/{prefix}_{sample}_{subsample}.root'
        hadd_command = f'hadd -fk {ofname} {indir}/*root'
        if dryrun : print(f'Processline = {hadd_command}')
        else : os.system(hadd_command)
        
        if test : break #subsample loop
    if test : break #sample loop

end_time = time.time()
time_taken = end_time-start_time

print('\nDone!')
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

text_to_print = f'Time taken = {str(int(time_taken))} seconds.'
display_text(text_to_print, 33)
