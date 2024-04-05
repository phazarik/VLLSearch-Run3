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

void makestack(){

  time_t start, end;
  time(&start);
   
  //Initializing some global variables:
  //input_path = "../trees/2023-12-13";
  TString jobname = "hist_2muSS_Apr04_baselineIso";
  input_path = "../input_files/"+jobname;
  globalSbyB = 0;
  toSave = true;
  toLog = true;
  toOverlayData = false;
  toZoom = false; //forcefully zooms on the x axis.
  tag = "2muSS_basline_Iso"; //Don't use special symbols (because this string is part of the folder name)
  tag2 = "QCD Baseline (iso mu)"; //This appears on the plot.
  //QCDscale = 1.0;
  QCDscale = 0.094647493; //0.129596389;//0.136561*0.949;

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
    //For histograms, nbins do not matter (already decided).
    //It matters if the code is reading branches.
    //Rebin can be overwritten inside the plot loop.
    //{.var="dilep_mass",      .name="Dilep mass (GeV)",  200, 0, 200, 5},
    //{.var="lep0_pt",  .name="Leading lepton pT (GeV)",    200, 0, 200, 1},
    //{.var="HT",       .name="HT (GeV)",       200, 0, 200, 1},
    //{.var="lep0_iso", .name="Leading lepton reliso03",    1000, 0, 10, 10},
    //{.var="lep1_iso", .name="SubLeading lepton reliso03", 1000, 0, 10, 10},
    //{.var="dilep_deta",      .name="deta(lep0, lep1)",  200, 0, 6,   5},
    //{.var="dphi_metlep_min", .name="min-dphi(lep, MET)",200, 0, 4, 5},
    /*
    {.var="nlep",     .name="number of leptons", 10, 0, 10, 1},
    {.var="njet",     .name="number of jets",    10, 0, 10, 1},
    {.var="nbjet",    .name="number of bjets",  10, 0, 10, 1},
    {.var="HT",       .name="HT (GeV)",       200, 0, 200, 1},
    {.var="STvis",    .name="HT+LT (GeV)",    200, 0, 200, 5},
    {.var="STfrac",   .name="LT/(HT+LT)", 200, 0, 1.2, 5},
    {.var="MET",      .name="MET (GeV)",    200, 0, 200, 2},
    {.var="MET_phi",  .name="MET phi",    200, -4, 4, 5},
    {.var="lep0_pt",  .name="Leading lepton pT (GeV)",    200, 0, 200, 2},
    {.var="lep0_eta", .name="Leading lepton eta",         200, -4, 4,  5},
    {.var="lep0_phi", .name="Leading lepton phi",         200, -4, 4,  5},
    {.var="lep0_mt",  .name="Leading lepton mT (GeV)",    200, 0, 200, 2},
    {.var="lep0_iso", .name="Leading lepton reliso03",    1000, 0, 10, 10},    
    {.var="lep1_pt",  .name="SubLeading lepton pT (GeV)", 200, 0, 200, 2},
    {.var="lep1_eta", .name="SubLeading lepton eta",      200, -4, 4,  5},
    {.var="lep1_phi", .name="SubLeading lepton phi",      200, -4, 4,  5},
    {.var="lep1_mt",  .name="SubLeading lepton mT (GeV)", 200, 0, 200, 2},
    {.var="lep1_iso", .name="SubLeading lepton reliso03", 1000, 0, 10, 10},*/
    
    {.var="ST",              .name="ST (GeV)",          200, 0, 200, 5},
    {.var="dilep_pt",        .name="Dilep pT (GeV)",    200, 0, 200, 2},
    {.var="dilep_eta",       .name="Dilep eta",         200, -4, 4,  5},
    {.var="dilep_phi",       .name="Dilep phi",         200, -4, 4,  5},
    {.var="dilep_mass",      .name="Dilep mass (GeV)",  200, 0, 200, 2},
    {.var="dilep_mt",        .name="Dilep mT (GeV)",    200, 0, 200, 2},
    {.var="dilep_deta",      .name="deta(lep0, lep1)",  200, 0, 6,   5},
    {.var="dilep_dphi",      .name="dphi(lep0, lep1)",  200, 0, 6,   5},
    {.var="dilep_dR",        .name="dR(lep0, lep1)",    200, 0, 6,   5},
    {.var="dilep_ptratio",   .name="pT1/pT0",           200, 0, 1, 5},
    {.var="dphi_metlep0",    .name="dphi(lep0, MET)",   200, 0, 4, 5},
    {.var="dphi_metlep1",    .name="dphi(lep1, MET)",   200, 0, 4, 5},
    {.var="dphi_metdilep",   .name="dphi(dilep, MET)",  200, 0, 4, 5},
    {.var="dphi_metlep_max", .name="max-dphi(lep, MET)",200, 0, 4, 5},
    {.var="dphi_metlep_min", .name="min-dphi(lep, MET)",200, 0, 4, 5},
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
  DisplayText(report, 33); //33 is the ANSI color code for yellow
}

//---------------------------------------------------------------
// Event selection: Put cuts on the branches and filter the tree
//---------------------------------------------------------------
/*
TTree* GetFilteredTree(TTree *intree){

  //Define the branch cuts using TCut
  TCut pt0cut = "lep0_pt > 35";

  TCut cut = pt0cut; 
  TTree *filtered_tree = intree->CopyTree(cut);
  return filtered_tree;
  }*/

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
  TString filename = dump_folder+"/"+var;
  //---------------------------------------------
  // Preparing the histograms:
  //---------------------------------------------
    
  //Reading from files using: get_hist(var, sample, subsample, lumi)
  vector<TH1F *> DY = {
    get_hist(var, "DYJetsToLL", "M10to50", 5925.522),
    get_hist(var, "DYJetsToLL", "M50",    30321.155),
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
    merge_and_decorate(QCD,   "QCD",   kYellow),
    merge_and_decorate(DY,    "DY",    kRed-7),
    merge_and_decorate(WJets, "WJets", kGray+1),
    merge_and_decorate(ST,    "ST",    kCyan-7),
    merge_and_decorate(TTBar, "TTBar", kAzure+1),
    merge_and_decorate(TTW,   "TTW",   kAzure+2),
    merge_and_decorate(TTZ,   "TTZ",   kAzure+3),
    merge_and_decorate(WW,    "WW",    kGreen-3),
    merge_and_decorate(WZ,    "WZ",    kGreen-9),
    merge_and_decorate(ZZ,    "ZZ",    kGreen-10),
  };

  //Remove null pointers:
  std::vector<TH1F*> nulls_removed;
  for(size_t i = 0; i < bkg.size(); ++i){
    if (bkg[i] != nullptr) nulls_removed.push_back(bkg[i]);
  }
  bkg = nulls_removed;  
  TH1F *hst_qcd = bkg[0]; //Assuming qcd isn't null

  //################
  //Managing signal:
  sig1 = get_hist(var, "VLLD", "mu_M100", 8689.91);
  sig2 = get_hist(var, "VLLS", "mu_M125", 1316553.24);
  sig3 = nullptr; //get_hist(var, "VLLS", "mu_M100",    657832.10);
  if(sig1) {SetHistoStyle(sig1, kRed+2); sig1->SetName("VLLD mu M100");}
  if(sig2) {SetHistoStyle(sig2, kRed+0); sig2->SetName("VLLS mu M125");}
  //if(sig3) {SetHistoStyle(sig3, kRed+2); sig3->SetName("VLLD mu M125");}

  //###############
  //Managing data:
  TH1F *hst_smuon = merge_and_decorate(SingleMuon, "SingleMuon", kBlack);
  TH1F *hst_egamma= merge_and_decorate(EGamma,     "EGamma",   kBlack);

  hst_data = (TH1F *)hst_smuon->Clone();
  if(hst_egamma) hst_data->Add(hst_egamma);
  hst_data->SetName("Data (2018)");

  //Sorting the background collection and stacking:
  std::sort(bkg.begin(), bkg.end(), compareHists);
  bkgstack = new THStack("Stacked",var+";"+var+";Events");
  for(int i=0; i<(int)bkg.size(); i++) bkgstack->Add(bkg[i]);
  SetFillColorFromLineColor(bkgstack);
  bkgstack->SetTitle("");
  
  //----------------------------------------------------------------------------
  //ON SCREEN DISPLAYS:
  //Caculating scale factors, since the histograms are ready:
  if(toOverlayData && var=="HT") GetBinwiseSF(var,hst_data, hst_qcd, bkg);
  //Displaying yield with error:
  if(var=="HT"){
    DisplayText("Yields with uncertainty:", 33);
    if(toOverlayData){
      cout<<fixed<<setprecision(2);
      cout<<"Data\t";
      cout<<hst_data->Integral()<<" ± "<<GetStatUncertainty(hst_data);
      cout<<fixed<<setprecision(5);
      cout<<"\t (SF= "<<hst_data->Integral()/hst_data->GetEntries()<<" )";
      cout<<defaultfloat<<endl;
    }
    Double_t sum_bkg = 0;
    Double_t sum_bkg_sqerr = 0;
    for(int i=(int)bkg.size()-1; i>=0; i--){
      cout<<fixed<<setprecision(2);
      cout<<bkg[i]->GetName()<<"\t";
      cout<<bkg[i]->Integral()<<"\\pm"<<GetStatUncertainty(bkg[i]);
      cout<<fixed<<setprecision(5);
      cout<<"\t (SF= "<<bkg[i]->Integral()/bkg[i]->GetEntries()<<" )";
      cout<<defaultfloat<<endl;
      sum_bkg += bkg[i]->Integral();
      sum_bkg_sqerr += pow(GetStatUncertainty(bkg[i]), 2);
    }
    cout<<fixed<<setprecision(2);
    cout<<"Total background = "<<sum_bkg<<" ± "<<sqrt(sum_bkg_sqerr)<<"\n";
    if(sig1) cout<<"Signal = "<<sig1->Integral()<<" ± "<<GetStatUncertainty(sig1)<<endl;
    if(sig2) cout<<"Signal = "<<sig2->Integral()<<" ± "<<GetStatUncertainty(sig2)<<endl;
    if(sig3) cout<<"Signal = "<<sig2->Integral()<<" ± "<<GetStatUncertainty(sig3)<<endl;
    cout<<defaultfloat<<endl;
  } 
  //-----------------------------------------------------------------------------
  

  //---------------------------------------------
  // Plotting:
  //---------------------------------------------
  canvas = create_canvas(var, filename, 800, 600);
  mainPad  = create_mainPad(0, 0.22, 1, 1);     mainPad ->Draw();
  ratioPad = create_ratioPad(0, 0.01, 1, 0.25); ratioPad->Draw();
  
  mainPad->cd();

  //Setting up a dummy hist to enforce certain things in the plot.
  //It should alywas be drawn first.
  //dummy->Reset();
  dummy = (TH1F *)bkg[0]->Clone(); dummy->Reset(); 
  dummy->GetYaxis()->SetTitle("Events");
  dummy->GetYaxis()->SetRangeUser(0.1, 10E6);
  if(toZoom) dummy->GetXaxis()->SetRangeUser(xmin, xmax);
  dummy->SetStats(0);
  dummy->Draw("hist");

  //Now draw the rest.
  if(toOverlayData) hst_data->Draw("ep same");
  bkgstack->Draw("hist same");
  if(sig1) sig1->Draw("HIST same");
  if(sig2) sig2->Draw("HIST same");
  if(sig3) sig3->Draw("HIST same");
  if(toOverlayData) hst_data->Draw("ep same");

  ratioPad->cd();

  //SoverB
  globalSbyB = 0;
  sbyrb = nullptr;
  if     (sig1) sbyrb = GetSbyRootB(sig1, bkg);
  else if(sig2) sbyrb = GetSbyRootB(sig2, bkg);
  else if(sig3) sbyrb = GetSbyRootB(sig3, bkg);
  if(sbyrb){
    SetRatioStyle(sbyrb, name);
    sbyrb->GetYaxis()->SetTitle("S/sqrtB");
    if(toZoom) sbyrb->GetXaxis()->SetRangeUser(xmin, xmax);
    if(!toOverlayData) sbyrb->Draw("ep");
  }
  
  //obs/exp
  globalObsbyExp =0;
  if(toOverlayData){
    ratiohist = GetRatio(hst_data, bkg);
    SetRatioStyle(ratiohist, name);
    ratiohist->GetYaxis()->SetTitle("obs/exp");
    ratiohist->GetYaxis()->SetRangeUser(0, 2.2);
    if(toZoom) ratiohist->GetXaxis()->SetRangeUser(xmin, xmax);
  
    //Setting up a horizontal line on the ratiopad:
    float xlow  = ratiohist->GetXaxis()->GetBinLowEdge(1);
    float xhigh = ratiohist->GetXaxis()->GetBinUpEdge(ratiohist->GetNbinsX());
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
    ratiohist->Draw("ep"); //Inheriting the settings from the ratio hist.
    err->Draw("SAME P E2");
    line->Draw("same");
    ratiohist->Draw("ep same"); //I want the ratio to be on top.
  }
  
  mainPad->cd();
  
  put_text("CMS", 0.10, 0.93, 62, 0.06);
  put_text("preliminary", 0.18, 0.93, 52, 0.05);
  put_text(tag2+"", 0.35, 0.93, 42, 0.04);
  put_latex_text("(2018) 59.8 fb^{-1}", 0.61, 0.93, 42, 0.04);

  TLegend *lg = create_legend(0.76, 0.30, 0.95, 0.90);
  if(toOverlayData){
    SetLegendEntry(lg, hst_data);
    if(hst_smuon)  SetLegendEntry(lg, hst_smuon);
    if(hst_egamma) SetLegendEntry(lg, hst_egamma);
  }

  for(int i=(int)bkg.size()-1; i>=0; i--) SetLegendEntry(lg, bkg[i]);
  if(sig1) SetLegendEntry(lg, sig1);
  if(sig2) SetLegendEntry(lg, sig2);
  if(sig3) SetLegendEntry(lg, sig3);
  TString legendheader = ("Global significance = " + to_string(globalSbyB)).c_str();
  if(toOverlayData){
    legendheader = ("Global obs/exp = " + to_string(globalObsbyExp)).c_str();
    cout<<defaultfloat<<"Obs/Exp = "<<globalObsbyExp<<" ± "<<globalObsbyExpErr<<"\n"<<endl;
  }
  else cout<<legendheader<<"\n"<<endl; 
  lg->SetHeader(legendheader);
  lg->Draw();

  DisplayText("Made plot for "+var, 0);

  //Make a new folder and put all the plots there:
  if(toSave){
    createFolder(dump_folder);
    canvas->SaveAs(filename+".png");
    //canvas->Clear();
    //delete canvas;
    //delete lg;
    //delete bkgstack;
  }
}
