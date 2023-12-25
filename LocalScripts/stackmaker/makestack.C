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
#include "settings.h"
#include "decorations.h"
using namespace std;

//Global variables need to be declared here.
//Being used by settings.h:
TString input_path;
int nbins;
float xmin;
float xmax;
int rebin;
//Being used by decorations.h
float globalSbyB;
//Others:
bool toSave;
bool toLog;
bool toOverlayData;
TString tag; //Additional info while saving the plots

void plot(TString var, TString name);

//------------------------------------------------
// Main function where the variables are decided
//------------------------------------------------

void makestack(){

  time_t start, end;
  time(&start);
   
  //Initializing some global variables:
  //input_path = "../trees/2023-12-13";
  input_path = "../input_files";
  globalSbyB = 0;
  toSave = false;
  toLog = true;
  toOverlayData = false;
  tag = "";

  struct plotdata {
    TString var;
    TString name;
    int nbins;
    float xmin;
    float xmax;
    int rebin;
  };

  vector<plotdata> p = {
    //Parameters : branch name, plot name, nbins, xmin, xmax, rebin
    {.var="lep0_pt", .name="Leading lepton pT (GeV)",    200, 0, 200, 20},
    {.var="lep1_pt", .name="SubLeading lepton pT (GeV)", 200, 0, 200, 20},
  };

  for(int i=0; i<(int)p.size(); i++){
    TString var  = p[i].var;
    TString name = p[i].name;
    nbins = p[i].nbins;
    xmin  = p[i].xmin;
    xmax  = p[i].xmax;
    rebin = p[i].rebin;
    plot(var, name);
    break;
  }

  //Done!
  time(&end);

  double time_taken = double(end-start);
  TString report = "\nDone!!\nTime taken : "+to_string(time_taken)+" seconds.\n";
  DisplayText(report, 33); //33 is the ANSI color code for yellow

}

//---------------------------------------------------------------
// Event selection: Put cuts on the branches and filter the tree
//---------------------------------------------------------------

TTree* GetFilteredTree(TTree *intree){

  //Define the branch cuts using TCut
  TCut pt0cut = "lep0_pt > 35";

  TCut cut = pt0cut; 
  TTree *filtered_tree = intree->CopyTree(cut);
  return filtered_tree;
}

//-----------------------------------
// Plotmaker (runs for each variable)
//-----------------------------------

void plot(TString var, TString name){

  //---------------------------------------------
  // Preparing the histograms:
  //---------------------------------------------
    
  //Reading from files using: get_hist(var, sample, subsample, lumi)
  vector<TH1F *> DY = {
    get_hist(var, "DYJetsToLL", "M10to50", 5925.522),
    get_hist(var, "DYJetsToLL", "M50",    30321.155),
  };
  vector<TH1F *> QCD = {
    get_hist(var, "QCD_MuEnriched", "20to30",    23.893),
    get_hist(var, "QCD_MuEnriched", "30to50",    42.906),
    get_hist(var, "QCD_MuEnriched", "50to80",    105.880),
    get_hist(var, "QCD_MuEnriched", "80to120",   508.715),
    get_hist(var, "QCD_MuEnriched", "120to170",  1802.854),
    get_hist(var, "QCD_MuEnriched", "170to300",  10265.815),
    get_hist(var, "QCD_MuEnriched", "300to470",  95249.242),
    get_hist(var, "QCD_MuEnriched", "470to600",  656872.156),
    get_hist(var, "QCD_MuEnriched", "600to800",  2060827.812),
    get_hist(var, "QCD_MuEnriched", "800to1000", 11337379.457),

    get_hist(var, "QCD_EMEnriched", "15to20", 5.96666541),
  };
  vector<TH1F *>WJets = {
    get_hist(var, "HTbinnedWJets", "70to100",    52100.910),
    get_hist(var, "HTbinnedWJets", "100to200",   41127.174),
    get_hist(var, "HTbinnedWJets", "200to400",   172265.183),
    get_hist(var, "HTbinnedWJets", "400to600",   163821.083),
    get_hist(var, "HTbinnedWJets", "600to800",   708793.021),
    get_hist(var, "HTbinnedWJets", "800to1200",  1481985.193),
    get_hist(var, "HTbinnedWJets", "1200to2500", 5602003.457),
    get_hist(var, "HTbinnedWJets", "2500toInf",  79396214.989),
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
  
  //DisplayText("Reading done.", 33);

  TH1F *sig_eles_100 = get_hist(var, "VLLS", "ele_M100", 663355.82);
  TH1F *sig_eled_100 = get_hist(var, "VLLD", "ele_M100", 8608.00);
  
  //Merging the histograms from each samples:
  TH1F *hst_qcd   = merge_and_decorate(QCD,   "QCD",       kYellow);
  TH1F *hst_dy    = merge_and_decorate(DY,    "Drell-Yan", kRed-7);
  TH1F *hst_wjets = merge_and_decorate(WJets, "WJets",     kGray+1);
  TH1F *hst_st    = merge_and_decorate(ST,    "SingleTop", kCyan-7);
  TH1F *hst_ttbar = merge_and_decorate(TTBar, "TTBar",     kAzure+1);
  TH1F *hst_ttw   = merge_and_decorate(TTW,   "TTW",       kAzure+2);
  TH1F *hst_ww    = merge_and_decorate(WW,    "WW",        kGreen-3);
  TH1F *hst_wz    = merge_and_decorate(WZ,    "WZ",        kGreen-9);
  TH1F *hst_zz    = merge_and_decorate(ZZ,    "ZZ",        kGreen-10);

  SetHistoStyle(sig_eles_100, kRed);  sig_eles_100->SetName("VLLS ele M100");
  SetHistoStyle(sig_eled_100, kBlue); sig_eled_100->SetName("VLLD ele M100");

  //Defining the background collection:
  vector<TH1F*> bkg = {hst_qcd, hst_dy, hst_wjets, hst_st, hst_ttbar, hst_ttw, hst_ww, hst_wz, hst_zz};

  //Sorting the collection and stacking:
  std::sort(bkg.begin(), bkg.end(), compareHists);
  THStack *stack = new THStack("Stacked",var+";"+var+";Events");
  for(int i=0; i<(int)bkg.size(); i++) stack->Add(bkg[i]);
  SetFillColorFromLineColor(stack);
  stack->SetTitle("");
  
  //---------------------------------------------
  // Plotting:
  //---------------------------------------------
  TCanvas *canvas = create_canvas(var, 800, 600);
  TPad *mainPad  = create_mainPad(0, 0.22, 1, 1);     mainPad ->Draw();
  TPad *ratioPad = create_ratioPad(0, 0.01, 1, 0.25); ratioPad->Draw();
  
  mainPad->cd();

  //Setting up a dummy hist to enforce certain things in the plot.
  //It should alywas be drawn first.
  TH1F *dummy = (TH1F *)bkg[0]->Clone(); dummy->Reset();
  dummy->GetYaxis()->SetTitle("Events");
  dummy->GetYaxis()->SetRangeUser(1, 10E6);
  dummy->SetStats(0);
  dummy->Draw("hist");

  //Now draw the rest.
  stack->Draw("hist same");
  sig_eled_100->Draw("hist same");
  sig_eles_100->Draw("hist same");
  

  ratioPad->cd();
  TH1F *ratio = GetSbyRootB(sig_eles_100, bkg); //This also calculates globalSbyB
  SetRatioStyle(ratio, name);
  ratio->Draw("ep");

  mainPad->cd();
  put_text();

  TLegend *lg = create_legend(0.76, 0.50, 0.95, 0.90);
  for(int i=(int)bkg.size()-1; i>=0; i--) SetLegendEntry(lg, bkg[i]);
  SetLegendEntry(lg, sig_eled_100);
  SetLegendEntry(lg, sig_eles_100);
  lg->SetHeader(("Global S/sqrt{B} = " + to_string(globalSbyB)).c_str());
  lg->Draw();

  //Make a new folder and put all the plots there:
  TString date_stamp  = todays_date();
  TString dump_folder = "plots/"+date_stamp+tag;
  if(toSave){
    createFolder(dump_folder);
    canvas->SaveAs(dump_folder+"/"+var+".png");
    canvas->Clear();
    delete canvas;
    delete lg;
    delete sig_eled_100;
    delete sig_eles_100;
    delete stack;
    delete ratio;
    for(auto p : bkg){delete p;} bkg.clear();
  }
}
