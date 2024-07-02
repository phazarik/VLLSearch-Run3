from dataclasses import dataclass
from typing import List
import os, sys
import time

@dataclass
class PlotData:
    var: str
    name: str
    nbins: int
    xmin: float
    xmax: float
    rebin: int

p = [
    PlotData(var="HT",      name="HT (GeV)",              nbins=200, xmin=0, xmax=200, rebin=1),
    #PlotData(var="dilep_mass", name="Dilep mass (GeV)", nbins=200, xmin=0, xmax=200, rebin=2),
    
    PlotData(var="nlep",    name="number of leptons",     nbins=10, xmin=0, xmax=10, rebin=1),
    PlotData(var="njet",    name="number of jets",        nbins=10, xmin=0, xmax=10, rebin=1),
    PlotData(var="nbjet",   name="number of bjets",       nbins=10, xmin=0, xmax=10, rebin=1),
    
    PlotData(var="LT",      name="LT (GeV)",              nbins=200, xmin=0, xmax=200, rebin=1),
    PlotData(var="STvis",   name="HT+LT (GeV)",           nbins=200, xmin=0, xmax=200, rebin=5),
    PlotData(var="ST",      name="ST (GeV)",              nbins=200, xmin=0, xmax=200, rebin=5),
    PlotData(var="STfrac",  name="LT/ST",                 nbins=200, xmin=0, xmax=1.2, rebin=5),
    PlotData(var="HTMETllpt",name="HT+MET+Dilep Pt (GeV)",nbins=200, xmin=0, xmax=200, rebin=5),
    PlotData(var="metpt",   name="MET (GeV)",             nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="metphi",  name="MET phi",               nbins=200, xmin=-4, xmax=4, rebin=5),

    PlotData(var="lep0_pt",    name="Leading lepton pT (GeV)", nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="lep0_eta",   name="Leading lepton eta",      nbins=200, xmin=-4, xmax=4, rebin=5),
    PlotData(var="lep0_phi",   name="Leading lepton phi",      nbins=200, xmin=-4, xmax=4, rebin=5),
    PlotData(var="lep0_mt",    name="Leading lepton mT (GeV)", nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="lep0_iso",   name="Leading lepton reliso03", nbins=1000, xmin=0, xmax=10, rebin=10),
    PlotData(var="lep0_sip3d", name="Leading lepton sip3d",    nbins=500, xmin=0, xmax=50, rebin=20),
    PlotData(var="lep0_deepjet", name="DeepJet score (near L0)", nbins=300, xmin=-1, xmax=2, rebin=10),

    PlotData(var="lep1_pt",    name="SubLeading lepton pT (GeV)", nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="lep1_eta",   name="SubLeading lepton eta",      nbins=200, xmin=-4, xmax=4, rebin=5),
    PlotData(var="lep1_phi",   name="SubLeading lepton phi",      nbins=200, xmin=-4, xmax=4, rebin=5),
    PlotData(var="lep1_mt",    name="SubLeading lepton mT (GeV)", nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="lep1_iso",   name="SubLeading lepton reliso03", nbins=1000, xmin=0, xmax=10, rebin=10),
    PlotData(var="lep1_sip3d", name="SubLeading lepton sip3d",    nbins=500, xmin=0, xmax=50, rebin=20),
    PlotData(var="lep1_deepjet", name="DeepJet score (near L1)",  nbins=300, xmin=-1, xmax=2, rebin=10),
    
    PlotData(var="dilep_pt",   name="Dilep pT (GeV)",   nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="dilep_eta",  name="Dilep eta",        nbins=200, xmin=-4, xmax=4, rebin=5),
    PlotData(var="dilep_phi",  name="Dilep phi",        nbins=200, xmin=-4, xmax=4, rebin=5),
    PlotData(var="dilep_mass", name="Dilep mass (GeV)", nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="dilep_mt",   name="Dilep mT (GeV)",   nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="dilep_deta", name="deta(lep0, lep1)", nbins=200, xmin=0, xmax=6, rebin=5),
    PlotData(var="dilep_dphi", name="dphi(lep0, lep1)", nbins=200, xmin=0, xmax=6, rebin=5),
    PlotData(var="dilep_dR",   name="dR(lep0, lep1)",   nbins=200, xmin=0, xmax=6, rebin=5),

    PlotData(var="dilep_ptratio",   name="pT1/pT0",            nbins=200, xmin=0, xmax=1, rebin=5),
    PlotData(var="dphi_metlep0",    name="dphi(lep0, MET)",    nbins=200, xmin=0, xmax=4, rebin=5),
    PlotData(var="dphi_metlep1",    name="dphi(lep1, MET)",    nbins=200, xmin=0, xmax=4, rebin=5),
    PlotData(var="dphi_metdilep",   name="dphi(dilep, MET)",   nbins=200, xmin=0, xmax=4, rebin=5),
    PlotData(var="dphi_metlep_max", name="max-dphi(lep, MET)", nbins=200, xmin=0, xmax=4, rebin=5),
    PlotData(var="dphi_metlep_min", name="min-dphi(lep, MET)", nbins=200, xmin=0, xmax=4, rebin=5),

    PlotData(var="all_lep_flav_abs", name="lep abs(id)",  nbins=20, xmin=0,   xmax=20, rebin=1),
    PlotData(var="lep0_flav_abs",    name="lep0 abs(id)", nbins=20, xmin=0,   xmax=20, rebin=1),
    PlotData(var="lep1_flav_abs",    name="lep1 abs(id)", nbins=20, xmin=0,   xmax=20, rebin=1),
    PlotData(var="all_lep_flav_raw", name="lep id",       nbins=40, xmin=-20, xmax=20, rebin=1),
    PlotData(var="lep0_flav_raw",    name="lep0 id",      nbins=40, xmin=-20, xmax=20, rebin=1),
    PlotData(var="lep1_flav_raw",    name="lep1 id",      nbins=40, xmin=-20, xmax=20, rebin=1),

    PlotData(var="HT",      name="HT (GeV)",              nbins=200, xmin=0, xmax=200, rebin=1),

    #PlotData(var="overlap_before_trigger", name="overlap before trigger", nbins=10, xmin=0, xmax=10, rebin=1),
    #PlotData(var="overlap_after_trigger",  name="overlap after trigger",  nbins=10, xmin=0, xmax=10, rebin=1),
    #PlotData(var="overlap_ofIsoEle27",     name="overlap of IsoEle27",    nbins=10, xmin=0, xmax=10, rebin=1)
    ]

start_time = time.time()

# Accessing elements of the vector
count = 0
for plot_data in p:
    
    count = count+1
    #print(plot_data.var, plot_data.name, plot_data.nbins, plot_data.xmin, plot_data.xmax, plot_data.rebin)

    p1 = plot_data.var
    p2 = plot_data.name
    p3 = plot_data.nbins
    p4 = plot_data.xmin
    p5 = plot_data.xmax
    p6 = plot_data.rebin

    arguments = f'"{p1}", "{p2}", {p3}, {p4}, {p5}, {p6}'
    processline = "root -q -b -l 'makestack.C("+arguments+")'"
    #print("Executing :" + processline + " ... ", end = "\n")
    os.system(processline)
    #print("\033[033mSuccess.\033[0m")
    break

end_time = time.time()
time_taken = end_time-start_time

print('\n\033[93mDone!\033[0m\n')
print(f'\033[93mtime taken : {time_taken:.2f} seconds\033[0m')
print(f'\033[93mNo. of plots : {count}\033[0m')
