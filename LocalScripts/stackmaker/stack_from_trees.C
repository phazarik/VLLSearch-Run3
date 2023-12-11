#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TH1F.h>
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

  //Merging the histograms from each samples:
  TH1F *hst_qcd = merge_and_decorate(QCD, var, name, kYellow);
  TH1F *hst_dy = merge_and_decorate(DY, var, name, kRed-7);

  //Defining the background collection:
  vector<TH1F*> bkg = {
    hst_qcd, hst_dy
  };

  //Sorting the collection and stacking:
  std::sort(bkg.begin(), bkg.end(), compareHists);
  THStack *stack = new THStack("Stacked",var+";"+var+";Events");
  for(int i=0; i<(int)bkg.size(); i++) stack->Add(bkg[i]);
  setFillColorFromLineColor(stack);
  
  //---------------------------------------------
  // Plotting:
  //---------------------------------------------
  TCanvas *canvas = create_canvas(var, 800, 600);
  TPad *mainPad  = create_mainPad(0, 0.2, 1, 1);  mainPad->Draw();
  TPad *ratioPad = create_ratioPad(0, 0, 1, 0.2); ratioPad->Draw();

  TLegend *lg = create_legend(0.87, 0.45, 0.95, 0.90);
  lg->AddEntry(hst_dy, "DY", "f");

  mainPad->cd();
  hst_dy->SetStats(0);
  hst_dy->Draw("hist");
  lg->Draw("same");

  ratioPad->cd();
  hst_qcd->SetStats(0);
  hst_qcd->Draw("hist");

  mainPad->cd();
  put_text();
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
    {.var="lep0_pt", .name="Leading lepton pT", 50, 0, 500},
    {.var="lep1_pt", .name="SubLeading lepton pT", 50, 0, 500},
  };

  for(int i=0; i<(int)p.size(); i++){
    TString var  = p[i].var;
    TString name = p[i].name;
    nbins = p[i].nbins;
    xmin  = p[i].xmin;
    xmax  = p[i].xmax;
    plot(var, name);
    break;
  }

}
