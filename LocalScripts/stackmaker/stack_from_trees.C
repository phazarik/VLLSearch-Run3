#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1F.h"
#include "THStack.h"
#include "TCanvas.h"
#include "settings.h"
#include "decorations.h"
using namespace std;

//Global variables need to be declared here.
TString input_path;
int nbins;
float xmin;
float xmax;

void plot(TString var, TString name){
  
  //---------------------------------------------
  // Preparing the histograms:
  //---------------------------------------------
    
  //Reading from files using: get_hist(var, sample, subsample, lumi)
  vector<TH1F *> QCD = {
    get_hist(var, "QCD_MuEnriched", "20to30", 23.893),
  };
  vector<TH1F *> DY = {
    get_hist(var, "DYJetsToLL", "M10to50", 5925.522),
    get_hist(var, "DYJetsToLL", "M50",    30321.155)
  };
  TH1F *sig_eles_100 = get_hist(var, "VLLS", "ele_M100", 663355.82);
  TH1F *sig_eled_100 = get_hist(var, "VLLD", "ele_M100", 8608.00);
  
  //Merging the histograms from each samples:
  TH1F *hst_qcd = merge_and_decorate(QCD,  "QCD",       kYellow);
  TH1F *hst_dy  = merge_and_decorate(DY,   "Drell-Yan", kRed-7);

  SetHistoStyle(sig_eles_100, kRed);  sig_eles_100->SetName("VLLS ele M100");
  SetHistoStyle(sig_eled_100, kBlue); sig_eled_100->SetName("VLLD ele M100");

  //Defining the background collection:
  vector<TH1F*> bkg = {hst_qcd, hst_dy};

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
  TH1F *ratio = GetSbyRootB(sig_eles_100, bkg);
  SetRatioStyle(ratio, name);
  ratio->Draw("hist");

  mainPad->cd();
  put_text();

  TLegend *lg = create_legend(0.76, 0.50, 0.95, 0.90);
  for(int i=(int)bkg.size()-1; i>=0; i--) SetLegendEntry(lg, bkg[i]);
  SetLegendEntry(lg, sig_eled_100);
  SetLegendEntry(lg, sig_eles_100);
  lg->Draw();
  
  canvas->SetLogy();
}

//Main function:
void stack_from_trees(){

  input_path = "../trees/2023-12-06";

  struct plotdata {
    TString var;
    TString name;
    int nbins;
    float xmin;
    float xmax;
  };

  vector<plotdata> p = {
    {.var="lep0_pt", .name="Leading lepton pT (GeV)",    50, 0, 200},
    {.var="lep1_pt", .name="SubLeading lepton pT (GeV)", 50, 0, 200},
  };

  for(int i=0; i<(int)p.size(); i++){
    TString var  = p[i].var;
    TString name = p[i].name;
    nbins = p[i].nbins;
    xmin  = p[i].xmin;
    xmax  = p[i].xmax;
    plot(var, name);
    //break;
  }

}
