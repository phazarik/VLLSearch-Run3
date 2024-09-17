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
using namespace std;

//Global variables need to be declared here.
//Being used by settings.h:
TString input_path;
int nbins;
float xmin;
float xmax;
int rebin;
double QCDscale;
//Being used by decorations.h
float globalSbyB;
float globalObsbyExp;
float globalObsbyExpErr;
//Others:
bool toSave;
bool toLog;
bool toOverlayData;
bool toZoom;
TString tag, tag2; //Additional info while saving the plots
TString channel;

//Declaring ROOT objects here for memory management.
TH1F *dummy;
TH1F *sig1;
TH1F *sig2;
TH1F *sig3;
TH1F *hst_data;
//TH1F *hst_smuon;
//TH1F *hst_egamma;
vector<TH1F*> bkg;
THStack *bkgstack;
TCanvas *canvas;
TPad *mainPad;
TPad *ratioPad;
TH1F *sbyrb;
TH1F *ratiohist;

void reset_if_not_null(TH1F *hist){
  if(hist != nullptr)
    hist->Reset();
}

void plot(TString var, TString name);

//------------------------------------------------
// Main function where the variables are decided
//------------------------------------------------

void CalculateYields(TString _var = "", TString _name = "", int _nbins = 200, float _xmin = 0.0, float _xmax = 200, int _rebin = 2){
//void makeplotForCMS(){
  time_t start, end;
  time(&start);

  //SET GLOBAL SETTINGS HERE:
  channel = "ee";
  TString jobname = "hist_2LSS_2018UL_Aug11_SRgeneral_"+channel;
  input_path = "../input_hists/"+jobname;
  globalSbyB = 0;
  toSave = true;
  toLog = true;
  toOverlayData = false;
  toZoom = false; //forcefully zooms on the x axis.
  tag = "SRgeneral_"+channel; //Don't use special symbols (folder name)
  TString info = "S_{T}>200, N_{bj}=0";

  //DON'T TOUCH BELOW:
  if     (channel == "ee") tag2 = info+" (e-e channel)"; //This appears on the plot.
  else if(channel == "em") tag2 = info+" (e-#mu channel)";
  else if(channel == "mm") tag2 = info+" (#mu-#mu channel)";
  else DisplayText("Error! Please provide correct channel name!", 31);

  QCDscale = 1.0;/*
  //------------2024-08-09---------------//
  if     (channel == "ee") QCDscale = 0.49775447; //ee-channel
  else if(channel == "em") QCDscale = 0.27948709; //em-channel
  else if(channel == "mm") QCDscale = 0.20128545; //mm-channel
  else DisplayText("Error! Please provide correct channel name!", 31);*/
  //-------------------------------------//
  //------------2024-08-09-v2------------//
  if     (channel == "ee") QCDscale = 0.15881616; //ee-channel
  else if(channel == "em") QCDscale = 0.23838132; //em-channel
  else if(channel == "mm") QCDscale = 0.18179923; //mm-channel
  else DisplayText("Error! Please provide correct channel name!", 31);
  //-------------------------------------//
  
  struct plotdata {
    TString var;
    TString name;
    int nbins;
    float xmin;
    float xmax;
    int rebin;
  };

  vector<plotdata> p = {
    //{.var=_var, .name= _name, _nbins, _xmin, _xmax, _rebin}
    //Rebin can be overwritten inside the plot loop.
    {.var="dilep_mass", .name="M_{ll} (GeV)",  200, 0, 200, 2},
    //{.var="lep0_iso",   .name="Leading lepton reliso03",  1000, 0, 10, 5},
    //{.var="HT",       .name="HT (GeV)",       200, 0, 200, 1},
    //{.var="dilep_ptratio", .name="p_{T1} / p_{T0}", 200, 0, 1, 5}
  };

  int count = 0;
  for(int i=0; i<(int)p.size(); i++){
    TString var  = p[i].var;
    TString name = p[i].name;
    nbins = p[i].nbins;
    xmin  = p[i].xmin;
    xmax  = p[i].xmax;
    rebin = p[i].rebin;
    plot(var, name);
    count ++;
    //break;
  }
  
  //Done!
  time(&end);

  double time_taken = double(end-start);
  TString report = "\nDone!!\nTime taken : "+to_string((int)time_taken)+" second(s).\nNo of plots = "+to_string(count)+"\n";
  //DisplayText(report, 33); //33 is the ANSI color code for yellow
}

//-----------------------------------
// Plotmaker (runs for each variable)
//-----------------------------------

void plot(TString var, TString name){

  //rebin = 1; //overriding rebin
  //cout<<"Test : toZoom = "<<toZoom;
  //cout<<"\txmin, xmax = "<<xmin<<" "<<xmax<<endl;
  
  bkg.clear();
  TString date_stamp  = todays_date();
  TString dump_folder = "plots/"+date_stamp+"_"+tag;
  TString filename = dump_folder+"/"+tag+"_"+var;

  //---------------------------------------------
  // Preparing the histograms:
  //---------------------------------------------
    
  //Reading from files using: get_hist(var, sample, subsample, lumi)
  vector<TH1F *> DY = {
    get_hist(var, "DYJetsToLL", "M10to50", 5925.522),
    get_hist(var, "DYJetsToLL", "M50",    30321.155),
  };
  vector<TH1F *>ZGamma={
    get_hist(var, "ZGamma", "ZGToLLG_01J", 592588.5918)
  };
  vector<TH1F *> QCD = {       
    get_hist(var, "QCD_MuEnriched", "20to30",         23.893),
    get_hist(var, "QCD_MuEnriched", "30to50",         42.906),
    get_hist(var, "QCD_MuEnriched", "50to80",        105.880),
    get_hist(var, "QCD_MuEnriched", "80to120",       508.715),
    get_hist(var, "QCD_MuEnriched", "120to170",     1802.854),
    get_hist(var, "QCD_MuEnriched", "170to300",    10265.815),
    get_hist(var, "QCD_MuEnriched", "300to470",    95249.242),
    get_hist(var, "QCD_MuEnriched", "470to600",   656872.156),
    get_hist(var, "QCD_MuEnriched", "600to800",  2060827.812),
    get_hist(var, "QCD_MuEnriched", "800to1000",11337379.457),

    get_hist(var, "QCD_EMEnriched", "15to20",      5.96666541),
    get_hist(var, "QCD_EMEnriched", "20to30",      2.92664338), //weird low stat bin
    get_hist(var, "QCD_EMEnriched", "30to50",      1.33001225),
    get_hist(var, "QCD_EMEnriched", "50to80",      5.28031791),
    get_hist(var, "QCD_EMEnriched", "80to120",    25.76427755),
    get_hist(var, "QCD_EMEnriched", "120to170",  145.33569605),
    get_hist(var, "QCD_EMEnriched", "170to300",  223.50433213),
    get_hist(var, "QCD_EMEnriched", "300toInf", 2007.24094203),
  };
  vector<TH1F *>WJets = {
    get_hist(var, "HTbinnedWJets", "70to100",      52100.910),
    get_hist(var, "HTbinnedWJets", "100to200",     41127.174),
    get_hist(var, "HTbinnedWJets", "200to400",    172265.183),
    get_hist(var, "HTbinnedWJets", "400to600",    163821.083),
    get_hist(var, "HTbinnedWJets", "600to800",    708793.021),
    get_hist(var, "HTbinnedWJets", "800to1200",  1481985.193),
    get_hist(var, "HTbinnedWJets", "1200to2500", 5602003.457),
    get_hist(var, "HTbinnedWJets", "2500toInf", 79396214.989),
  };
  vector<TH1F *>ST = {
    get_hist(var, "SingleTop", "s-channel_LeptonDecays",            5456748.098),
    get_hist(var, "SingleTop", "t-channel_AntiTop_InclusiveDecays", 1407728.544),
    get_hist(var, "SingleTop", "t-channel_Top_InclusiveDecays",     1572627.866),
    get_hist(var, "SingleTop", "tW_AntiTop_InclusiceDecays",         238357.428),
    get_hist(var, "SingleTop", "tW_Top_InclusiveDecays",             245177.196)
  };
  vector<TH1F *>TTBar={
    get_hist(var, "TTBar", "TTTo2L2Nu",        1642541.624),
    get_hist(var, "TTBar", "TTToSemiLeptonic", 1304012.700),
  };
  vector<TH1F *>TTW ={
    get_hist(var, "TTW", "TTWToLNu", 48627268.5)
  };
  vector<TH1F *>TTZ ={
    get_hist(var, "TTZ", "TTZToLL", 107659472.141)
  };
  vector<TH1F *>WW={
    get_hist(var, "WW", "WWTo1L1Nu2Q", 787485.589),
    get_hist(var, "WW", "WWTo2L2Nu",   901172.227),
    get_hist(var, "WW", "WWTo4Q",      773049.853),
  };
  vector<TH1F *>WZ={
    get_hist(var, "WZ", "WZTo1L1Nu2Q", 805257.731),
    get_hist(var, "WZ", "WZTo2Q2L",   4499605.731),
    get_hist(var, "WZ", "WZTo3LNu",   1889798.538),
  };
  vector<TH1F *>ZZ={
    get_hist(var, "ZZ", "ZZTo2L2Nu", 58416512.631),
    get_hist(var, "ZZ", "ZZTo2Q2L",   7928149.608),
    get_hist(var, "ZZ", "ZZTo2Q2Nu",  4405016.452),
    get_hist(var, "ZZ", "ZZTo4L",    74330566.038),
  };
  vector<TH1F *>VVV={
    get_hist(var, "WWZ", "Inclusive", 1452841.24194),
    get_hist(var, "WZZ", "Inclusive", 5254860.74619),
    get_hist(var, "ZZZ", "Inclusive", 16937669.376694),
  };
  vector<TH1F *>Rare={
    get_hist(var, "Rare", "THQ",     39983860.190527),
    get_hist(var, "Rare", "THW",    101122448.979592),
    get_hist(var, "Rare", "TTHH",   745156482.861401),
    get_hist(var, "Rare", "TTTJ",  1246882793.01746),
    get_hist(var, "Rare", "TTTT",  1613891978.74181),
    get_hist(var, "Rare", "TTTW",   678385059.049712),
    get_hist(var, "Rare", "TTWH",   435201401.050788),
    get_hist(var, "Rare", "TTWW",   134857142.857143),
    get_hist(var, "Rare", "TTWZ",   202686202.686203),
    get_hist(var, "Rare", "TTZH",   440528634.361234),
    get_hist(var, "Rare", "TTZZ",   358273381.29496),
    get_hist(var, "Rare", "TZq_ll", 157598201.29612),
  };
  vector<TH1F *>SingleMuon={
    get_hist(var, "SingleMuon", "SingleMuon_A", 59800),
    get_hist(var, "SingleMuon", "SingleMuon_B", 59800),
    get_hist(var, "SingleMuon", "SingleMuon_C", 59800),
    get_hist(var, "SingleMuon", "SingleMuon_D", 59800),
  };
  vector<TH1F *>EGamma={
    get_hist(var, "EGamma", "EGamma_A", 59800),
    get_hist(var, "EGamma", "EGamma_B", 59800),
    get_hist(var, "EGamma", "EGamma_C", 59800),
    get_hist(var, "EGamma", "EGamma_D", 59800),
  };
  
  //DisplayText("Reading done.", 33);
  
  //Merging the histograms from each samples and storing in a collection:
  bkg = {
    merge_and_decorate(QCD,   "QCD",       kYellow),
    merge_and_decorate(DY,    "DY",        kRed-7),
    merge_and_decorate(ZGamma,"Z#gamma",   kRed-9),
    merge_and_decorate(WJets, "W+jets",    kGray+1),
    merge_and_decorate(ST,    "Single t",  kCyan-7),
    merge_and_decorate(TTBar, "t#bar{t}",  kAzure+1),
    merge_and_decorate(TTW,   "t#bar{t}W", kAzure+2),
    merge_and_decorate(TTZ,   "t#bar{t}Z", kAzure+3),
    merge_and_decorate(WW,    "WW",        kGreen-3),
    merge_and_decorate(WZ,    "WZ",        kGreen-9),
    merge_and_decorate(ZZ,    "ZZ",        kGreen-10),
    merge_and_decorate(VVV,   "VVV",       kGreen),
    merge_and_decorate(Rare,  "Rare",      kMagenta),
  };

  //Remove null pointers:
  std::vector<TH1F*> nulls_removed;
  for(size_t i = 0; i < bkg.size(); ++i){
    if   (bkg[i] != nullptr) nulls_removed.push_back(bkg[i]);
    else DisplayText("Remove null hist: "+(TString)bkg[i]->GetName(), 31);
  }
  bkg = nulls_removed;

  //######################################################################
  // Custom background management:
  TH1F *allbkg = (TH1F *)bkg[0]->Clone(); allbkg->Reset();
  for(int i=0; i<(int)bkg.size(); i++) allbkg->Add(bkg[i]);
  /*
  //Looping through histograms and combining bin by bin
  for (int i = 0; i < (int)bkg.size(); i++) {
    TH1F *hist = bkg[i];
    for (int bin = 1; bin <= hist->GetNbinsX(); bin++) {
      Double_t content = hist->GetBinContent(bin);
      Double_t error   = hist->GetBinError(bin);
      
      // Adding content to combined histogram
      Double_t current_content = allbkg->GetBinContent(bin);
      allbkg->SetBinContent(bin, current_content + content);
	
      // Updating error (adding squared errors)
      Double_t current_error = allbkg->GetBinError(bin);
      allbkg->SetBinError(bin, sqrt(current_error * current_error + error * error));
    }
    }*/

  //Stacking:
  std::sort(bkg.begin(), bkg.end(), compareHists);
  bkgstack = new THStack("Stacked",var+";"+var+";Events");
  for(int i=0; i<(int)bkg.size(); i++) bkgstack->Add(bkg[i]);
  //bkgstack->Add(hst_qcd);
  //SetFillColorFromLineColor(bkgstack);
  bkgstack->SetTitle("");

  //######################################################################
  //Managing data:
  TH1F *hst_smuon = merge_and_decorate(SingleMuon, "SingleMuon", kBlack);
  TH1F *hst_egamma= merge_and_decorate(EGamma,     "EGamma",     kBlack);

  hst_data = (TH1F *)hst_smuon->Clone();
  if(hst_egamma) hst_data->Add(hst_egamma);
  /*
  // If the second histogram exists, combine it bin by bin
  if (hst_egamma) {
    for (int bin = 1; bin <= hst_data->GetNbinsX(); bin++) {
      Double_t content_smuon = hst_data->GetBinContent(bin);
      Double_t error_smuon = hst_data->GetBinError(bin);

      Double_t content_egamma = hst_egamma->GetBinContent(bin);
      Double_t error_egamma = hst_egamma->GetBinError(bin);

      // Combining the bin contents
      hst_data->SetBinContent(bin, content_smuon + content_egamma);

      // Combining the bin errors in quadrature
      hst_data->SetBinError(bin, sqrt(error_smuon * error_smuon + error_egamma * error_egamma));
    }
    }*/
  hst_data->SetName("Data (2018)");

  //######################################################################
  // Managing signal:
  vector<TH1F*> sig_VLLD_ele = {
    get_hist(var, "VLLD", "ele_M100", 6560.41),
    get_hist(var, "VLLD", "ele_M200", 54213.24),
    get_hist(var, "VLLD", "ele_M300", 85061.86),
    get_hist(var, "VLLD", "ele_M400", 270022.05),
    get_hist(var, "VLLD", "ele_M600", 1651744.97),
    get_hist(var, "VLLD", "ele_M800", 6824207.49),
    get_hist(var, "VLLD", "ele_M1000", 25011328.53)
  };

  vector<TH1F*> sig_VLLD_mu = {
    get_hist(var, "VLLD", "mu_M100", 6622.84),
    get_hist(var, "VLLD", "mu_M200", 54344.12),
    get_hist(var, "VLLD", "mu_M300", 85986.25),
    get_hist(var, "VLLD", "mu_M400", 267905.18),
    get_hist(var, "VLLD", "mu_M600", 1670469.80),
    get_hist(var, "VLLD", "mu_M800", 7136311.24)
  };
  
  //----------------------------------------------------------------------------
  //ON SCREEN DISPLAYS:
  createFolder("yields");
  DisplayYieldsInBins(allbkg, allbkg);
  for(int i=0; i<(int)sig_VLLD_ele.size(); i++) DisplaySignalYieldsInBins(sig_VLLD_ele[i]);
  for(int i=0; i<(int)sig_VLLD_mu.size(); i++)  DisplaySignalYieldsInBins(sig_VLLD_mu[i]);

  DisplayText("Done!", 33);

}
