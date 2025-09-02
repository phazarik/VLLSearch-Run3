import os, sys, argparse
from PIL import Image
from math import ceil, sqrt

parser = argparse.ArgumentParser()
parser.add_argument('--indir',  type=str,            required=True,  help='Plot directory.')
parser.add_argument('--test',   action='store_true', required=False, help='Run once.')
parser.add_argument('--dryrun', action='store_true', required=False, help='Dryrun.')
args = parser.parse_args()
indir = args.indir

set0 = ["njet", "nbjet", "HT", "LT", "ST", "STfrac", "dilep_ptratio", "metpt", "metphi"]
set1 = ["nnscore_Run2_vlld_qcd", "nnscore_Run2_vlld_wjets", "nnscore_Run2_vlld_ttbar",
        "nnscore_Run3_vlld_qcd", "nnscore_Run3_vlld_wjets", "nnscore_Run3_vlld_ttbar"]
set2 = ["lep0_pt", "lep0_eta", "lep0_phi", "lep0_mt", "lep0_iso", "lep0_sip3d"]
set3 = ["lep1_pt", "lep1_eta", "lep1_phi", "lep1_mt", "lep1_iso", "lep1_sip3d"]
set4 = ["dilep_pt", "dilep_eta", "dilep_phi", "dilep_mt", "dilep_dphi", "dilep_dR"]
set5 = ["dphi_metlep0", "dphi_metlep1", "dphi_metdilep", "dphi_metlep_max"]
set6 = ["2LSS_wt_leptonSF", "2LSS_wt_trig", "2LSS_wt_pileup", "2LSS_wt_bjet", "2LSS_wt_evt"]
set7 = ["dilep_mass", "dilep_pt", "dilep_mt", "dilep_dR", 
        "dilep_ptratio","HT", "LT", "metpt",
        "lep0_pt", "lep0_phi", "njet", "nbjet"
        ]
set8 = ["lep0_pt", "dilep_dR", "dilep_mt", "njet"]
dnn1 = [
    'njet',
    'dilep_dR',
    'dilep_ptratio',
    'LTplusMET',
    'ST',
    'STfrac',
    'metpt',
    'dphi_metdilep',
    'dphi_metlep_min'
]
dnn2 = [
    'njet',
    'dilep_mt',
    'dilep_ptratio',
    'dilep_dR',
    'LTplusMET',
    'STfrac'
]
dnn3 = [
    'nbjet',
    'dilep_ptratio',
    'LTplusMET',
    'ST',
    'STfrac',
    'dphi_metlep0',
]
dnn4 = [
    'njet',
    'lep0_eta',
    'lep1_eta',
    'dilep_mt',
    'LTplusMET',
    'metpt',
]

plotdict = {
    "event_level": set0,
    "nnscores": set1,
    "lep0": set2,
    "lep1": set3,
    "dilep": set4,
    "dphis": set5,
    "weights": set6,
    "mixed": set7,
    "slides": set8,
    "dnn-qcd": dnn1,
    "dnn-wjets": dnn2,
    "dnn-top": dnn3,
    "dnn-dy": dnn4
}

for plotname, plotset in plotdict.items():

    infiles = []
    for key in plotset:
        matching_files = [f for f in os.listdir(indir) if f == f"{key}.png"]
        infiles.extend(matching_files)

    #infiles = [f for f in os.listdir(indir) if f.endswith('.png')]
    #infiles = [f for f in infiles if any(k in f for k in plotset)]
    #infiles.sort()
    if not infiles: continue

    print("==> Combining the following plots: ")
    index = 0
    for fig in infiles:
        index+=1
        print(f"{index}. {fig}")
    
    images = [Image.open(os.path.join(indir, f)) for f in infiles]
    w, h = images[0].size
    n = len(images)
    cols = int(ceil(sqrt(n)))
    rows = int(ceil(n / cols))

    combined = Image.new('RGB', (cols * w, rows * h), (255, 255, 255))

    for i, img in enumerate(images):
        x = (i % cols) * w
        y = (i // cols) * h
        combined.paste(img, (x, y))

    outname = os.path.join(indir, f"combined_{plotname}.png")
    if not args.dryrun: combined.save(outname)
    print(f"Saved: \033[93m{outname}\033[0m ({rows} rows x {cols} cols)\n")

    if args.test: break

print('Done!')
