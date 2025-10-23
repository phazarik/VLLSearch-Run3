#ifndef EVENTPROCESSOR_H
#define EVENTPROCESSOR_H

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <iostream>
#include <vector>
#include <string>
#include "setBranchesAndHistograms.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;

extern Int_t channel, trigger, nlep, njet, nbjet, nfatjet;
extern Float_t lep0_pt, lep0_eta, lep0_phi, lep0_iso, lep0_sip3d, lep0_mt;
extern Float_t lep1_pt, lep1_eta, lep1_phi, lep1_iso, lep1_sip3d, lep1_mt;
extern Float_t dilep_pt, dilep_eta, dilep_phi, dilep_mass, dilep_mt, dilep_deta, dilep_dphi, dilep_dR, dilep_ptratio;
extern Float_t HT, LT, STvis, ST, HTMETllpt, STfrac, metpt, metphi;
extern Float_t HTfat, STvisfat, STfat, HTfatMETllpt;
extern Float_t LTplusMET, HTplusMET, HTfatplusMET;
extern Float_t dphi_metlep0, dphi_metlep1, dphi_metdilep, dphi_metlep_max, dphi_metlep_min;
extern Float_t nnscore11, nnscore12, nnscore13, nnscore14, nnscore15, nnscore16, nnscore17, nnscore18;
extern Float_t nnscore21, nnscore22, nnscore23, nnscore24;
extern Double_t gen_weight_evt, lumi_weight_evt;
extern Double_t wt_leptonSF, wt_trig, wt_pileup, wt_bjet, wt_pdf, wt_qcdscale;
extern Double_t wt_leptonSF_up, wt_trig_up, wt_pileup_up, wt_bjet_up, wt_pdf_up, wt_qcdscale_up;
extern Double_t wt_leptonSF_down, wt_trig_down, wt_pileup_down, wt_bjet_down, wt_pdf_down, wt_qcdscale_down;
/*
// ------------------- TEST RUN -----------------------
// 1. define all extern variables here
// 2. Load the dependencies: .L eventProcessor.h
// 3. Run the main macro: processTree()
Int_t channel, trigger, nlep, njet, nbjet, nfatjet;
Float_t lep0_pt, lep0_eta, lep0_phi, lep0_iso, lep0_sip3d, lep0_mt;
Float_t lep1_pt, lep1_eta, lep1_phi, lep1_iso, lep1_sip3d, lep1_mt;
Float_t dilep_pt, dilep_eta, dilep_phi, dilep_mass, dilep_mt, dilep_deta, dilep_dphi, dilep_dR, dilep_ptratio;
Float_t HT, LT, STvis, ST, HTMETllpt, STfrac, metpt, metphi;
Float_t HTfat, STvisfat, STfat, HTfatMETllpt;
Float_t LTplusMET, HTplusMET, HTfatplusMET;
Float_t dphi_metlep0, dphi_metlep1, dphi_metdilep, dphi_metlep_max, dphi_metlep_min;
Float_t nnscore1, nnscore2, nnscore3, nnscore4, nnscore5, nnscore6, nnscore7;
Double_t wt_leptonSF, wt_trig, wt_pileup, wt_bjet, weight;
Double_t wt_leptonSF_up, wt_trig_up, wt_pileup_up, wt_bjet_up, weight_up;
Double_t wt_leptonSF_down, wt_trig_down, wt_pileup_down, wt_bjet_down, weight_down;*/

struct hists{
  TString name;
  TString title;
  Int_t nbins;
  Double_t xmin;
  Double_t xmax;
  std::vector<float> binning;  // For custom binning
};

struct hists2D {
    TString name;
    TString title;
    Int_t nbinsX;
    Double_t xmin;
    Double_t xmax;
    Int_t nbinsY;
    Double_t ymin;
    Double_t ymax;
    std::vector<float> binningX;
    std::vector<float> binningY;
};

double getScaleFactorInBins(const char* campaign_cstr, int channelval, double var, const json& scale_factors, TString mode="nom");
double getScaleFactorGlobal(const char* campaign_cstr, int channelval, const json& scale_factors, TString mode="nom");
void fillHistogram(TH1F* hist, float value, float weight = 1.0);
bool find_key(const string& inputFilename, const string& key);

//________________________________________________________________________________________________________________
//
// Process one file:
//________________________________________________________________________________________________________________


void processTree(
		 const char* inputFilename = "/mnt/d/work/GitHub/VLLSearch-Run3/AnalysisScripts/TreeMaker/test_outputs/tree_testcompile.root",
		 const char* outputFilename = "test.root",
		 const char* campaign = "2018_UL",
		 int channelval = 0,
		 float lumisf = 1,
		 bool test = true
		 )
{
  // Load corrections from JSON: (use path from the main macro)
  json sf_qcd   = loadJson("corrections/2L_QCD_global_corrections.json");
  json sf_dy    = loadJson("corrections/2LOS_dycr_SF_inBins_LTplusMET.json");
  json sf_ttbar = loadJson("corrections/2LOS_topcr_SF_inBins_LTplusMET.json");
  cout << "Corrections loaded from JSON." << endl;
  
  vector<TH1D*> hst_collection;
  vector<TH2D*> hst2D_collection;
  //hst_collection->clear();

  vector<float> ptbins500 = {0, 25, 50, 100, 200, 300, 400, 500};
  vector<float> ptbins300 = {0, 25, 50, 100, 200, 300};
  vector<float> isobinslog = {0.002, 0.004, 0.008, 0.016, 0.032, 0.064, 0.128, 0.256};
  //vector<float> searchbins = {200, 300, 350, 400, 450, 500, 550};
  vector<float> searchbins = {0, 200, 400, 450, 500, 550, 600};
  
  vector<hists> hdef = {
    // integers:
    {"channel", "channel", 10, 0, 10, {}},
    {"trigger", "trigger", 10, 0, 10, {}},
    {"nlep", "nlep", 10, 0, 10, {}},
    {"njet", "njet", 10, 0, 10, {}},
    {"nfatjet", "nfatjet", 10, 0, 10, {}},
    {"nbjet", "nbjet", 10, 0, 10, {}},
    // leading lepton:
    {"lep0_pt", "lep0_pt", 50, 0, 500, {}},
    {"lep0_eta", "lep0_eta", 100, -4, 4, {}},
    {"lep0_phi", "lep0_phi", 100, -4, 4, {}},
    //{"lep0_iso", "lep0_iso", 200, 0, 0.2, {}},
    {"lep0_iso", "lep0_iso", 0, 0, 0, isobinslog},
    {"lep0_sip3d", "lep0_sip3d", 200, 0, 50, {}},
    {"lep0_mt", "lep0_mt", 50, 0, 500, {}},
    // subleading lepton:
    {"lep1_pt", "lep1_pt", 50, 0, 500, {}},
    {"lep1_eta", "lep1_eta", 100, -4, 4, {}},
    {"lep1_phi", "lep1_phi", 100, -4, 4, {}},
    {"lep1_iso", "lep1_iso", 0, 0, 0, isobinslog},
    {"lep1_sip3d", "lep1_sip3d", 200, 0, 50, {}},
    {"lep1_mt", "lep1_mt", 50, 0, 500, {}},
    // dilepton system:
    {"dilep_pt", "dilep_pt", 0, 0, 0, ptbins300}, //variable binning
    {"dilep_eta", "dilep_eta", 100, -10, 10, {}},
    {"dilep_phi", "dilep_phi", 100, -4, 4, {}},
    {"dilep_mass", "dilep_mass", 50, 0, 500, {}},
    {"dilep_mt", "dilep_mt", 0, 0, 0, ptbins500}, //variable binning
    {"dilep_deta", "dilep_deta", 100, 0, 6, {}},
    {"dilep_dphi", "dilep_dphi", 100, 0, 6, {}},
    {"dilep_dR", "dilep_dR", 100, 0, 6, {}},
    {"dilep_ptratio", "dilep_ptratio", 100, 0, 1, {}},
    // event level variables:
    {"LT",           "LT",           0, 0, 0, ptbins500}, //variable binning
    {"LTplusMET",    "LTplusMET",    0, 0, 0, searchbins}, //variable binning
    {"HT",           "HT",           0, 0, 0, ptbins500}, //variable binning
    {"HTfat",        "HTfat",        0, 0, 0, ptbins500}, //variable binning
    {"HTplusMET",    "HTplusMET",    0, 0, 0, ptbins500}, //variable binning
    {"HTfatplusMET", "HTfatplusMET", 0, 0, 0, ptbins500}, //variable binning
    {"STvis",        "STvis",        0, 0, 0, ptbins500}, //variable binning
    {"STvisfat",     "STvisfat",     0, 0, 0, ptbins500}, //variable binning
    {"ST",           "ST",           0, 0, 0, ptbins500}, //variable binning
    {"STfat",        "STfat",        0, 0, 0, ptbins500}, //variable binning
    {"HTMETllpt",    "HTMETllpt",    0, 0, 0, ptbins500}, //variable binning
    {"HTfatMETllpt", "HTfatMETllpt", 0, 0, 0, ptbins500}, //variable binning
    {"STfrac", "STfrac", 100, 0, 1.1, {}},
    {"metpt", "metpt", 0, 0, 0, ptbins500}, //variable binning
    {"metphi", "metphi", 100, -4, 4, {}},
    // dphis:
    {"dphi_metlep0", "dphi_metlep0", 100, 0, 4, {}},
    {"dphi_metlep1", "dphi_metlep1", 100, 0, 4, {}},
    {"dphi_metdilep", "dphi_metdilep", 100, 0, 4, {}},
    {"dphi_metlep_max", "dphi_metlep_max", 100, 0, 4, {}},
    {"dphi_metlep_min", "dphi_metlep_min", 100, 0, 4, {}},
    // Event weights:
    {"gen_weight_evt",  "gen_weight_evt",  1000, -5, 5, {}},
    {"lumi_weight_evt", "lumi_weight_evt", 1000, -5, 5, {}},
    // Corrections: nominal:
    {"wt_leptonSF", "wt_leptonSF", 200, 0, 2, {}},
    {"wt_trig",     "wt_trig",     200, 0, 2, {}},
    {"wt_pileup",   "wt_pileup",   200, 0, 2, {}},
    {"wt_bjet",     "wt_bjet",     200, 0, 2, {}},
    {"wt_pdf",      "wt_pdf",      200, 0, 2, {}},
    {"wt_qcdscale", "wt_qcdscale", 200, 0, 2, {}},
    // up variations:
    {"wt_leptonSF_up", "wt_leptonSF_up", 200, 0, 2, {}},
    {"wt_trig_up",     "wt_trig_up",     200, 0, 2, {}},
    {"wt_pileup_up",   "wt_pileup_up",   200, 0, 2, {}},
    {"wt_bjet_up",     "wt_bjet_up",     200, 0, 2, {}},
    {"wt_pdf_up",      "wt_pdf_up",      200, 0, 2, {}},
    {"wt_qcdscale_up", "wt_qcdscale_up", 200, 0, 2, {}},
    // down variations:
    {"wt_leptonSF_down", "wt_leptonSF_down", 200, 0, 2, {}},
    {"wt_trig_down",     "wt_trig_down",     200, 0, 2, {}},
    {"wt_pileup_down",   "wt_pileup_down",   200, 0, 2, {}},
    {"wt_bjet_down",     "wt_bjet_down",     200, 0, 2, {}},
    {"wt_pdf_down",      "wt_pdf_down",      200, 0, 2, {}},
    {"wt_qcdscale_down", "wt_qcdscale_down", 200, 0, 2, {}},
    //Final weight:
    {"wt_evt", "weight", 200, 0, 2, {}},
    // nnscores:
    {"nnscore_2LSS_Run2_vlld_qcd",   "nnscore_2LSS_Run2_vlld_qcd",   200, 0, 1, {}},
    {"nnscore_2LSS_Run2_vlld_ttbar", "nnscore_2LSS_Run2_vlld_ttbar", 200, 0, 1, {}},
    {"nnscore_2LSS_Run2_vlld_wjets", "nnscore_2LSS_Run2_vlld_wjets", 200, 0, 1, {}},
    {"nnscore_2LSS_Run2_vlld_dy",    "nnscore_2LSS_Run2_vlld_dy",    200, 0, 1, {}},
    {"nnscore_2LSS_Run3_vlld_qcd",   "nnscore_2LSS_Run3_vlld_qcd",   200, 0, 1, {}},
    {"nnscore_2LSS_Run3_vlld_ttbar", "nnscore_2LSS_Run3_vlld_ttbar", 200, 0, 1, {}},
    {"nnscore_2LSS_Run3_vlld_wjets", "nnscore_2LSS_Run3_vlld_wjets", 200, 0, 1, {}},
    {"nnscore_2LSS_Run3_vlld_dy",    "nnscore_2LSS_Run3_vlld_dy",    200, 0, 1, {}},
    {"nnscore_2LOS_Run2_vlld_ttbar", "nnscore_2LOS_Run2_vlld_ttbar", 200, 0, 1, {}},
    {"nnscore_2LOS_Run2_vlld_dy",    "nnscore_2LSS_Run2_vlld_dy",    200, 0, 1, {}},
    {"nnscore_2LOS_Run3_vlld_ttbar", "nnscore_2LOS_Run3_vlld_ttbar", 200, 0, 1, {}},
    {"nnscore_2LOS_Run3_vlld_dy",    "nnscore_2LSS_Run3_vlld_dy",    200, 0, 1, {}},
  };
  vector<hists2D> h2Ddef = {
    {"nnscore_2LSS_Run2_vlld_1v3", "nnscore_2LSS_Run2_vlld_1v3", 200, 0, 1, 200, 0, 1, {}},
    {"nnscore_2LSS_Run3_vlld_1v3", "nnscore_2LSS_Run3_vlld_1v3", 200, 0, 1, 200, 0, 1, {}},
    {"nnscore_2LOS_Run2_vlld_1v2", "nnscore_2LOS_Run2_vlld_1v2", 200, 0, 1, 200, 0, 1, {}},
    {"nnscore_2LOS_Run3_vlld_1v2", "nnscore_2LOS_Run3_vlld_1v2", 200, 0, 1, 200, 0, 1, {}},
  };
    
  //Booking 1D histograms:
  for (int i=0; i<(int)hdef.size(); i++) {
    TH1D* hist;
    //cout<<hdef[i].name <<"\t"<< hdef[i].title <<"\t"<< hdef[i].nbins <<"\t"<< hdef[i].xmin <<"\t"<< hdef[i].xmax<<endl;
    if ((int)hdef[i].binning.size()!=0) hist = new TH1D(hdef[i].name, hdef[i].title, hdef[i].binning.size() - 1, &hdef[i].binning[0]);
    else                                hist = new TH1D(hdef[i].name, hdef[i].title, hdef[i].nbins, hdef[i].xmin, hdef[i].xmax);
    hist->Sumw2();
    hist->SetDirectory(0);
    hst_collection.push_back(hist);
  }
  cout<<"hst_collection size = "<<(int)hst_collection.size()<<"\033[0m"<<endl;

  // Booking 2D histograms:
  for (int i = 0; i < (int)h2Ddef.size(); i++) {
    TH2D* hist2D;
    if ((int)h2Ddef[i].binningX.size() != 0 && (int)h2Ddef[i].binningY.size() != 0)
      hist2D = new TH2D(h2Ddef[i].name, h2Ddef[i].title,
			h2Ddef[i].binningX.size() - 1, &h2Ddef[i].binningX[0],
			h2Ddef[i].binningY.size() - 1, &h2Ddef[i].binningY[0]);
    else
      hist2D = new TH2D(h2Ddef[i].name, h2Ddef[i].title,
			h2Ddef[i].nbinsX, h2Ddef[i].xmin, h2Ddef[i].xmax,
			h2Ddef[i].nbinsY, h2Ddef[i].ymin, h2Ddef[i].ymax);
    hist2D->Sumw2();
    hist2D->SetDirectory(0);
    hst2D_collection.push_back(hist2D);
  }

  //________________________________________________________________________________________________
  //
  // Event loop:
  //________________________________________________________________________________________________

  // Opening the ROOT file
  TFile *file = TFile::Open(inputFilename);
  if (!file || file->IsZombie() || !file->IsOpen()) {
    cerr << "Error opening file: " << inputFilename << endl;
    return;
  }

  //-------------------------------------------------------------------------
  //Flagging specific files for corrections:
  bool flag_qcd   = find_key(inputFilename, "_QCDEM_") || find_key(inputFilename, "_QCDMu_");
  bool flag_dy    = (channelval==0 || channelval==3) && find_key(inputFilename, "_DYto2L_");
  bool flag_ttbar = find_key(inputFilename, "_TT_") || find_key(inputFilename, "_TTV_");
  bool flag_vll   = find_key(inputFilename, "_VLLD");
  if(flag_qcd)   cout<<"\033[35;1m==> Correcting QCD globally.\033[0m"<<endl;
  if(flag_dy)    cout<<"\033[35;1m==> Correcting DY in dilep_pt bins.\033[0m"<<endl;
  if(flag_ttbar) cout<<"\033[35;1m==> Correcting tt+X in HT bins.\033[0m"<<endl;
  //-------------------------------------------------------------------------
  
  TTree* tree = (TTree*)file->Get("myEvents");
  setBranches(tree);
  Long64_t nentries = tree->GetEntries();
  for (Long64_t i = 0; i < nentries; i++) {
    
    if(test) cout << "Looping over event " << i << endl;

    tree->GetEntry(i);

    /*
    int verbosity = 100000;
    if(i % verbosity == 0) {
      cout << "Processed " << i << " events ";
      cout << "("<<fixed<<setprecision(2)<<(float(i)/nentries)*100<<"%)";
      cout << defaultfloat <<endl;
    }
    if(i==nentries-1) cout << "Processed " << nentries << " events (100%)"<<endl;*/

    // Based on the event-level variables available,
    // Put event selections here.
    bool event_selection = false;
    bool channel_selection = channel == channelval;
	
    Double_t wt = 1.0;
    if(flag_vll) gen_weight_evt = 1.0;
    wt = wt*gen_weight_evt;                 // Generator weight (can be negative)
    wt = wt*wt_pdf*wt_qcdscale;             // Event-level corrections
    wt = wt*wt_leptonSF*wt_trig*wt_pileup;  // Object-level corrections
    //wt = wt*wt_bjet;                      // b-tagging corrections

    //--------------------------------
    // Corrections to the histograms:
    //--------------------------------

    //1) QCD global correction (same as 2LSS):
    if(flag_qcd){
      Double_t scale_qcd = 1.0;
      scale_qcd = (Double_t)getScaleFactorGlobal(campaign, channelval, sf_qcd, "nom");
      wt = wt * scale_qcd;
    }
    //2) DY correction:
    if(flag_dy){
      Double_t scale_dy = 1.0;
      scale_dy = (Double_t)getScaleFactorInBins(campaign, channelval, LTplusMET, sf_dy, "nom");
      wt = wt * scale_dy;
    }
    //3) TTBar+TTV:
    if(flag_ttbar){
      Double_t scale_ttbar = 1.0;
      scale_ttbar = getScaleFactorInBins(campaign, channelval, LTplusMET, sf_ttbar, "nom");
      wt = wt * scale_ttbar;
    }
   
    //--------------------------------
    // Filling up the histograms:
    // Caution: Careful with the order of the variables and size of hist_collection!
    //--------------------------------
    
    event_selection = channel_selection && (lep0_iso<0.15 && lep1_iso<0.15);
    if(channelval == 3) event_selection = event_selection && !(76<dilep_mass && dilep_mass<106);

    int count = 0;
    if(event_selection){
      Double_t fnwt = wt;

      count++;
      //if(test) cout << count << " Filling events with weight = " << fnwt << endl;

      // integers:
      hst_collection[0]->Fill(channel, 1.0);
      hst_collection[1]->Fill(trigger, 1.0);
      hst_collection[2]->Fill(nlep, fnwt);
      hst_collection[3]->Fill(njet, fnwt);
      hst_collection[4]->Fill(nfatjet, fnwt);
      hst_collection[5]->Fill(nbjet, fnwt);

      // leading lepton:
      hst_collection[6]->Fill(lep0_pt, fnwt);
      hst_collection[7]->Fill(lep0_eta, fnwt);
      hst_collection[8]->Fill(lep0_phi, fnwt);
      hst_collection[9]->Fill(lep0_iso, fnwt);
      hst_collection[10]->Fill(lep0_sip3d, fnwt);
      hst_collection[11]->Fill(lep0_mt, fnwt);

      // subleading lepton:
      hst_collection[12]->Fill(lep1_pt, fnwt);
      hst_collection[13]->Fill(lep1_eta, fnwt);
      hst_collection[14]->Fill(lep1_phi, fnwt);
      hst_collection[15]->Fill(lep1_iso, fnwt);
      hst_collection[16]->Fill(lep1_sip3d, fnwt);
      hst_collection[17]->Fill(lep1_mt, fnwt);

      // dilepton system:
      hst_collection[18]->Fill(dilep_pt, fnwt);
      hst_collection[19]->Fill(dilep_eta, fnwt);
      hst_collection[20]->Fill(dilep_phi, fnwt);
      hst_collection[21]->Fill(dilep_mass, fnwt);
      hst_collection[22]->Fill(dilep_mt, fnwt);
      hst_collection[23]->Fill(dilep_deta, fnwt);
      hst_collection[24]->Fill(dilep_dphi, fnwt);
      hst_collection[25]->Fill(dilep_dR, fnwt);
      hst_collection[26]->Fill(dilep_ptratio, fnwt);

      // event level variables:
      hst_collection[27]->Fill(LT, fnwt);
      hst_collection[28]->Fill(LTplusMET, fnwt);
      hst_collection[29]->Fill(HT, fnwt);
      hst_collection[30]->Fill(HTfat, fnwt);
      hst_collection[31]->Fill(HTplusMET, fnwt);
      hst_collection[32]->Fill(HTfatplusMET, fnwt);
      hst_collection[33]->Fill(STvis, fnwt);
      hst_collection[34]->Fill(STvisfat, fnwt);
      hst_collection[35]->Fill(ST, fnwt);
      hst_collection[36]->Fill(STfat, fnwt);
      hst_collection[37]->Fill(HTMETllpt, fnwt);
      hst_collection[38]->Fill(HTfatMETllpt, fnwt);
      hst_collection[39]->Fill(STfrac, fnwt);
      hst_collection[40]->Fill(metpt, fnwt);
      hst_collection[41]->Fill(metphi, fnwt);

      // dphis:
      hst_collection[42]->Fill(dphi_metlep0, fnwt);
      hst_collection[43]->Fill(dphi_metlep1, fnwt);
      hst_collection[44]->Fill(dphi_metdilep, fnwt);
      hst_collection[45]->Fill(dphi_metlep_max, fnwt);
      hst_collection[46]->Fill(dphi_metlep_min, fnwt);

      // Event weights
      hst_collection[47]->Fill(gen_weight_evt, 1.0);
      hst_collection[48]->Fill(lumi_weight_evt, 1.0);
      
      // Corrections: nominal:
      hst_collection[49]->Fill(wt_leptonSF, 1.0);
      hst_collection[50]->Fill(wt_trig, 1.0);
      hst_collection[51]->Fill(wt_pileup, 1.0);
      hst_collection[52]->Fill(wt_bjet, 1.0);
      hst_collection[53]->Fill(wt_pdf, 1.0);
      hst_collection[54]->Fill(wt_qcdscale, 1.0);

      // up weights:
      hst_collection[55]->Fill(wt_leptonSF_up, 1.0);
      hst_collection[56]->Fill(wt_trig_up, 1.0);
      hst_collection[57]->Fill(wt_pileup_up, 1.0);
      hst_collection[58]->Fill(wt_bjet_up, 1.0);
      hst_collection[59]->Fill(wt_pdf_up, 1.0);
      hst_collection[60]->Fill(wt_qcdscale_up, 1.0);

      // down weights:
      hst_collection[61]->Fill(wt_leptonSF_down, 1.0);
      hst_collection[62]->Fill(wt_trig_down, 1.0);
      hst_collection[63]->Fill(wt_pileup_down, 1.0);
      hst_collection[64]->Fill(wt_bjet_down, 1.0);
      hst_collection[65]->Fill(wt_pdf_down, 1.0);
      hst_collection[66]->Fill(wt_qcdscale_down, 1.0);

      // combined weight:
      hst_collection[67]->Fill(fnwt, 1.0);
      
      // NN scores:
      hst_collection[68]->Fill(nnscore11, fnwt); //Run-2 SS QCD
      hst_collection[69]->Fill(nnscore12, fnwt); //Run-2 SS Top
      hst_collection[70]->Fill(nnscore13, fnwt); //Run-2 SS Wjets
      hst_collection[71]->Fill(nnscore14, fnwt); //Run-2 SS DY
      hst_collection[72]->Fill(nnscore15, fnwt); //Run-3 SS QCD
      hst_collection[73]->Fill(nnscore16, fnwt); //Run-3 SS Top
      hst_collection[74]->Fill(nnscore17, fnwt); //Run-3 SS Wjets
      hst_collection[75]->Fill(nnscore18, fnwt); //Run-3 SS DY
      hst_collection[76]->Fill(nnscore21, fnwt); //Run-2 OS ttbar
      hst_collection[77]->Fill(nnscore22, fnwt); //Run-2 0S DY
      hst_collection[78]->Fill(nnscore23, fnwt); //Run-3 OS ttbar
      hst_collection[79]->Fill(nnscore24, fnwt); //Run-3 0S DY

      //2D plots
      hst2D_collection[0]->Fill(nnscore11, nnscore13, fnwt); // 2LSS DNN: QCD-vs-Wjets Run2
      hst2D_collection[1]->Fill(nnscore15, nnscore17, fnwt); // 2LSS DNN: QCD-vs-Wjets Run3
      hst2D_collection[2]->Fill(nnscore21, nnscore22, fnwt); // 2LOS DNN: ttbar-vs-DY  Run2
      hst2D_collection[3]->Fill(nnscore23, nnscore24, fnwt); // 2LOS DNN: ttbar-vs-DY  Run3
    }
    //if(i>=100000) break;
  }//Event loop
    
  // Event loop ends
  //________________________________________________________________________________________________
  
  //Luminosity scaling and overflow handling:
  if(test) cout << "Before SetLastBinAsOverflow: channel entries = " << hst_collection[0]->GetEntries() << endl;
  for(int i=0; i<(int)hst_collection.size(); i++){
    if (hst_collection[i]->GetEntries() > 0) SetLastBinAsOverflow(hst_collection[i]);
    hst_collection[i]->Scale(lumisf);
  }
  for(int i = 0; i < (int)hst2D_collection.size(); i++){
    hst2D_collection[i]->Scale(lumisf);
  }
  if(test) cout << "After SetLastBinAsOverflow: channel entries = " << hst_collection[0]->GetEntries() << endl;
  
  // Save histograms to a new ROOT file
  TFile *outputFile = new TFile(outputFilename, "RECREATE");
  for(int i=0; i<(int)hst_collection.size();   i++) hst_collection[i]->Write();
  for(int i=0; i<(int)hst2D_collection.size(); i++) hst2D_collection[i]->Write();

  // Close files
  for(int i=0; i<(int)hst_collection.size();   i++) delete hst_collection[i];
  for(int i=0; i<(int)hst2D_collection.size(); i++) delete hst2D_collection[i];
  outputFile->Close();
  file->Close();

  cout << "Input:  " << inputFilename << endl;
  cout << "Output: " << outputFilename << endl;
}

//_______________________________________________________________________________________________________________
//_______________________________________________________________________________________________________________

double getScaleFactorInBins(const char* campaign_cstr, int channelval, double var, const json& scale_factors, TString mode="nom") {
  std::string campaign(campaign_cstr);
  std::string channel_key = std::to_string(channelval);
  //if (scale_factors.contains(campaign) && scale_factors[campaign].contains(channel_key))
  if (scale_factors.find(campaign) != scale_factors.end() &&
      scale_factors[campaign].find(channel_key) != scale_factors[campaign].end()){
    
    const auto& ranges = scale_factors[campaign][channel_key];
    for (const auto& range : ranges) {
      double low = range["low"];
      double high = 0.0;
      //if (range["high"].is_string() && range["high"] == "inf") high = std::numeric_limits<double>::infinity();
      if (range["high"].is_string() && range["high"] == "inf") high = 9999;
      else high = range["high"];
      if (var >= low && var < high) {
        double sf  = range["scale"][0];
        double err = range["scale"][1];
        if (mode == "nom")  return sf;
        if (mode == "up")   return sf + err;
        if (mode == "down") return sf - err;
      }
    }
  }
  return 1.0;
}

double getScaleFactorGlobal(const char* campaign_cstr, int channelval, const json& scale_factors, TString mode="nom") {
  std::string campaign(campaign_cstr);
  std::string channel_key = std::to_string(channelval);
  //if (scale_factors.contains(campaign) && scale_factors[campaign].contains(channel_key)) {
  if (scale_factors.find(campaign) != scale_factors.end() &&
      scale_factors[campaign].find(channel_key) != scale_factors[campaign].end()) {
    
    double sf = scale_factors[campaign][channel_key][0];
    double err = scale_factors[campaign][channel_key][1];
    if (mode == "nom")  return sf;
    if (mode == "up")   return sf + err;
    if (mode == "down") return sf - err;

  }
  return 1.0;
}

void fillHistogram(TH1F* hist, float value, float weight = 1.0) {
    if      (value < hist->GetXaxis()->GetXmin()) hist->Fill(hist->GetXaxis()->GetXmin(), weight);
    else if (value > hist->GetXaxis()->GetXmax()) hist->Fill(hist->GetXaxis()->GetXmax(), weight);
    else     hist->Fill(value, weight);
}

bool find_key(const string& inputFilename, const string& key) {
    string baseFilename = inputFilename.substr(inputFilename.find_last_of("/\\") + 1);
    return baseFilename.find(key) != string::npos;
}

#endif //EVENTPROCESSOR_H
