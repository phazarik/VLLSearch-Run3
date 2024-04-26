import ROOT
import os, sys
import argparse
import time
start_time = time.time()

parser=argparse.ArgumentParser()
parser.add_argument('--jobname',type=str, required=True,  help='Condor job name')
parser.add_argument('--test',   type=bool,required=False, help='Run for only one sample')
parser.add_argument('--dryrun', type=bool,required=False, help='Print statments')
args=parser.parse_args()

jobname = args.jobname
test    = args.test
dryrun  = args.dryrun

def extractHistFromTree(jobname):

    # Define a structure for histograms
    class Histogram:
        def __init__(self, var, nbins, xmin, xmax):
            self.var = var
            self.nbins = nbins
            self.xmin = xmin
            self.xmax = xmax

    # Mention which samples to run on
    samples = ["DYJetsToLL_M10to50","DYJetsToLL_M50",
              "HTbinnedWJets_70to100","HTbinnedWJets_100to200","HTbinnedWJets_200to400","HTbinnedWJets_400to600","HTbinnedWJets_600to800","HTbinnedWJets_800to1200","HTbinnedWJets_1200to2500","HTbinnedWJets_2500toInf",
              "QCD_MuEnriched_20to30","QCD_MuEnriched_30to50","QCD_MuEnriched_50to80", "QCD_MuEnriched_80to120", "QCD_MuEnriched_120to170","QCD_MuEnriched_170to300", "QCD_MuEnriched_300to470", "QCD_MuEnriched_470to600","QCD_MuEnriched_600to800", "QCD_MuEnriched_800to1000",
              "QCD_EMEnriched_15to20","QCD_EMEnriched_20to30","QCD_EMEnriched_30to50","QCD_EMEnriched_50to80", "QCD_EMEnriched_80to120", "QCD_EMEnriched_120to170","QCD_EMEnriched_170to300", "QCD_EMEnriched_300toInf",
              "SingleTop_s-channel_LeptonDecays","SingleTop_t-channel_AntiTop_InclusiveDecays","SingleTop_t-channel_Top_InclusiveDecays","SingleTop_tW_AntiTop_InclusiceDecays","SingleTop_tW_Top_InclusiveDecays",
              "TTBar_TTTo2L2Nu","TTBar_TTToSemiLeptonic",
              "TTW_TTWToLNu",
              "TTZ_TTZToLL",
              "WW_WWTo1L1Nu2Q","WW_WWTo4Q",
              "WZ_WZTo1L1Nu2Q","WZ_WZTo2Q2L","WZ_WZTo3LNu",
              "ZZ_ZZTo2L2Nu","ZZ_ZZTo2Q2L","ZZ_ZZTo2Q2Nu","ZZ_ZZTo4L",
              "VLLS_ele_M100","VLLS_ele_M125","VLLS_ele_M150","VLLS_ele_M200","VLLS_ele_M250","VLLS_ele_M300","VLLS_ele_M350","VLLS_ele_M400","VLLS_ele_M450","VLLS_ele_M500","VLLS_ele_M750","VLLS_ele_M1000",
              "VLLS_mu_M100","VLLS_mu_M125","VLLS_mu_M150","VLLS_mu_M200","VLLS_mu_M250","VLLS_mu_M300","VLLS_mu_M350","VLLS_mu_M400","VLLS_mu_M450","VLLS_mu_M500","VLLS_mu_M750","VLLS_mu_M1000",
              "VLLD_ele_M100","VLLD_ele_M200","VLLD_ele_M300","VLLD_ele_M400","VLLD_ele_M600","VLLD_ele_M800","VLLD_ele_M1000",
              "VLLD_mu_M100","VLLD_mu_M200","VLLD_mu_M300","VLLD_mu_M400","VLLD_mu_M600","VLLD_mu_M800","VLLD_mu_M1000",
              "SingleMuon_SingleMuon_A","SingleMuon_SingleMuon_B","SingleMuon_SingleMuon_C","SingleMuon_SingleMuon_D",
              "EGamma_EGamma_A","EGamma_EGamma_B","EGamma_EGamma_C","EGamma_EGamma_D",
              ]
    #samples = ["DYJetsToLL_M10to50", "DYJetsToLL_M50", "WZ_WZto3LNu"]

    # Booking histograms
    histograms = [
        Histogram("nlep", 10, 0, 10),
        Histogram("njet", 10, 0, 10),
        Histogram("nbjet", 10, 0, 10),

        Histogram("lep0_pt", 50, 0, 500),
        Histogram("lep0_eta", 100, -4, 4),
        Histogram("lep0_phi", 100, -4, 4),
        Histogram("lep0_iso", 200, 0, 1),
        Histogram("lep0_mt", 50, 0, 500),

        Histogram("lep1_pt", 50, 0, 500),
        Histogram("lep1_eta", 100, -4, 4),
        Histogram("lep1_phi", 100, -4, 4),
        Histogram("lep1_iso", 200, 0, 1),
        Histogram("lep1_mt", 50, 0, 500),

        Histogram("dilep_pt", 50, 0, 500),
        Histogram("dilep_eta", 100, -4, 4),
        Histogram("dilep_phi", 100, -4, 4),
        Histogram("dilep_mass", 50, 0, 500),
        Histogram("dilep_mt", 50, 0, 500),
        Histogram("dilep_deta", 100, 0, 6),
        Histogram("dilep_dphi", 100, 0, 6),
        Histogram("dilep_dR", 100, 0, 6),
        Histogram("dilep_ptratio", 100, 0, 1),

        Histogram("metpt", 50, 0, 500),
        Histogram("metphi", 100, -4, 4),
        Histogram("HT", 20, 0, 500),
        #Histogram("LT", 20, 0, 500),
        Histogram("STvis", 20, 0, 500),
        Histogram("ST", 50, 0, 500),
        #Histogram("STtrue", 20, 0, 500),
        Histogram("STfrac", 100, 0, 1.1),
        Histogram("dphi_metlep0", 100, 0, 4),
        Histogram("dphi_metlep1", 100, 0, 4),
        Histogram("dphi_metdilep", 100, 0, 4),
        Histogram("dphi_metlep_max", 100, 0, 4),
        Histogram("dphi_metlep_min", 100, 0, 4),

        Histogram('NNscore', 200, 0, 1)
    ]

    prev_time = start_time

    for s in samples:
        print("Making histograms for "+s+ " ...")
        #indir = "../input_trees/" + jobname + "/"
        indir = "../input_trees_modified/" + jobname + "/"
        outdir = "../input_hists/"+jobname+"/"
        inputfilename = "tree_" + s + ".root"
        outputfilename = "hst_" + s + ".root"

        # Reading the input file and accessing the tree
        if not os.path.exists(indir+inputfilename):
            print("\033[31mError: File not found: "+indir+inputfilename+" \033[0m")
            continue

        infile = ROOT.TFile(indir+inputfilename, "READ")
        tree = infile.Get("myEvents")

        # Generating the output file
        os.system('mkdir -p '+outdir)
        outfile = ROOT.TFile(outdir+outputfilename, "RECREATE")
        if not infile:
            print("\033[31mError: Output file not created for " +s+ " \033[0m")
            continue

        # Looping over each branch to create a histogram
        for hist in histograms:
            h = ROOT.TH1F(hist.var, hist.var, hist.nbins, hist.xmin, hist.xmax)
            h.Sumw2()            

            # Looping over events and filling the histogram along with event weights
            nentries = tree.GetEntries()
            for j in range(nentries):
                tree.GetEntry(j)
                weight = tree.wt
                varValue = getattr(tree, hist.var)
                h.Fill(varValue, weight)

            h.Write()
            #break  # Break here for processing one histogram

        infile.Close()
        outfile.Close()

        current_time = time.time()
        processing_time = current_time - prev_time
        prev_time = current_time
        print(f"\033[33mFile created:\033[0m {outdir+outputfilename} in \033[33m{processing_time:.2f} seconds\033[0m")

        if test: break

extractHistFromTree(jobname)

end_time = time.time()
time_taken = end_time-start_time
print(f"\n\033[33mDone!\nTotal time taken = {time_taken:.2f} seconds.\033[0m")
