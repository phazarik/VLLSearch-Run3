#!/usr/bin/env python3
import os, sys
os.system("cmsenv")
from subprocess import run, CalledProcessError
from rich.console import Console
console = Console(highlight=False)
print = console.print
import time

def main():
    datacards_dir = "datacards"
    limits_dir = "limits"
    test = False

    if not check_cmssw(): sys.exit(1)
    print("This will take a while. Hold on ...\n")
    
    if not os.path.isdir(datacards_dir):
        print(f"Error: '{datacards_dir}' directory not found.", style="bold red")
        sys.exit(1)

    os.makedirs(limits_dir, exist_ok=True)

    subdirs = [d for d in os.listdir(datacards_dir) if os.path.isdir(os.path.join(datacards_dir, d))]

    if not subdirs:
        print(f"Error: No subdirectories found in '{datacards_dir}'.", style="bold red")
        return
    
    for index, folder_name in enumerate(subdirs):
        
        print(f'({index+1}/{len(subdirs)}) Processing {folder_name}', style="yellow bold")
        cmd = ["python3", "find_limits_oneJob.py", "--jobname", folder_name]
        if test: cmd.append("--test")
        print(f" ".join(cmd), style="yellow")
        
        try: run(cmd, check=True)
        except FileNotFoundError:
            print("Error: 'find_limits_oneJob.py' not found in current directory.", style="bold red")
            sys.exit(1)
        except CalledProcessError as e:
            print(f"Error: Command failed for '{folder_name}' with exit code {e.returncode}", style="bold red")

        if test: break 

def check_cmssw():
    cmssw_base = os.environ.get("CMSSW_BASE")
    if not cmssw_base:
        print("\n[WARNING] No CMSSW environment detected! Please set up CMSSW before running\n", style="bold red")
        return False
    print(f"\nCMSSW environment detected: {cmssw_base}")
    return True
        
if __name__ == "__main__":
    time1 = time.time()
    main()
    time2 = time.time()
    dtime = time2-time1
    print(f"\nDone in {int(dtime)} seconds!\n", style="yellow bold")
    
