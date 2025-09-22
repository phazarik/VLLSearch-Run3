import os, subprocess, argparse, time

parser = argparse.ArgumentParser()
parser.add_argument("--test", action="store_true", help="test mode")
args = parser.parse_args()
test = args.test

remote_user = "phazarik"
remote_host = "lxplus.cern.ch"
local_dir = "shapes/"
archive_name = "shapes.tar.gz"
remote_path = "/afs/cern.ch/user/p/phazarik/work/Combine/CMSSW_14_1_0_pre4/src/HiggsAnalysis/CombinedLimit/data/tutorials/longexercise"

print(f"\n{'-'*40}")
print(f"Starting packaging and upload process")
print(f"Local directory: {local_dir}")
print(f"Remote target : {remote_user}@{remote_host}:{remote_path}")
print(f"{'-'*40}\n")

start = time.time()

if os.path.exists(archive_name):
    print(f"\nRemoving existing archive: {archive_name}")
    if not test:
        os.remove(archive_name)

print(f"\nCreating tar.gz archive: {archive_name}")
if not test:
    subprocess.run(["tar", "-czvf", archive_name, local_dir], check=True)

remote_target = f"{remote_user}@{remote_host}:{remote_path}/{archive_name}"
print(f"\nUploading archive to remote server: {remote_target}")
if not test:
    subprocess.run(["scp", archive_name, remote_target], check=True)

end = time.time()
print(f"\nProcess completed successfully!")
print(f"Time taken: {end - start:.2f} seconds")
if test: print("Test mode: No actual file was created or uploaded.")
