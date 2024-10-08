#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <iostream>
using namespace std;

void processTrees(const char* inputFilename, const char* outputFilename, int channelval) {
  // Opening the ROOT file
  TFile *file = TFile::Open(inputFilename);
  if (!file || file->IsZombie()) {
    std::cerr << "Error opening file: " << inputFilename << std::endl;
    return;
  }

  // Getting the TTree
  TTree *tree = (TTree*)file->Get("myEvents");
  if (!tree) {
    std::cerr << "Error getting TTree from file: " << inputFilename << std::endl;
    file->Close();
    return;
  }
  
  struct plotdata {
    TString branchname_;
    int nbins_;
    float xmin_;
    float xmax_;
    TH1F *hist_ = nullptr;
    void *branchdata_=nullptr;
    char  branchtype_;
  };

  vector<plotdata> plots = {
    {.branchname_ = "channel",    .nbins_ = 10, .xmin_ = 0, .xmax_ = 10,   .branchtype_ = 'I'},
    {.branchname_ = "nlep",       .nbins_ = 10, .xmin_ = 0, .xmax_ = 10,   .branchtype_ = 'I'},
    {.branchname_ = "njet",       .nbins_ = 10, .xmin_ = 0, .xmax_ = 10,   .branchtype_ = 'I'},
    {.branchname_ = "nbjet",      .nbins_ = 10, .xmin_ = 0, .xmax_ = 10,   .branchtype_ = 'I'},
    
    {.branchname_ = "lep0_pt",    .nbins_ = 50, .xmin_ = 0, .xmax_ = 500,  .branchtype_ = 'F'},
    {.branchname_ = "lep0_eta",   .nbins_ = 100, .xmin_ = -4, .xmax_ = 4,  .branchtype_ = 'F'},
    {.branchname_ = "lep0_phi",   .nbins_ = 100, .xmin_ = -4, .xmax_ = 4,  .branchtype_ = 'F'},
    {.branchname_ = "lep0_iso",   .nbins_ = 200, .xmin_ = 0, .xmax_ = 0.2, .branchtype_ = 'F'},
    {.branchname_ = "lep0_sip3d", .nbins_ = 200, .xmin_ = 0, .xmax_ = 50,  .branchtype_ = 'F'},
    {.branchname_ = "lep0_mt",    .nbins_ = 50, .xmin_ = 0, .xmax_ = 500,  .branchtype_ = 'F'},
    
    {.branchname_ = "lep1_pt",    .nbins_ = 50, .xmin_ = 0, .xmax_ = 500,  .branchtype_ = 'F'},
    {.branchname_ = "lep1_eta",   .nbins_ = 100, .xmin_ = -4, .xmax_ = 4,  .branchtype_ = 'F'},
    {.branchname_ = "lep1_phi",   .nbins_ = 100, .xmin_ = -4, .xmax_ = 4,  .branchtype_ = 'F'},
    {.branchname_ = "lep1_iso",   .nbins_ = 200, .xmin_ = 0, .xmax_ = 0.2, .branchtype_ = 'F'},
    {.branchname_ = "lep1_sip3d", .nbins_ = 200, .xmin_ = 0, .xmax_ = 50,  .branchtype_ = 'F'},
    {.branchname_ = "lep1_mt",    .nbins_ = 50, .xmin_ = 0, .xmax_ = 500,  .branchtype_ = 'F'},
    
    {.branchname_ = "dilep_pt",   .nbins_ = 50, .xmin_ = 0, .xmax_ = 500,  .branchtype_ = 'F'},
    {.branchname_ = "dilep_eta",  .nbins_ = 100, .xmin_ = -10, .xmax_ = 10,.branchtype_ = 'F'},
    {.branchname_ = "dilep_phi",  .nbins_ = 100, .xmin_ = -4, .xmax_ = 4,  .branchtype_ = 'F'},
    {.branchname_ = "dilep_mass", .nbins_ = 50, .xmin_ = 0, .xmax_ = 500,  .branchtype_ = 'F'},
    {.branchname_ = "dilep_mt",   .nbins_ = 50, .xmin_ = 0, .xmax_ = 500,  .branchtype_ = 'F'},
    {.branchname_ = "dilep_deta", .nbins_ = 100, .xmin_ = 0, .xmax_ = 6,   .branchtype_ = 'F'},
    {.branchname_ = "dilep_dphi", .nbins_ = 100, .xmin_ = 0, .xmax_ = 6,   .branchtype_ = 'F'},
    {.branchname_ = "dilep_dR",   .nbins_ = 100, .xmin_ = 0, .xmax_ = 6,   .branchtype_ = 'F'},
    {.branchname_ = "dilep_ptratio", .nbins_ = 100, .xmin_ = 0, .xmax_ = 1,.branchtype_ = 'F'},
    
    {.branchname_ = "HT",         .nbins_ = 20, .xmin_ = 0, .xmax_ = 500,  .branchtype_ = 'F'},
    {.branchname_ = "LT",         .nbins_ = 20, .xmin_ = 0, .xmax_ = 500,  .branchtype_ = 'F'},
    {.branchname_ = "STvis",      .nbins_ = 50, .xmin_ = 0, .xmax_ = 500,  .branchtype_ = 'F'},
    {.branchname_ = "ST",         .nbins_ = 50, .xmin_ = 0, .xmax_ = 500,  .branchtype_ = 'F'},
    {.branchname_ = "STfrac",     .nbins_ = 100, .xmin_ = 0, .xmax_ = 1.1, .branchtype_ = 'F'},
    {.branchname_ = "HTMETllpt",  .nbins_ = 50, .xmin_ = 0, .xmax_ = 500,  .branchtype_ = 'F'},
    {.branchname_ = "metpt",      .nbins_ = 50, .xmin_ = 0, .xmax_ = 500,  .branchtype_ = 'F'},
    {.branchname_ = "metphi",     .nbins_ = 100, .xmin_ = -4, .xmax_ = 4,  .branchtype_ = 'F'},
    
    {.branchname_ = "dphi_metlep0", .nbins_ = 100, .xmin_ = 0, .xmax_ = 4, .branchtype_ = 'F'},
    {.branchname_ = "dphi_metlep1", .nbins_ = 100, .xmin_ = 0, .xmax_ = 4, .branchtype_ = 'F'},
    {.branchname_ = "dphi_metdilep", .nbins_ = 100, .xmin_ = 0, .xmax_ = 4, .branchtype_ = 'F'},
    {.branchname_ = "dphi_metlep_max", .nbins_ = 100, .xmin_ = 0, .xmax_ = 4, .branchtype_ = 'F'},
    {.branchname_ = "dphi_metlep_min", .nbins_ = 100, .xmin_ = 0, .xmax_ = 4, .branchtype_ = 'F'},
    
    {.branchname_ = "wt_leptonSF", .nbins_ = 200, .xmin_ = 0, .xmax_ = 2, .branchtype_ = 'D'},
    {.branchname_ = "wt_trig",     .nbins_ = 200, .xmin_ = 0, .xmax_ = 2, .branchtype_ = 'D'},
    {.branchname_ = "wt_bjet",     .nbins_ = 200, .xmin_ = 0, .xmax_ = 2, .branchtype_ = 'D'},
    {.branchname_ = "weight",      .nbins_ = 200, .xmin_ = 0, .xmax_ = 2, .branchtype_ = 'D'},

    {.branchname_ = "nnscore_qcd_vlldmu", .nbins_ = 200, .xmin_ = 0, .xmax_ = 1, .branchtype_ = 'F'},
    {.branchname_ = "nnscore_qcd_vlldele", .nbins_ = 200, .xmin_ = 0, .xmax_ = 1, .branchtype_ = 'F'},
    {.branchname_ = "nnscore_qcd_vlld_combined", .nbins_ = 200, .xmin_ = 0, .xmax_ = 1, .branchtype_ = 'F'},
    
  };

  //-------------------------------------
  //Dynamically declaring the histograms:
  for (int i = 0; i < (int)plots.size(); i++) {
    
    TString branchname = plots[i].branchname_;
    int nbins = plots[i].nbins_;
    float xmin = plots[i].xmin_;
    float xmax = plots[i].xmax_;

    if (plots[i].branchtype_ == 'I') {
      plots[i].branchdata_ = new Long64_t;//Int_t;
      tree->SetBranchAddress(branchname, (Long64_t*)plots[i].branchdata_);
    }
    else if (plots[i].branchtype_ == 'F') {
      plots[i].branchdata_ = new Double_t;//Float_t;
      tree->SetBranchAddress(branchname, (Double_t*)plots[i].branchdata_);
    }
    else if (plots[i].branchtype_ == 'D') {
      plots[i].branchdata_ = new Double_t;
      tree->SetBranchAddress(branchname, (Double_t*)plots[i].branchdata_);
    }
    
    //Replacing the nullptr in the collection:
    plots[i].hist_ = new TH1F(branchname, branchname, nbins, xmin, xmax);
    plots[i].hist_->Sumw2();
  }

  //----------------------------------------
  //Filling the histograms in an event loop:
  Long64_t nentries = tree->GetEntries();
  
  for (Long64_t evt = 0; evt < nentries; evt++) {
    tree->GetEntry(evt);

    //Local variables that extract values from pointers which are pointing to the branches:
    Double_t wt;   //point to "weight"
    Int_t channel; //point to "channel"

    // Extracting data from the right pointer and putting them in the local variables.
    // Reseting branch address will set the previous pointer in branchdata_ to random location in the memeory. 
    for (int i = 0; i < (int)plots.size(); i++) {
      TString branchname = plots[i].branchname_;
      //1. Changing the pointer type from void * to whatever we need *
      //2. Dereferencing to get the value.
      //3. Stpring the value in the local variable.
      if (branchname == "weight")  wt      = *(Double_t*)plots[i].branchdata_;
      if (branchname == "channel") channel = *(Long64_t*)plots[i].branchdata_;     
    }

    //Put event selection using the local variables:
    bool event_selection = (channel == channelval); //Choosing the final state
    
    if (event_selection) {
      
      // Fill all the histograms:
      for (int i = 0; i < (int)plots.size(); i++) {
	TString branchname = plots[i].branchname_;
	
	if (branchname == "wt_leptonSF" ||
	    branchname == "wt_trig" ||
	    branchname == "weight" ||
	    branchname == "wt_bjet") wt = 1.0;

	if (plots[i].branchtype_ == 'I') {
          Int_t value = *(Int_t*)plots[i].branchdata_;
          //cout << "Filling histogram for " << branchname << " with value: " << value << " and weight: " << wt << endl;
          plots[i].hist_->Fill(value, wt);
        }
        else if (plots[i].branchtype_ == 'F') {
          Float_t value = *(Float_t*)plots[i].branchdata_;
          //cout << "Filling histogram for " << branchname << " with value: " << value << " and weight: " << wt << endl;
          plots[i].hist_->Fill(value, wt);
        }
        else if (plots[i].branchtype_ == 'D') {
          Double_t value = *(Double_t*)plots[i].branchdata_;
          //cout << "Filling histogram for " << branchname << " with value: " << value << " and weight: " << wt << endl;
          plots[i].hist_->Fill(value, wt);
        }
	//break;
      }//hist loop
    }//event selection
  }//event loop
  
  // Save histograms to a new ROOT file
  TFile *outputFile = new TFile(outputFilename, "RECREATE");
  for (int i = 0; i < (int)plots.size(); i++) {
    plots[i].hist_->Write();
  }
  
  // Close files
  outputFile->Close();
  file->Close();

  std::cout << "Histograms have been saved to " << outputFilename << std::endl;
}
