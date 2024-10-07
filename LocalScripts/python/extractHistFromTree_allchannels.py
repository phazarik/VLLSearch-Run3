import ROOT
import os, sys
import argparse
import time
start_time_global = time.time()

parser=argparse.ArgumentParser()
parser.add_argument('--jobname',type=str, required=True,  help='Condor job name')
parser.add_argument('--test',   type=bool,required=False, help='Run for only one sample')
parser.add_argument('--dryrun', type=bool,required=False, help='Print statments')
args=parser.parse_args()

jobname = args.jobname
test    = args.test
dryrun  = args.dryrun

channels = ['mm', 'me', 'em', 'ee']

for ch in channels:
    
    dump = jobname.replace('tree', 'hist')+'_'+ch
    command = f'python3 extractHistFromTree.py --jobname {jobname} --dump {dump} --channel {ch}'
    if dryrun: print(command)
    else: os.system(command)
    if test: break
    
end_time_global = time.time()

def time_in_hhmmss(seconds):
    hours = seconds // 3600
    minutes = (seconds % 3600) // 60
    seconds = seconds % 60
    return f"{int(hours):02} hours, {int(minutes):02} minutes, {int(seconds):02} seconds"

time_taken_global = time_in_hhmmss(end_time_global - start_time_global)

print('\n'+'-'*100)
print(f"\033[33mTotal time taken = {time_taken_global}.\033[0m\n")
