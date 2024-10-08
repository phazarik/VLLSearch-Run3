import ROOT
import os, sys
import argparse
import time
start_time = time.time()

parser=argparse.ArgumentParser()
parser.add_argument('--jobname',type=str, required=True,  help='Condor job name')
parser.add_argument('--dump',   type=str, required=True,  help='Output job name')
parser.add_argument('--channel',type=str, required=True,  help='Mention channel name: ee, em, me or mm')
parser.add_argument('--test',   type=bool,required=False, help='Run for only one sample')
parser.add_argument('--dryrun', type=bool,required=False, help='Print statments')
args=parser.parse_args()

jobname = args.jobname
dump    = args.dump
channel = args.channel
test    = args.test
dryrun  = args.dryrun

def extractHistFromTree(jobname, channel):

    chval = -1
    if channel == 'mm':   chval = 0
    elif channel == 'me': chval = 1
    elif channel == 'em': chval = 2
    elif channel == 'ee': chval = 3
    else: print("Pick one: ee, em, me or mm.")

    # Mention which samples to run on
    samples = [
        #QCD samples:
        "QCD_MuEnriched_20to30","QCD_MuEnriched_30to50","QCD_MuEnriched_50to80", "QCD_MuEnriched_80to120", "QCD_MuEnriched_120to170","QCD_MuEnriched_170to300", "QCD_MuEnriched_300to470", "QCD_MuEnriched_470to600","QCD_MuEnriched_600to800", "QCD_MuEnriched_800to1000", "QCD_EMEnriched_15to20","QCD_EMEnriched_20to30","QCD_EMEnriched_30to50","QCD_EMEnriched_50to80", "QCD_EMEnriched_80to120", "QCD_EMEnriched_120to170","QCD_EMEnriched_170to300", "QCD_EMEnriched_300toInf",

        #Single-t samples:
        "SingleTop_s-channel_LeptonDecays","SingleTop_t-channel_AntiTop_InclusiveDecays","SingleTop_t-channel_Top_InclusiveDecays","SingleTop_tW_AntiTop_InclusiceDecays","SingleTop_tW_Top_InclusiveDecays",

        #TH+X samples:
        "Rare_THQ", "Rare_THW",

        #TZq:
        "Rare_TZq_ll",
        
        #ttbar samples:
        "TTBar_TTTo2L2Nu","TTBar_TTToSemiLeptonic",

        #tt+V samples:
        "TTW_TTWToLNu","TTZ_TTZToLL",

        #tt+X samples:
        "Rare_TTHH","Rare_TTTJ","Rare_TTTT","Rare_TTTW","Rare_TTWH","Rare_TTWW","Rare_TTWZ","Rare_TTZH","Rare_TTZZ",

        #V+X samples:
        "HTbinnedWJets_70to100","HTbinnedWJets_100to200","HTbinnedWJets_200to400","HTbinnedWJets_400to600","HTbinnedWJets_600to800","HTbinnedWJets_800to1200","HTbinnedWJets_1200to2500","HTbinnedWJets_2500toInf",
        "WGamma_WGToLNuG_01J","DYJetsToLL_M10to50","DYJetsToLL_M50","ZGamma_ZGToLLG_01J",
              
        #VV samples:      
        "WW_WWTo1L1Nu2Q","WW_WWTo2L2Nu","WW_WWTo4Q","WZ_WZTo1L1Nu2Q","WZ_WZTo2Q2L","WZ_WZTo3LNu","ZZ_ZZTo2L2Nu","ZZ_ZZTo2Q2L","ZZ_ZZTo2Q2Nu","ZZ_ZZTo4L",

        #WpWp:
        "WpWp_WpWpJJEWK","WpWp_WpWpJJQCD",

        #VVV samples:
        "WWW_Inclusive","WWZ_Inclusive","WZZ_Inclusive","ZZZ_Inclusive",

        #Higgs samples:
        "Higgs_bbH_HToZZTo4L","Higgs_GluGluHToZZTo4L","Higgs_GluGluToZH_HToZZTo4L","Higgs_GluGluZH_HToWW_ZTo2L","Higgs_ttHToNonbb","Higgs_VBF_HToZZTo4L","Higgs_VHToNonbb",

        #Signal:
        "VLLS_ele_M100","VLLS_ele_M125","VLLS_ele_M150","VLLS_ele_M200","VLLS_ele_M250","VLLS_ele_M300","VLLS_ele_M350","VLLS_ele_M400","VLLS_ele_M450","VLLS_ele_M500","VLLS_ele_M750","VLLS_ele_M1000",
        "VLLS_mu_M100","VLLS_mu_M125","VLLS_mu_M150","VLLS_mu_M200","VLLS_mu_M250","VLLS_mu_M300","VLLS_mu_M350","VLLS_mu_M400","VLLS_mu_M450","VLLS_mu_M500","VLLS_mu_M750","VLLS_mu_M1000",
        "VLLD_ele_M100","VLLD_ele_M200","VLLD_ele_M300","VLLD_ele_M400","VLLD_ele_M600","VLLD_ele_M800","VLLD_ele_M1000",
        "VLLD_mu_M100","VLLD_mu_M200","VLLD_mu_M300","VLLD_mu_M400","VLLD_mu_M600","VLLD_mu_M800","VLLD_mu_M1000",

        #Data:
        "SingleMuon_SingleMuon_A","SingleMuon_SingleMuon_B","SingleMuon_SingleMuon_C","SingleMuon_SingleMuon_D",
        "EGamma_EGamma_A","EGamma_EGamma_B","EGamma_EGamma_C","EGamma_EGamma_D"
    ]

    prev_time = start_time

    for s in samples:
        print("Making histograms for \033[33m"+s+ "\033[0m ...")
        indir = "../input_trees_modified/" + jobname + "/"
        #indir = "../input_trees_modified/" + jobname + "/"
        outdir = "../input_hists/"+dump+"/"
        inputfilename = indir+"tree_" + s + ".root"
        outputfilename = outdir+"hst_" + s + ".root"

        # Reading the input file and accessing the tree
        if not os.path.exists(inputfilename):
            print("\033[31mError: File not found: "+inputfilename+" \033[0m")
            continue

        # Generating the output directory
        os.makedirs(outdir, exist_ok=True)
        arguments = f'"{inputfilename}", "{outputfilename}", {chval}'
        processline = f"root -q -b -l 'processTrees_simpler.C({arguments})'"
        if dryrun: print(processline)
        else:  os.system(processline)
        
        current_time = time.time()
        processing_time = current_time - prev_time
        prev_time = current_time
        print(f"\033[33mFile created:\033[0m {outputfilename} in \033[33m{processing_time:.2f} seconds\033[0m")
        print('-'*120)

        if test: break

extractHistFromTree(jobname, channel)

end_time = time.time()
time_taken = end_time-start_time
print(f"\n\033[33mtime taken = {time_taken:.2f} seconds.\033[0m")
