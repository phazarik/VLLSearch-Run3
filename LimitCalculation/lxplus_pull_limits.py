import os, argparse
import subprocess
from datetime import datetime
from rich.console import Console
console = Console(highlight=False)
print = console.print

parser = argparse.ArgumentParser()
parser.add_argument("--test", action="store_true", help="test mode")
args = parser.parse_args()
test = args.test

remote_user = "phazarik"
remote_host = "lxplus.cern.ch"
remote_path = "/afs/cern.ch/user/p/phazarik/work/Combine/CMSSW_14_1_0_pre4/src/HiggsAnalysis/CombinedLimit/data/tutorials/statistical_routines_tutorial/limits"

today = datetime.today().strftime("%Y-%m-%d")
local_dir = f"fromCMScombine/{today}"

if not os.path.exists(local_dir): os.makedirs(local_dir)

print(f"\nPreparing to copy *.txt files from remote to: {local_dir}")
scp_command = f"scp {remote_user}@{remote_host}:{remote_path}/*.txt {local_dir}/"
print(scp_command, style="yellow italic dim")

if not test:
    os.makedirs(local_dir, exist_ok=True)
    subprocess.run(scp_command, shell=True)

print("\n[yellow]Transfer complete.[/yellow]\n")
