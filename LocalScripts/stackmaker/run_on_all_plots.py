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

    #PlotData(var="2LSS_wt_leptonSF", name="Lepton IdIso SF",   nbins=200, xmin=0, xmax=2, rebin=5),
    #PlotData(var="2LSS_wt_trig", name="Trigger efficiency SF", nbins=200, xmin=0, xmax=2, rebin=5),
    #PlotData(var="2LSS_wt_bjet", name="bJet weight",           nbins=200, xmin=0, xmax=2, rebin=5),
    #PlotData(var="2LSS_wt_evt",   name="Event weight (leptonSF * trigger)", nbins=200, xmin=0, xmax=2, rebin=5),
    
    PlotData(var="nlep",    name="N_{L}",                 nbins=10, xmin=0, xmax=10, rebin=1),
    PlotData(var="njet",    name="N_{J}",                 nbins=10, xmin=0, xmax=10, rebin=1),
    PlotData(var="nbjet",   name="N_{bJ}",               nbins=10, xmin=0, xmax=10, rebin=1),

    PlotData(var="HT",      name="H_{ T} (GeV)",           nbins=200, xmin=0, xmax=200, rebin=1),
    PlotData(var="LT",      name="L_{T} (GeV)",           nbins=200, xmin=0, xmax=200, rebin=1),
    PlotData(var="STvis",   name="H_{ T} + L_{T} (GeV)",   nbins=200, xmin=0, xmax=200, rebin=5),
    PlotData(var="ST",      name="S_{T} (GeV)",           nbins=200, xmin=0, xmax=200, rebin=5),
    PlotData(var="STfrac",  name="L_{T}/S_{T}",           nbins=200, xmin=0, xmax=1.2, rebin=5),
    PlotData(var="HTMETllpt", name="H_{ T} + p_{T}^{miss} + p_{T}^{LL} (GeV)", nbins=200, xmin=0, xmax=200, rebin=5),
    PlotData(var="metpt",   name="p_{T}^{miss} (GeV)",    nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="metphi",  name="#phi^{miss}",         nbins=200, xmin=-4, xmax=4, rebin=5),

    PlotData(var="lep0_pt",    name="p_{T}(L_{0}) (GeV)", nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="lep0_eta",   name="#eta(L_{0})",        nbins=200, xmin=-4, xmax=4, rebin=5),
    PlotData(var="lep0_phi",   name="#phi(L_{0})",        nbins=200, xmin=-4, xmax=4, rebin=5),
    PlotData(var="lep0_mt",    name="m_{ T}(L_{0}) (GeV)", nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="lep0_iso",   name="reliso03 (L_{0})",   nbins=1000, xmin=0, xmax=10, rebin=5),
    PlotData(var="lep0_sip3d", name="sip3d (L_{0})",      nbins=500, xmin=0, xmax=50, rebin=20),
    PlotData(var="lep0_deepjet", name="DeepJet score (J near L_{0})", nbins=300, xmin=-1, xmax=2, rebin=10),

    PlotData(var="lep1_pt",    name="p_{T}(L_{1}) (GeV)", nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="lep1_eta",   name="#eta(L_{1})",        nbins=200, xmin=-4, xmax=4, rebin=5),
    PlotData(var="lep1_phi",   name="#phi(L_{1})",        nbins=200, xmin=-4, xmax=4, rebin=5),
    PlotData(var="lep1_mt",    name="m_{ T}(L_{1}) (GeV)", nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="lep1_iso",   name="reliso03 (L_{1})",   nbins=1000, xmin=0, xmax=10, rebin=5),
    PlotData(var="lep1_sip3d", name="sip3d (L_{1})",      nbins=500, xmin=0, xmax=50, rebin=20),
    PlotData(var="lep1_deepjet", name="DeepJet score (J near L_{1})", nbins=300, xmin=-1, xmax=2, rebin=10),

    PlotData(var="dilep_pt",   name="p_{T}^{LL} (GeV)",   nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="dilep_eta",  name="#eta_{LL}",        nbins=200, xmin=-4, xmax=4, rebin=5),
    PlotData(var="dilep_phi",  name="#phi_{LL}",        nbins=200, xmin=-4, xmax=4, rebin=5),
    PlotData(var="dilep_mt",   name="m_{ T}^{LL} (GeV)",   nbins=200, xmin=0, xmax=200, rebin=2),
    PlotData(var="dilep_deta", name="|#Delta#eta(L_{0}, L_{1})|", nbins=200, xmin=0, xmax=6, rebin=5),
    PlotData(var="dilep_dphi", name="#Delta#phi(L_{0}, L_{1})", nbins=200, xmin=0, xmax=6, rebin=5),
    PlotData(var="dilep_dR",   name="#Delta R(L_{0}, L_{1})",   nbins=200, xmin=0, xmax=6, rebin=5),

    PlotData(var="dilep_ptratio",   name="p_{T1} / p_{T0}",    nbins=200, xmin=0, xmax=1, rebin=5),
    PlotData(var="dphi_metlep0",    name="#Delta#phi(L_{0}, p_{T}^{miss})", nbins=200, xmin=0, xmax=4, rebin=5),
    PlotData(var="dphi_metlep1",    name="#Delta#phi(L_{1}, p_{T}^{miss})", nbins=200, xmin=0, xmax=4, rebin=5),
    PlotData(var="dphi_metdilep",   name="#Delta#phi(LL, p_{T}^{miss})", nbins=200, xmin=0, xmax=4, rebin=5),
    PlotData(var="dphi_metlep_max", name="max(#Delta#phi(L, p_{T}^{miss}))", nbins=200, xmin=0, xmax=4, rebin=5),
    PlotData(var="dphi_metlep_min", name="min(#Delta#phi(L, p_{T}^{miss}))", nbins=200, xmin=0, xmax=4, rebin=5),

    PlotData(var="all_lep_flav_abs", name="L |id|",  nbins=20, xmin=0, xmax=20, rebin=1),
    PlotData(var="lep0_flav_abs",    name="L_{0} |id|", nbins=20, xmin=0, xmax=20, rebin=1),
    PlotData(var="lep1_flav_abs",    name="L_{1} |id|", nbins=20, xmin=0, xmax=20, rebin=1),
    PlotData(var="all_lep_flav_raw", name="L id",       nbins=40, xmin=-20, xmax=20, rebin=1),
    PlotData(var="lep0_flav_raw",    name="L_{0} id",    nbins=40, xmin=-20, xmax=20, rebin=1),
    PlotData(var="lep1_flav_raw",    name="L_{1} id",    nbins=40, xmin=-20, xmax=20, rebin=1),

    PlotData(var="2LSS_wt_leptonSF", name="Lepton IdIso SF", nbins=200, xmin=0, xmax=2, rebin=1),
    PlotData(var="2LSS_wt_trig", name="Trigger efficiency SF", nbins=200, xmin=0, xmax=2, rebin=1),
    PlotData(var="2LSS_wt_bjet", name="bJet weight", nbins=200, xmin=0, xmax=2, rebin=1),
    PlotData(var="2LSS_wt_evt",   name="Event weight (leptonSF * trigger)", nbins=200, xmin=0, xmax=2, rebin=1),

    PlotData(var="dilep_mass", name="M_{LL} (GeV)", nbins=200, xmin=0, xmax=200, rebin=2)
    
    #PlotData(var="overlap_before_trigger", name="overlap before trigger", nbins=10, xmin=0, xmax=10, rebin=1),
    #PlotData(var="overlap_after_trigger",  name="overlap after trigger",  nbins=10, xmin=0, xmax=10, rebin=1),
    #PlotData(var="overlap_ofIsoEle27",     name="overlap of IsoEle27",    nbins=10, xmin=0, xmax=10, rebin=1)
]

start_time = time.time()

# Accessing elements of the vector
count = 0
for i, plot_data in enumerate(p):
    
    count = count+1
    print(f'\n\033[93mPlot no: {count}\033[0m')
    #print(plot_data.var, plot_data.name, plot_data.nbins, plot_data.xmin, plot_data.xmax, plot_data.rebin)

    p1 = plot_data.var
    p2 = plot_data.name
    p3 = plot_data.nbins
    p4 = plot_data.xmin
    p5 = plot_data.xmax
    p6 = plot_data.rebin

    arguments = f'"{p1}", "{p2}", {p3}, {p4}, {p5}, {p6}'
    #processline = "root -q -b -l 'makestack.C("+arguments+")'"
    processline = "root -q -b -l 'makeplotForCMS.C("+arguments+")'"
    #print("Executing :" + processline + " ... ", end = "\n")
    os.system(processline)
    #print("\033[033mSuccess.\033[0m")
    #if i==4: break

end_time = time.time()
time_taken = end_time-start_time

print('\n\033[93mDone!\033[0m\n')
print(f'\033[93mtime taken : {time_taken:.2f} seconds\033[0m')
print(f'\033[93mNo. of plots : {count}\033[0m')
