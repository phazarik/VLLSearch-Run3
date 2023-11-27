import os, sys, argparse

parser = argparse.ArgumentParser()
parser.add_argument('--all', action='store_true', help='removes all the output root files in test_outputs')
args = parser.parse_args()

junk = [
    "*~",
    "*.pcm",
    "*.so",
    "*#",
    "*d"
]

if args.all : junk.append("test_outputs/*")

print("Running the following commands ...")
for item in junk:
    processline = "rm -rf "+item
    print(processline)
    os.system(processline)
print("Done!")
    
