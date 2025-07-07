import os, argparse
import subprocess
from rich.markdown import Markdown
from rich.panel import Panel
from rich.console import Console
console = Console(highlight=False)
print = console.print

parser = argparse.ArgumentParser()
parser.add_argument("--test", action="store_true", help="test mode")
args = parser.parse_args()
test = args.test

remote_user = "phazarik"
remote_host = "lxplus.cern.ch"
remote_path = "/afs/cern.ch/user/p/phazarik/work/Combine/CMSSW_14_1_0_pre4/src/HiggsAnalysis/CombinedLimit/data/tutorials/statistical_routines_tutorial/datacards"
local_dir = "datacards/2025-07-04"

dirs_to_copy = [
    os.path.join(local_dir, d)
    for d in os.listdir(local_dir)
    if os.path.isdir(os.path.join(local_dir, d))
]

if not dirs_to_copy:  print("No subdirectories found to copy.")
else:
    print("\nPreparing to copy the following directories:")
    for d in dirs_to_copy: print(f" - {d}")

    print('\nRunning scp command ...')
    cmd = ["scp", "-r"] + dirs_to_copy + [f"{remote_user}@{remote_host}:{remote_path}"]
    print(' '.join(cmd)+'\n', style="yellow italic dim")
    if not test: subprocess.run(cmd)

    print("\n[yellow]Transfer complete.[/yellow]\n")

    instructions = """
1. Login to lxplus.
2. Go to the work-area:
```
/afs/cern.ch/user/p/phazarik/work/Combine/CMSSW_14_1_0_pre4/src/HiggsAnalysis/CombinedLimit/data/tutorials/statistical_routines_tutorial
```
3. Do `cmsenv`.
4. Run the limit calculator tool there: (this will take some time)
```
python3 find_limits_allJobs.py
```
5. Come back to local area and bring back limit files.
```
python3 lxplus_pull_limits.py
```
"""

    print(Panel(Markdown(instructions), title="Next Steps", border_style="grey50"), "\n")
