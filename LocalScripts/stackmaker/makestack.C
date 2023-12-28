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
float globalObsbyExp;
//Others:
bool toSave;
bool toLog;
bool toOverlayData;
bool toZoom;
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
  toSave = true;
  toLog = true;
  toOverlayData = false;
  toZoom = false; //forcefully zooms on the x axis.
  tag = "_signf_DY";

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
    //{.var="lep0_pt",  .name="Leading lepton pT (GeV)",    200, 0, 200, 10},
    
    {.var="nlep", .name="number of leptons", 10, 0, 10, 1},
    {.var="njet", .name="number of jets",    10, 0, 10, 1},
    {.var="nbjet", .name="number of bjets",  10, 0, 10, 1},
    {.var="lep0_pt",  .name="Leading lepton pT (GeV)",    200, 0, 200, 10},
    {.var="lep0_eta", .name="Leading lepton eta",         200, -4, 4,  10},
    {.var="lep0_phi", .name="Leading lepton phi",         200, -4, 4,  10},
    {.var="lep0_mt",  .name="Leading lepton mT (GeV)",    200, 0, 200, 10},
    {.var="lep0_iso",  .name="Leading lepton reliso03",    1000, 0, 10, 20},    
    {.var="lep1_pt",  .name="SubLeading lepton pT (GeV)", 200, 0, 200, 10},
    {.var="lep1_eta", .name="SubLeading lepton eta",      200, -4, 4,  10},
    {.var="lep1_phi", .name="SubLeading lepton phi",      200, -4, 4,  10},
    {.var="lep1_mt",  .name="SubLeading lepton mT (GeV)", 200, 0, 200, 10},
    {.var="lep1_iso",  .name="SubLeading lepton reliso03",    1000, 0, 10, 20},
    {.var="dilep_pt",  .name="Dilep pT (GeV)",    200, 0, 200, 10},
    {.var="dilep_eta", .name="Dilep eta",         200, -4, 4,  10},
    {.var="dilep_phi", .name="Dilep phi",         200, -4, 4,  10},
    {.var="dilep_mass",.name="Dilep mass (GeV)",  200, 0, 200, 10},
    {.var="dilep_mt",  .name="Dilep mT (GeV)",    200, 0, 200, 10},
    {.var="dilep_deta", .name="deta(lep0, lep1)", 200, 0, 6,   10},
    {.var="dilep_dphi", .name="dphi(lep0, lep1)", 200, 0, 6,   10},
    {.var="dilep_dR",   .name="dR(lep0, lep1)",   200, 0, 6,   10},
    {.var="dilep_ptratio",.name="pT1/pT0",        200, 0, 1, 10},
    /*
    {.var="HT", .name="HT (GeV)",       200, 0, 200, 10},
    {.var="ST", .name="HT+LT (GeV)",    200, 0, 200, 10},
    {.var="STfrac", .name="LT/(HT+LT)", 200, 0, 1.2, 10},
    {.var="dphi_metlep0",    .name="dphi(lep0, MET)",    200, 0, 4, 10},
    {.var="dphi_metlep1",    .name="dphi(lep1, MET)",    200, 0, 4, 10},
    {.var="dphi_metdilep",   .name="dphi(dilep, MET)",   200, 0, 4, 10},
    {.var="dphi_metlep_max", .name="max-dphi(lep, MET)", 200, 0, 4, 10},
    {.var="dphi_metlep_min", .name="min-dphi(lep, MET)", 200, 0, 4, 10},*/
  };

  for(int i=0; i<(int)p.size(); i++){
    TString var  = p[i].var;
    TString name = p[i].name;
    nbins = p[i].nbins;
    xmin  = p[i].xmin;
    xmax  = p[i].xmax;
    rebin = p[i].rebin;
    plot(var, name);
    //break;
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

    //get_hist(var, "QCD_EMEnriched", "15to20", 5.96666541),
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
  vector<TH1F *>Data={
    get_hist(var, "SingleMuon", "SingleMuon_A", 59800),
    get_hist(var, "SingleMuon", "SingleMuon_B", 59800),
    get_hist(var, "SingleMuon", "SingleMuon_C", 59800),
    get_hist(var, "SingleMuon", "SingleMuon_D", 59800),
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
  TH1F *hst_ttz   = merge_and_decorate(TTZ,   "TTZ",       kAzure+3);
  TH1F *hst_ww    = merge_and_decorate(WW,    "WW",        kGreen-3);
  TH1F *hst_wz    = merge_and_decorate(WZ,    "WZ",        kGreen-9);
  TH1F *hst_zz    = merge_and_decorate(ZZ,    "ZZ",        kGreen-10);
  TH1F *hst_data  = merge_and_decorate(Data,  "Data",      kBlack);

  if(sig_eles_100) {SetHistoStyle(sig_eles_100, kRed);  sig_eles_100->SetName("VLLS ele M100");}
  if(sig_eled_100) {SetHistoStyle(sig_eled_100, kBlue); sig_eled_100->SetName("VLLD ele M100");}

  //Defining the background collection:
  vector<TH1F*> bkg = {hst_qcd, hst_dy, hst_wjets, hst_st, hst_ttbar, hst_ttw, hst_ttz, hst_ww, hst_wz, hst_zz};
  //vector<TH1F*> bkg = {hst_qcd, hst_dy, hst_wjets, hst_st, hst_ttbar, hst_wz, hst_zz};

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
  dummy->GetYaxis()->SetRangeUser(0.1, 10E6);
  if(toZoom) dummy->GetXaxis()->SetRangeUser(xmin, xmax);
  dummy->SetStats(0);
  dummy->Draw("hist");

  //Now draw the rest.
  if(toOverlayData) hst_data->Draw("ep same");
  stack->Draw("hist same");
  if(sig_eled_100) sig_eled_100->Draw("HIST same");
  if(sig_eles_100) sig_eles_100->Draw("HIST same");
  if(toOverlayData) hst_data->Draw("ep same");

  ratioPad->cd();

  //SoverB
  globalSbyB = 0;
  if(sig_eles_100){
    //TH1F *sbyrb = GetSbyRootB(sig_eles_100, bkg); SetRatioStyle(sbyrb, name);
    TH1F *sbyrb = GetSbyRootB(hst_dy, bkg); SetRatioStyle(sbyrb, name);
    sbyrb->GetYaxis()->SetTitle("DY/sqrtB");
    if(toZoom) sbyrb->GetXaxis()->SetRangeUser(xmin, xmax);
    if(!toOverlayData) sbyrb->Draw("ep");
  }
  
  //obs/exp
  globalObsbyExp =0;
  if(toOverlayData){
    TH1F *ratio = GetRatio(hst_data, bkg);
    SetRatioStyle(ratio, name);
    ratio->GetYaxis()->SetTitle("obs/exp");
    ratio->GetYaxis()->SetRangeUser(0, 2.2);
    if(toZoom) ratio->GetXaxis()->SetRangeUser(xmin, xmax);
  
    //Setting up a horizontal line on the ratiopad:
    float xlow  = ratio->GetXaxis()->GetBinLowEdge(1);
    float xhigh = ratio->GetXaxis()->GetBinUpEdge(ratio->GetNbinsX());
    TLine *line = new TLine(xlow, 1, xhigh, 1);
    line->SetLineColor(kRed);
    line->SetLineWidth(2);
    
    //Calculating the uncertainty on the background in each bin:
    TH1F *allbkg = (TH1F *)bkg[0]->Clone(); allbkg->Reset();
    for(int i=0; i<(int)bkg.size(); i++) allbkg->Add(bkg[i]);
    TGraphErrors *err = GetUncertainty(allbkg);
    err->GetYaxis()->SetNdivisions(5, kTRUE);
    err->SetStats(0);
    if(toZoom) err->GetXaxis()->SetRangeUser(xmin, xmax);

    //Drawing everything in the proper order:
    ratio->Draw("ep"); //Inheriting the settings from the ratio hist.
    err->Draw("SAME P E2");
    line->Draw("same");
    ratio->Draw("ep same"); //I want the ratio to be on top.
  }
  
  mainPad->cd();
  put_text();

  TLegend *lg = create_legend(0.76, 0.30, 0.95, 0.90);
  if(toOverlayData) SetLegendEntry(lg, hst_data);
  for(int i=(int)bkg.size()-1; i>=0; i--) SetLegendEntry(lg, bkg[i]);
  if(sig_eled_100) SetLegendEntry(lg, sig_eled_100);
  if(sig_eles_100) SetLegendEntry(lg, sig_eles_100);
  TString legendheader = ("Global significance = " + to_string(globalSbyB)).c_str();
  if(toOverlayData) legendheader = ("Global obs/exp = " + to_string(globalObsbyExp)).c_str();
  lg->SetHeader(legendheader);
  lg->Draw();

  DisplayText("Made plot for "+var, 0);

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
    //delete ratio;
    for(auto p : bkg){delete p;} bkg.clear();
  }
}
