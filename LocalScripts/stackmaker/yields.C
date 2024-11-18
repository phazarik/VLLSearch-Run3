#include <iostream>
#include <fstream>
#include <vector>
#include <utility> // for std::pair
#include <algorithm>
#include <map>
#include <iomanip>
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1F.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TCut.h"
#include "TMathText.h"
#include "TLine.h"
#include "settings.h"
#include "decorations.h"
#include "setglobalparameters.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;

//Global variables need to be declared here.
TString input_path;
int nbins;
float xmin;
float xmax;
int rebin;
float globalSbyB;
float globalObsbyExp;
float globalObsbyExpErr;
double QCDscale, TOPscale, DYscale;
bool toZoom;
TCanvas *canvas;
TPad *mainPad;
TPad *ratioPad;
TH1F *dummy;

//Others:
bool toSave;
bool toLog;
bool toOverlayData;
TString tag, tag2, tag3, info; //Additional info while saving the plots
TString channel;
json lumiData;
double datalumi;
TString campaign, campaign_name;

void reset_if_not_null(TH1F *hist){
  if(hist != nullptr)
    hist->Reset();
}

TH1F * return_bkg(TString var);
TH1F * return_data(TString var);
  
//------------------
// Yield calculator 
//------------------

void yields(){

  time_t start, end;
  time(&start);

  //SET GLOBAL SETTINGS HERE:
  channel = "em";
  TString jobname    = "hist_2LSS_2018UL_SE1_Nov18_"+channel;
  TString savefolder = "yields_2018UL_SE1_"+channel;
  TString dump_folder = "signalyields/"+savefolder;
  //TString var = "nnscore_qcd_vlldmu_200_800";
  TString var = "dilep_mt";
  campaign = "2018_UL";
  input_path = "../input_hists/"+jobname;
  globalSbyB = 0;
  toOverlayData = false;

  InitializeValues();
  SetLumi();
  
  rebin = 1;

  TH1F *hst_bkg  = return_bkg(var);
  TH1F *hst_data = hst_bkg;
  if(toOverlayData) hst_data = return_data(var);

  vector<pair<TString, TString>> vlld = {
    {"VLLD_mu", "M100"},
    {"VLLD_mu", "M200"},
    {"VLLD_mu", "M300"},
    {"VLLD_mu", "M400"},
    {"VLLD_mu", "M600"},
    {"VLLD_mu", "M800"},
    {"VLLD_ele", "M100"},
    {"VLLD_ele", "M200"},
    {"VLLD_ele", "M300"},
    {"VLLD_ele", "M400"},
    {"VLLD_ele", "M600"},
    {"VLLD_ele", "M800"},
    {"VLLD_ele", "M1000"}
  };
  
  createFolder(dump_folder);
  cout<<"\nFolder created: "+dump_folder<<endl;

  //Iterating over signals:
  int count = 0;
  for (const auto& entry : vlld){
    TString sample = entry.first;
    TString subsample = entry.second;
    TString filename = dump_folder+"/yield_"+sample+"_"+subsample+".txt";
    
    // Retrieve the histogram
    TH1F* hist = get_hist(var, sample, subsample);
    if (!hist) {
      cerr<<"Failed to retrieve histogram for "<<sample<< ", " <<subsample<<endl;
      continue;
    }

    count = count+1;
    int nbins = hist->GetNbinsX();
    cout <<"Yields for "<<sample<<" "<<subsample<<" : \033[93m"<<filename<<"\033[0m"<<endl;

    if(count == 1){
      cout <<"\n"<< string(80, '-') << endl;
      cout << setw(8) << "Bin" 
	   << setw(12) << "Nsig" 
	   << setw(12) << "Ndata" 
	   << setw(12) << "Nbkg" 
	   << setw(12) << "NbkgErr" 
	   << setw(12) << "S/B"
	   << setw(12) << "dbkg"
	   << endl;
      cout << string(80, '-') << endl;
    }

    //Create a text file for each sample
    ofstream outfile(filename.Data());
    if (!outfile.is_open()) {
      cerr << "Failed to open file: " << filename << endl;
      continue;
    }

    //Iterating over bins:
    for (int bin = 1; bin <= nbins; ++bin) {
      //float xbinlow = hist->GetXaxis()->GetBinLowEdge(bin);
      //float xbinhigh = hist->GetXaxis()->GetBinUpEdge(bin);
      float nsig = hist->GetBinContent(bin);
      float nsigerr = hist->GetBinError(bin);
      float nbkg = hst_bkg->GetBinContent(bin);
      float nbkgerr = hst_bkg->GetBinError(bin);
      int ndata = (int)hst_data->GetBinContent(bin);
      float sbyb = 0;
      float relative_bkgerr = 0;
      float dbkg = 0;
      if(nbkg!=0){
	sbyb = nsig / sqrt(nbkg);
	relative_bkgerr = nbkgerr/nbkg;
	dbkg = 1+relative_bkgerr;
      }

      if(count == 1){
	cout << setw(8) << bin
	     << setw(12) << nsig
	     << setw(12) << ndata
	     << setw(12) << nbkg
	     << setw(12) << nbkgerr
	     << setw(12) << sbyb
	     << setw(12) << dbkg
	     << endl;
      }

      // Write to file
      outfile << setw(8) << bin
	      << setw(12) << nsig
	      << setw(12) << ndata
	      << setw(12) << nbkg
	      << setw(12) << nbkgerr
	      << setw(12) << sbyb
	      << setw(12) << dbkg
	      << endl;
    }//bins
    if(count == 1) cout<<""<<endl;
    outfile.close();
    //break; //sample
  }

  cout<<"\nDone! Files saved to: "<<dump_folder<<endl;
}

//-------------------------------------------------------------------------------------------
// DO NOT TOUCH BELOW
//-------------------------------------------------------------------------------------------

//---------------------
// Reading backgrounds
//---------------------
TH1F* return_bkg(TString var){

  vector<TH1F*> bkg;
  bkg.clear();

  //QCD processes:
  vector<TH1F *> QCD = {       
    get_hist(var, "QCD_MuEnriched", "20to30"),
    get_hist(var, "QCD_MuEnriched", "30to50"),
    get_hist(var, "QCD_MuEnriched", "50to80"),
    get_hist(var, "QCD_MuEnriched", "80to120"),
    get_hist(var, "QCD_MuEnriched", "120to170"),
    get_hist(var, "QCD_MuEnriched", "170to300"),
    get_hist(var, "QCD_MuEnriched", "300to470"),
    get_hist(var, "QCD_MuEnriched", "470to600"),
    get_hist(var, "QCD_MuEnriched", "600to800"),
    get_hist(var, "QCD_MuEnriched", "800to1000"),
    get_hist(var, "QCD_EMEnriched", "15to20"),
    get_hist(var, "QCD_EMEnriched", "20to30"),
    get_hist(var, "QCD_EMEnriched", "30to50"),
    get_hist(var, "QCD_EMEnriched", "50to80"),
    get_hist(var, "QCD_EMEnriched", "80to120"),
    get_hist(var, "QCD_EMEnriched", "120to170"),
    get_hist(var, "QCD_EMEnriched", "170to300"),
    get_hist(var, "QCD_EMEnriched", "300toInf"),
  };
  //Bakcgrounds with single-top:
  vector<TH1F *>ST = {
    get_hist(var, "SingleTop", "s-channel_LeptonDecays"),
    get_hist(var, "SingleTop", "t-channel_AntiTop_InclusiveDecays"),
    get_hist(var, "SingleTop", "t-channel_Top_InclusiveDecays"),
    //get_hist(var, "SingleTop", "tW_AntiTop_InclusiveDecays"),
    get_hist(var, "SingleTop", "tW_AntiTop_InclusiceDecays"),
    get_hist(var, "SingleTop", "tW_Top_InclusiveDecays")
  };
  vector<TH1F *>THX = {
    get_hist(var, "Rare", "THQ"),
    get_hist(var, "Rare", "THW"),
  };
  vector<TH1F *>TZq = {
    get_hist(var, "Rare", "tZq_ll"),
  };
  //Backgrounds with multiple tops:
  vector<TH1F *>TTBar={
    get_hist(var, "TTBar", "TTTo2L2Nu"),
    get_hist(var, "TTBar", "TTToSemiLeptonic"),
  };
  vector<TH1F *>TTW ={
    get_hist(var, "TTW", "TTWToLNu")
  };
  vector<TH1F *>TTZ ={
    get_hist(var, "TTZ", "TTZToLL")
  };
  vector<TH1F *>TTX={
    get_hist(var, "Rare", "TTHH"),
    get_hist(var, "Rare", "TTTJ"),
    get_hist(var, "Rare", "TTTT"),
    get_hist(var, "Rare", "TTTW"),
    get_hist(var, "Rare", "TTWH"),
    get_hist(var, "Rare", "TTWW"),
    get_hist(var, "Rare", "TTWZ"),
    get_hist(var, "Rare", "TTZH"),
    get_hist(var, "Rare", "TTZZ")
  };
  //Backgrounds with single V:
  vector<TH1F *>WJets = {
    get_hist(var, "HTbinnedWJets", "70to100"),
    get_hist(var, "HTbinnedWJets", "100to200"),
    get_hist(var, "HTbinnedWJets", "200to400"),
    get_hist(var, "HTbinnedWJets", "400to600"),
    get_hist(var, "HTbinnedWJets", "600to800"),
    get_hist(var, "HTbinnedWJets", "800to1200"),
    get_hist(var, "HTbinnedWJets", "1200to2500"),
    get_hist(var, "HTbinnedWJets", "2500toInf"),
  };
  vector<TH1F *>WGamma={
    get_hist(var, "WGamma", "WGToLNuG_01J")
  };
  vector<TH1F *> DY = {
    get_hist(var, "DYJetsToLL", "M10to50"),
    get_hist(var, "DYJetsToLL", "M50")
  };
  vector<TH1F *>ZGamma={
    get_hist(var, "ZGamma", "ZGToLLG_01J")
  }; 
  //Backgrounds with VV:
  vector<TH1F *>VV={
    get_hist(var, "WW", "WWTo1L1Nu2Q"),
    get_hist(var, "WW", "WWTo2L2Nu"),
    get_hist(var, "WW", "WWTo4Q"),
    get_hist(var, "WZ", "WZTo1L1Nu2Q"),
    get_hist(var, "WZ", "WZTo2Q2L"),
    get_hist(var, "WZ", "WZTo3LNu"),
    get_hist(var, "ZZ", "ZZTo2L2Nu"),
    get_hist(var, "ZZ", "ZZTo2Q2L"),
    get_hist(var, "ZZ", "ZZTo2Q2Nu"),
    get_hist(var, "ZZ", "ZZTo4L"),
  };
  vector<TH1F *>WpWp={
    get_hist(var, "WpWp", "WpWpJJEWK"),
  };
  //Backgrounds with VVV:
  vector<TH1F *>VVV={
    get_hist(var, "WWW", "Inclusive"),
    get_hist(var, "WWZ", "Inclusive"),
    get_hist(var, "WZZ", "Inclusive"),
    get_hist(var, "ZZZ", "Inclusive"),
  };
  //Higgs processes:
  vector<TH1F *>Higgs={
    get_hist(var, "Higgs", "bbH_HToZZTo4L"),
    get_hist(var, "Higgs", "GluGluHToZZTo4L"),
    get_hist(var, "Higgs", "GluGluToZH_HToZZTo4L"),
    get_hist(var, "Higgs", "GluGluZH_HToWW_ZTo2L"),
    get_hist(var, "Higgs", "ttHToNonbb"),
    get_hist(var, "Higgs", "VBF_HToZZTo4L"),
    get_hist(var, "Higgs", "VHToNonbb")
  };

  bkg = {
    merge_and_decorate(QCD,   "QCD",       kYellow),
    //Top backgrounds: more top = darker blue
    merge_and_decorate(ST,    "Single t",  kCyan-7),
    merge_and_decorate(TZq,   "tZq",       kCyan-2),
    merge_and_decorate(THX,   "tH+X",      kViolet+1),
    merge_and_decorate(TTBar, "t#bar{t}",  kAzure+1),
    merge_and_decorate(TTW,   "t#bar{t}W", kAzure+2),
    merge_and_decorate(TTZ,   "t#bar{t}Z", kBlue-3),
    merge_and_decorate(TTX,   "t#bar{t}+X",kBlue+1),
    //Singe V backgrounds: red for Z, gray for W
    merge_and_decorate(DY,    "DY",        kRed-7),
    merge_and_decorate(WJets, "W+jets",    kGray+2),
    merge_and_decorate(ZGamma,"Z#gamma",   kRed-9),
    merge_and_decorate(WGamma,"W#gamma",   kGray+1),
    //VV and VVV: more V: darker green
    merge_and_decorate(VV,    "VV",        kGreen+1),
    merge_and_decorate(WpWp,  "WpWp",      kGreen-5), //weird one
    merge_and_decorate(VVV,   "VVV",       kGreen+3),
    //Higgs:
    merge_and_decorate(Higgs, "Higgs",     kMagenta),
  };
  std::vector<TH1F*> nulls_removed;
  for(size_t i = 0; i < bkg.size(); ++i){
    if   (bkg[i] != nullptr) nulls_removed.push_back(bkg[i]);
    else DisplayText("Remove null hist: "+(TString)bkg[i]->GetName(), 31);
  }
  bkg = nulls_removed;

  //Adding all the backgrounds:
  TH1F *totalbkg = (TH1F*)bkg[0]->Clone(); totalbkg->Reset(); 
  for(int i=0; i<(int)bkg.size(); i++) totalbkg->Add(bkg[i]);

  return totalbkg;
}

//---------------------
// Reading backgrounds
//---------------------
TH1F * return_data(TString var){

  vector<TH1F *>SingleMuon={
    get_hist(var, "SingleMuon", "SingleMuon_A"),
    get_hist(var, "SingleMuon", "SingleMuon_B"),
    get_hist(var, "SingleMuon", "SingleMuon_C"),
    get_hist(var, "SingleMuon", "SingleMuon_D"),
    get_hist(var, "SingleMuon", "SingleMuon_E"),
    get_hist(var, "SingleMuon", "SingleMuon_F"),
  };
  vector<TH1F *>EGamma={
    get_hist(var, "EGamma", "EGamma_A"),
    get_hist(var, "EGamma", "EGamma_B"),
    get_hist(var, "EGamma", "EGamma_C"),
    get_hist(var, "EGamma", "EGamma_D"),
    get_hist(var, "EGamma", "EGamma_E"),
    get_hist(var, "EGamma", "EGamma_F"),
  };

  TH1F *hst_smuon  = merge_and_decorate(SingleMuon, "SingleMuon", kBlack);
  TH1F *hst_egamma = merge_and_decorate(EGamma,     "EGamma",     kBlack);
  TH1F *hst_data = (TH1F *)hst_smuon->Clone();
  if (hst_egamma) {
    for (int bin = 1; bin <= hst_data->GetNbinsX(); bin++) {
      Double_t content_smuon  = hst_data->GetBinContent(bin);
      Double_t error_smuon    = hst_data->GetBinError(bin);
      Double_t content_egamma = hst_egamma->GetBinContent(bin);
      Double_t error_egamma    = hst_egamma->GetBinError(bin);
      hst_data->SetBinContent(bin, content_smuon + content_egamma);
      hst_data->SetBinError(bin, sqrt(error_smuon * error_smuon + error_egamma * error_egamma));
    }
  }
  hst_data->SetName("Data");
  hst_data->SetTitle("Data");

  return hst_data;
}


