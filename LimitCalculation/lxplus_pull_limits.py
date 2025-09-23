import os, subprocess, time
import shutil

remote_user = "phazarik"
remote_host = "lxplus.cern.ch"
remote_archive = "/afs/cern.ch/user/p/phazarik/work/Combine/CMSSW_14_1_0_pre4/src/HiggsAnalysis/CombinedLimit/data/tutorials/longexercise/limits.tar.gz"
archive_name = "limits.tar.gz"
local_dir = "limits/"

hline = "-"*40
print(f"\n{newline}")
print(f"Starting download process")
print(f"Remote archive : {remote_user}@{remote_host}:{remote_archive}")
print(f"Local target dir : {local_dir}")
print(newline)

start = time.time()

if os.path.exists(archive_name):
    print(f"\nRemoving existing local archive: {archive_name}")
    os.remove(archive_name)

if os.path.exists(local_dir):
    print(f"\nRemoving existing local directory: {local_dir}")
    shutil.rmtree(local_dir)

print(f"\nDownloading archive from remote server...")
subprocess.run(["scp", f"{remote_user}@{remote_host}:{remote_archive}", archive_name], check=True)

print(f"\nUnpacking archive locally...")
subprocess.run(["tar", "-xzvf", archive_name], check=True)

print(f"\nProcess completed successfully!")
end = time.time()
print(f"Time taken: {end - start:.2f} seconds")
