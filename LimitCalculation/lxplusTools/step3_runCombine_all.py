import os
import time
from datetime import timedelta
import argparse, subprocess
from rich import print

campaigns = [
    "2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
    "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix",
]
campaigns.extend(["Run2", "Run3", "FullDataset"])
channels  = ["mm", "me", "em", "ee"]
channels.extend(["combined"])

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--test", action="store_true", help="Run only once.")
    parser.add_argument("--dryrun", action="store_true", help="Dryrun mode.")
    args = parser.parse_args()

    time_start = time.time()
    count = 0
    for camp in campaigns:
        for ch in channels:
            if (camp=="Run2" or camp=="Run3") and ch != "combined": continue
            count += 1

            cmd = ["python3", "step2_runCombine_once.py", "--campaign", camp, "--channel", ch]
            if args.test: cmd.append("--test")
            if args.dryrun: cmd.append("--dryrun")
            
            print(f"Job #{count:>2} ==> [yellow]{' '.join(cmd)}[/yellow]")
            if not args.dryrun: subprocess.run(cmd)

            if args.test: break
        if args.test: break

    time_end = time.time()
    elapsed = timedelta(seconds=int(time_end - time_start))
    print(f"\nDone!\nTime taken: {elapsed}")

if __name__ == "__main__": main()
