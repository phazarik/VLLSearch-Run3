import os, sys, argparse
import json

parser=argparse.ArgumentParser()
parser.add_argument('--jobname',type=str, required=True, help='Condor job name: Such as hist_2LSS_Dec13')
parser.add_argument('--date',type=str, required=True, help='In the format : YYYY-MM-DD')
parser.add_argument('--test' ,type=bool,required=False,help='Run for only one sample')
parser.add_argument('--dryrun' ,type=bool,required=False,help='Check If everything is correct before running')
args=parser.parse_args()

jobname = args.jobname
date    = args.date #'2023-10-02'
test    = args.test
dryrun  = args.dryrun
DUMP    = 'input_files'

if not dryrun : os.system(f'mkdir -p {DUMP}')

with open('../InputJsons/lumidata_2018.json', 'r') as file: json_data = json.load(file)
indict = json_data

for sample, subdict in indict.items():
    for subsample, lumi in subdict.items():
        #print(sample, subsample, lumi)
        
        indir = f'hists/{jobname}/{sample}_{subsample}_{date}'
        nonempty = os.path.exists(indir)
        if not nonempty: continue
        #os.system(f'ls {ifname}')

        ofname = f'{DUMP}/hst_{sample}_{subsample}.root'
        hadd_command = f'hadd -fk {ofname} {indir}/*root'
        if dryrun : print(f'Processline = {hadd_command}')
        else : os.system(hadd_command)
        

        if test : break #subsample loop
    if test : break #sample loop
    
print('Done!')
