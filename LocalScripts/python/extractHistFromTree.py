import ROOT
import os, sys

def extractHistFromTree():
    jobname = "tree_Apr10_2muSS_ST150"

    # Define a structure for histograms
    class Histogram:
        def __init__(self, var, nbins, xmin, xmax):
            self.var = var
            self.nbins = nbins
            self.xmin = xmin
            self.xmax = xmax

    # Mention which samples to run on
    sample = ["DYJetsToLL_M50"]

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
        Histogram("STvis", 20, 0, 500),
        Histogram("ST", 20, 0, 500),
        Histogram("STfrac", 100, 0, 1.1),
        Histogram("dphi_metlep0", 100, 0, 4),
        Histogram("dphi_metlep1", 100, 0, 4),
        Histogram("dphi_metdilep", 100, 0, 4),
        Histogram("dphi_metlep_max", 100, 0, 4),
        Histogram("dphi_metlep_min", 100, 0, 4)
    ]

    for s in sample:
        print("Making histograms for", s)
        indir = "../input_trees/" + jobname + "/"
        outdir = "../StackMaker/input_files/"+jobname+"/"
        inputfilename = "tree_" + s + ".root"
        outputfilename = "hst_" + s + ".root"

        # Reading the input file and accessing the tree
        if not os.path.exists(indir+inputfilename):
            print("Error: File not found: "+indir+inputfilename)
            continue

        infile = ROOT.TFile(indir+inputfilename, "READ")
        tree = infile.Get("myEvents")

        # Generating the output file
        os.system('mkdir -p '+outdir)
        outfile = ROOT.TFile(outdir+outputfilename, "RECREATE")
        if not infile:
            print("Error: Output file not created for", s)
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
        print("File created: "+outdir+outputfilename)

extractHistFromTree()
