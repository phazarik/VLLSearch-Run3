import os, sys
import argparse
import subprocess
import time
import json
from datetime import timedelta
from tqdm import tqdm
from rich.console import Console
console = Console(highlight=False)
print = console.print

parser = argparse.ArgumentParser()
parser.add_argument('--test', action='store_true')
parser.add_argument('--dryrun', action='store_true')
args = parser.parse_args()
test   = args.test
dryrun = args.dryrun

if test:   print('[WARNING]: test mode',   style="red")
if dryrun: print('[WARNING]: dryrun mode', style="red")
#default = True

#----------------------------------- config ----------------------------------------
campaigns = ["2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
             "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix"]
campaigns.extend(["Run2", "Run3", "FullDataset"])
channels  = ["mm", "me", "em", "ee", "combined"]
basename  = "2025-09-02_val_cleaned"
tag       = "val"
text      = "validation region"
tousedata = False
tosave    = True
jobdict = {}
#------------------------------------------------------------------------------------

jobdict = {}
for camp in campaigns:
    if "Run3" in camp: continue
    for ch in channels:
        #if not (ch == "combined" or ch == "ee"): continue
        if not ch == "combined": continue
        key = f"{basename}/hist_{tag}_{camp}_{ch}"
        jobdict[key] = {"campaign": camp, "channel": ch}

print("Processing the following jobs:")
for job, val in jobdict.items(): print(f' - {job}')

variables = [
    ("nnscore_Run2_vlld_qcd",   "NNScore: QCD vs VLLD (Run-2)"),
    ("nnscore_Run2_vlld_ttbar", "NNScore: t#bar{t} vs VLLD (Run-2)"),
    ("nnscore_Run2_vlld_wjets", "NNScore: W+#gamma vs VLLD (Run-2)"),
    ("nnscore_Run2_vlld_dy",    "NNScore: DY+jets vs VLLD (Run-2)"),
    ("nnscore_Run3_vlld_qcd",   "NNScore: QCD vs VLLD (Run-3)"),
    ("nnscore_Run3_vlld_ttbar", "NNScore: t#bar{t} vs VLLD (Run-3)"),
    ("nnscore_Run3_vlld_wjets", "NNScore: W+#gamma vs VLLD (Run-3)"),
    ("nnscore_Run3_vlld_dy",    "NNScore: DY+jets vs VLLD (Run-3)"),
    
    ("nlep",  "N_{L}"),
    ("njet",  "N_{J} (AK4)"),
    ("nfatjet", "N_{J} (AK8)"),
    ("nbjet", "N_{bJ}"),

    ("lep0_pt",    "p_{T}(L_{0}) (GeV)"),
    ("lep0_eta",   "#eta(L_{0})"),
    ("lep0_phi",   "#phi(L_{0})"),
    ("lep0_iso",   "reliso03 (L_{0})"),
    ("lep0_sip3d", "sip3d (L_{0})"),
    ("lep0_mt",    "m_{ T}(L_{0}) (GeV)"),
    
    ("lep1_pt",    "p_{T}(L_{1}) (GeV)"),
    ("lep1_eta",   "#eta(L_{1})"),
    ("lep1_phi",   "#phi(L_{1})"),
    ("lep1_iso",   "reliso03 (L_{1})"),
    ("lep1_sip3d", "sip3d (L_{1})"),
    ("lep1_mt",    "m_{ T}(L_{1}) (GeV)"),

    ("dilep_pt",      "p_{T}^{LL} (GeV)"),
    ("dilep_eta",     "#eta_{LL}"),
    ("dilep_phi",     "#phi_{LL}"),
    ("dilep_mass",    "M_{LL} (GeV)"),
    ("dilep_mt",      "m_{ T}^{LL} (GeV)"),
    ("dilep_deta",    "|#Delta#eta(L_{0}, L_{1})|"),
    ("dilep_dphi",    "#Delta#phi(L_{0}, L_{1})"),
    ("dilep_dR",      "#Delta R(L_{0}, L_{1})"),
    ("dilep_ptratio", "p_{T1} / p_{T0}"),

    ("LTplusMET",    "L_{T}+p_{T}^{miss} (GeV)"),
    ("HT",           "H_{ T} (GeV)"),
    ("HTfat",        "H_{ T}(AK8) (GeV)"),
    ("STvis",        "H_{ T}+L_{T} (GeV)"),
    ("STvisfat",     "H_{ T}(AK8)+L_{T} (GeV)"),
    ("ST",           "H_{ T}+L_{T}+p_{T}^{miss} (GeV)"),
    ("STfat",        "H_{ T}(AK8)+L_{T}+p_{T}^{miss} (GeV)"),
    ("STfrac",       "L_{T}/(H_{ T}+L_{T}+p_{T}^{miss})"),
    ("HTMETllpt",    "H_{ T}+p_{T}^{miss}+p_{T}^{LL} (GeV)"),
    ("HTfatMETllpt", "H_{ T}(AK8)+p_{T}^{miss}+p_{T}^{LL} (GeV)"),
    ("metpt",        "p_{T}^{miss} (GeV)"),
    ("metphi",       "#phi^{miss}"),
    
    ("dphi_metlep0",    "#Delta#phi(L_{0}, p_{T}^{miss})"),
    ("dphi_metlep1",    "#Delta#phi(L_{1}, p_{T}^{miss})"),
    ("dphi_metdilep",   "#Delta#phi(LL, p_{T}^{miss})"),
    ("dphi_metlep_max", "max(#Delta#phi(L, p_{T}^{miss}))"),
    ("dphi_metlep_min", "min(#Delta#phi(L, p_{T}^{miss}))"),

    ("2LSS_wt_leptonSF", "Lepton IdIso SF"),
    ("2LSS_wt_trig", "Trigger efficiency SF"),
    ("2LSS_wt_pileup", "PileUp weight"),
    ("2LSS_wt_bjet", "bJet weight"),
    ("2LSS_wt_evt", "Event weight"),
]

#______________________________________________________________________________________________________________
#______________________________________________________________________________________________________________

start_time = time.time()

jobcount = 0
plotcount = 0

for jobname, info in jobdict.items():
    
    jobcount += 1
    campaign = info['campaign']
    channel  = info['channel']
    
    print(f'\n({jobcount}/{len(list(jobdict.items()))}) Making plot for {jobname} ({channel}, {tag}, {text})')

    count = 0
    iterator_var = variables if not test else [variables[0]]
    wrapper = (
        tqdm(iterator_var,total=len(variables),unit="plot",desc="Plotting",colour="green",ncols=100,leave=True,
             bar_format="{l_bar}{bar}| [{elapsed} < {remaining}, {n_fmt}/{total_fmt}]")
        if not test else iterator_var
    )

    for var, varname in wrapper:
        count += 1
        plotcount += 1
        if test: print(f"\nPlot no: {count}", style='underline')
        arguments = (
            f'"{var}", '
            f'"{varname}", '
            f'"{jobname}", '
            f'"{campaign}", '
            f'"{channel}", '
            f'"{tag}", '
            f'"{text}", '
            f'{str(tousedata).lower()}, '
            f'{str(tosave).lower()}'
        )
        ## For plotmaker:
        command = f"root -q -b -l 'makeStackedPlot.C({arguments})'"

        ## For writing yields"
        #if var != "dilep_pt": continue
        #if var != "LT": continue
        #command = f"root -q -b -l 'writeYields.C({arguments})'"

        if test: print(command, style="italic dim")
        if not test: command += " > /dev/null 2>&1" ## supress output
        if not dryrun: subprocess.run(command, shell=True)

    print(f'[yellow]{count}[/yellow] plot(s) made for {jobname}')
    if test: break

end_time = time.time()
time_taken = end_time - start_time

print("\nDone!", style='yellow bold')
print(f"Time taken = {str(timedelta(seconds=int(time_taken)))}\n", style='yellow bold')
print(f"Total number of plots = {plotcount}\n", style='yellow bold')
