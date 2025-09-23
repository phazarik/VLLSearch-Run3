import os, sys

limits_dir = "limits/"
archive_name = "limits.tar.gz"

if not os.path.exists(limits_dir):
    print("Error: limits don't exist.")
    sys.exit(1)
if os.path.exists(archive_name):
    print("Removing older tar.")
    os.system(f"rm -f {archive_name}")

os.system(f"tar -czvf {archive_name} {limits_dir}")
print("\nDone!")
