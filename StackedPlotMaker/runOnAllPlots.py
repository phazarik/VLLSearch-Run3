import os, sys
import argparse
import subprocess
import time
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
default = True

jobdict = {
    "2025-04-19/hist_2018_UL_topcr_mm":{
        "campaign":"2018_UL",
        "channel":"mm",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2018_UL_topcr_me":{
        "campaign":"2018_UL",
        "channel":"me",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2018_UL_topcr_em":{
        "campaign":"2018_UL",
        "channel":"em",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2018_UL_topcr_ee":{
        "campaign":"2018_UL",
        "channel":"ee",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2017_UL_topcr_mm":{
        "campaign":"2017_UL",
        "channel":"mm",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2017_UL_topcr_me":{
        "campaign":"2017_UL",
        "channel":"me",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2017_UL_topcr_em":{
        "campaign":"2017_UL",
        "channel":"em",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2017_UL_topcr_ee":{
        "campaign":"2017_UL",
        "channel":"ee",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2016preVFP_UL_topcr_mm":{
        "campaign":"2016preVFP_UL",
        "channel":"mm",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2016preVFP_UL_topcr_me":{
        "campaign":"2016preVFP_UL",
        "channel":"me",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2016preVFP_UL_topcr_em":{
        "campaign":"2016preVFP_UL",
        "channel":"em",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2016preVFP_UL_topcr_ee":{
        "campaign":"2016preVFP_UL",
        "channel":"ee",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2016postVFP_UL_topcr_mm":{
        "campaign":"2016postVFP_UL",
        "channel":"mm",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2016postVFP_UL_topcr_me":{
        "campaign":"2016postVFP_UL",
        "channel":"me",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2016postVFP_UL_topcr_em":{
        "campaign":"2016postVFP_UL",
        "channel":"em",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
    "2025-04-19/hist_2016postVFP_UL_topcr_ee":{
        "campaign":"2016postVFP_UL",
        "channel":"ee",
        "tag":"qcdCR",
        "text":"QCD CR"
    },
}

variables = [
    ("nnscore_qcd_vlld_2016preVFP",  "NNScore: QCD vs VLLD (2016-preVFP)"),
    ("nnscore_qcd_vlld_2016postVFP", "NNScore: QCD vs VLLD (2016-postVFP)"),
    ("nnscore_qcd_vlld_2017",        "NNScore: QCD vs VLLD (2017)"),
    ("nnscore_qcd_vlld_2018",        "NNScore: QCD vs VLLD (2018)"),
    ("nnscore_qcd_vlld_2022",        "NNScore: QCD vs VLLD (2022)"),
    
    ("nlep",  "N_{L}"),
    ("njet",  "N_{J}"),
    ("nbjet", "N_{bJ}"),
    
    ("dilep_mass", "M_{LL} (GeV)"),
    ("HT",         "H_{ T} (GeV)"),
    ("LT",         "L_{T} (GeV)"),
    ("STvis",      "H_{ T} + L_{T} (GeV)"),
    ("ST",         "S_{T} (GeV)"),
    ("STfrac",     "L_{T}/S_{T}"),
    ("HTMETllpt",  "H_{ T} + p_{T}^{miss} + p_{T}^{LL} (GeV)"),
    ("metpt",      "p_{T}^{miss} (GeV)"),
    ("metphi",     "#phi^{miss}"),
    
    ("lep0_pt",    "p_{T}(L_{0}) (GeV)"),
    ("lep0_eta",   "#eta(L_{0})"),
    ("lep0_phi",   "#phi(L_{0})"),
    ("lep0_mt",    "m_{ T}(L_{0}) (GeV)"),
    ("lep0_iso",   "reliso03 (L_{0})"),
    ("lep0_sip3d", "sip3d (L_{0})"),
    
    ("lep1_pt",    "p_{T}(L_{1}) (GeV)"),
    ("lep1_eta",   "#eta(L_{1})"),
    ("lep1_phi",   "#phi(L_{1})"),
    ("lep1_mt",    "m_{ T}(L_{1}) (GeV)"),
    ("lep1_iso",   "reliso03 (L_{1})"),
    ("lep1_sip3d", "sip3d (L_{1})"),
    
    ("dilep_pt",      "p_{T}^{LL} (GeV)"),
    ("dilep_eta",     "#eta_{LL}"),
    ("dilep_phi",     "#phi_{LL}"),
    ("dilep_mt",      "m_{ T}^{LL} (GeV)"),
    ("dilep_deta",    "|#Delta#eta(L_{0}, L_{1})|"),
    ("dilep_dphi",    "#Delta#phi(L_{0}, L_{1})"),
    ("dilep_dR",      "#Delta R(L_{0}, L_{1})"),
    ("dilep_ptratio", "p_{T1} / p_{T0}"),
    
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
    tag      = "topcr_unscaled" #info['tag']
    text     = "t#bar{t} CR" #info['text']
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
            f'"{text}"'
        )
        command = f"root -q -b -l 'makeStackedPlot.C({arguments})'"
        if test: print(command, style="italic dim")
        if not test: command += " > /dev/null 2>&1" ## supress output
        if not dryrun: subprocess.run(command, shell=True)

    print(f'[yellow]{count}[/yellow] plot(s) made for {jobname}')
    if test: break

end_time = time.time()
time_taken = end_time - start_time

print("\nDone!", style='yellow bold')
print(f"Time taken = {time_taken:.2f} seconds.", style='yellow bold')
print(f"Total number of plots = {plotcount}\n", style='yellow bold')

    

