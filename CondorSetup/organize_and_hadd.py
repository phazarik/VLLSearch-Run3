import os, sys, argparse
import json
import time
import datetime
today = datetime.datetime.now()
date = today.strftime('%Y-%m-%d')

start_time = time.time()

parser=argparse.ArgumentParser()
parser.add_argument('--jobname', type=str,  required=True,  help='Condor job name: Such as hist_2LSS_Dec13')
parser.add_argument('--test' ,   type=bool, required=False, help='Run for only one sample')
parser.add_argument('--dryrun',  type=bool, required=False, help='Check If everything is correct before running')
parser.add_argument('--skim',    type=bool, required=False, help='hadd skimmed files')
parser.add_argument('--tree',    type=bool, required=False, help='hadd trees')
args=parser.parse_args()

jobname = args.jobname
test    = args.test
dryrun  = args.dryrun
skimmed = args.skim
trees   = args.tree

basedir = os.path.join('/home/work/phazarik1/work/CondorDump/output/', jobname)
dumpdir = os.path.join('/home/work/phazarik1/work/CondorDump/hadded/', jobname)
if not dryrun : os.system(f'mkdir -p {dumpdir}')

samples = os.listdir(basedir)

for folder in samples:
    
    prefix = 'hst'
    if skimmed: prefix = 'skim'
    if trees:   prefix = 'tree'

    name = folder.split('_2024')[0]

    ### Corrections for names = {oldname:newname}
    corrections = {
        'M10To50':'M10to50',
        'Inclusice':'Inclusive',
        'WZTo2L2Q':'WZTo2Q2L',
        'TTWToLnu':'TTWToLNu',
        'SingleElectronBpreVFP_ver2':'SingleElectron_B',
        'SingleElectronCpreVFP':'SingleElectron_C',
        'SingleElectronDpreVFP':'SingleElectron_D',
        'SingleElectronEpreVFP':'SingleElectron_E',
        'SingleElectronFpreVFP':'SingleElectron_F',
        'SingleElectronFpostVFP':'SingleElectron_F',
        'SingleElectronGpostVFP':'SingleElectron_G',
        'SingleElectronHpostVFP':'SingleElectron_H'
    }

    for old, new in corrections.items():
        if old in name:
            print(f'\033[31m\nWarning! Name change: {name} -> ', end='')
            name = name.replace(old, new)
            name = name.replace('SingleElectron', 'EGamma') #for all
            print(f'{name}\033[0m')
    
    outfilename = f'{prefix}_{name}.root'
    outfilepath = os.path.join(dumpdir, outfilename)
    
    processline = f'hadd -fk {outfilepath} {os.path.join(basedir, folder)}/*root'

    print(f'\033[33m\nhadding: {outfilename}\033[0m')
    if dryrun: print(processline)
    else: os.system(processline)
    if test: break

end_time = time.time()
time_taken = end_time-start_time
print(f'\033[93m\nDone!\nTime taken = {int(time_taken)} seconds.\033[0m')
        
    
