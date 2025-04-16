import os, sys
import time

### default parameters:
jobname  = "2025-04-15/hist_Run3Summer22_baseline_mm"
campaign = "Run3Summer22"
channel  = "mm"
test = False

variables = [
    #("nnscore_qcd_vlld_2016preVFP",  "NNScore: QCD vs VLLD (2016-preVFP)"),
    #("nnscore_qcd_vlld_2016postVFP", "NNScore: QCD vs VLLD (2016-postVFP)"),
    #("nnscore_qcd_vlld_2017",        "NNScore: QCD vs VLLD (2017)"),
    #("nnscore_qcd_vlld_2018",        "NNScore: QCD vs VLLD (2018)"),
    #("nnscore_qcd_vlld_2022",        "NNScore: QCD vs VLLD (2022)"),
    
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

    #("2LSS_wt_leptonSF", "Lepton IdIso SF"),
    #("2LSS_wt_trig", "Trigger efficiency SF"),
    #("2LSS_wt_pileup", "PileUp weight"),
    #("2LSS_wt_bjet", "bJet weight"),
    #("2LSS_wt_evt", "Event weight"),
]

def color_print(text, color='red', style='normal'):
    colors = {
        'black': 30, 'red': 31, 'green': 32, 'yellow': 33,
        'blue': 34, 'magenta': 35, 'cyan': 36, 'white': 37
    }
    styles = {'normal':0,'bold':1,'dim':2,'italic':3,'underline':4}
    col = colors.get(color, 31)
    sty = styles.get(style, 0)
    color_text = f"\033[{sty};{col}m{text}\033[0m"
    print(color_text)

start_time = time.time()
count = 0

for var, varname in variables:

    count += 1
    color_print(f"\nPlot no: {count}", 'yellow', 'underline')

    #arguments = f'"{var}", "{varname}", "{jobname}", "{campaign}", "{channel}"'
    arguments = f'"{var}", "{varname}"'
    command   = f"root -q -b -l 'makeStackedPlot.C({arguments})'"

    os.system(command)

    if test: break

end_time = time.time()
time_taken = end_time - start_time

print('')
color_print("Done!", 'yellow', 'bold')
color_print(f"Time taken = {time_taken:.2f} seconds.", 'yellow')
color_print(f"Total number of plots = {count}\n", 'yellow')

    

