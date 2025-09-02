import json
import os

campaigns = ["2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL"]
#campaigns = ["Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix"]
indir = "."
outfile = "lumidata_Run2.json"

combined = {}

print("\nProcessing ..")
for camp in campaigns:
    path = os.path.join(indir, f"lumidata_{camp}.json")
    print(f" - {path}")
    with open(path) as f:
        data = json.load(f)
    for sample, subsamples in data.items():
        if sample not in combined:
            combined[sample] = {}
        for subsample, val in subsamples.items():
            combined[sample][subsample] = combined[sample].get(subsample, 0) + val

with open(outfile, "w") as f:
    json.dump(combined, f, indent=4)
    
print(f"\n\033[33;1mOutput: {outfile}\033[0m")
