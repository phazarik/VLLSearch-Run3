#include <iostream>
#include <vector>
#include <string>
#include "TFile.h"
#include "TH1.h"
#include "TString.h"
#include "functions.h"
using namespace std;
void plotmaker(TString plotname, int nbins, TString target_model, TString target_flavor);

void make_overlay(){
  TString target_model = "VLLD";
  TString target_flavor = "mu";

  struct plots{
    TString name;
    int nbins;   
  };

  vector<plots> plot = {
    //{.name = "allvlnu_charge",  .nbins = 1},
    //{.name = "allvll_decay",    .nbins = 1},
    {.name = "nvll",            .nbins = 1},
    {.name = "allvll_pt",       .nbins = 20},
    {.name = "allvll_eta",      .nbins = 5},
    {.name = "allvll_phi",      .nbins = 5},
    {.name = "allvll_mass",     .nbins = 1},
    {.name = "allvll_charge",   .nbins = 1},
    {.name = "allvll_decay",    .nbins = 1},
    {.name = "nvlnu",           .nbins = 1},
    {.name = "allvlnu_pt",      .nbins = 20},
    {.name = "allvlnu_eta",     .nbins = 5},
    {.name = "allvlnu_phi",     .nbins = 5},
    {.name = "allvlnu_mass",    .nbins = 1},
    {.name = "allvlnu_charge",  .nbins = 1},
    {.name = "allvlnu_decay",   .nbins = 1},
  };
  
  for(int i=0; i<(int)plot.size(); i++){
    plotmaker(plot[i].name, plot[i].nbins, target_model, target_flavor);
    //break;
  }
}

void plotmaker(TString plotname, int nbins, TString target_model, TString target_flavor){
  
  //Global settings:
  bool toSave = true;
  TString plottitle = target_model+" "+target_flavor;
  //nbins = 1;

  vector<database> hists = read_plots(inputs, plotname);
  TLegend *lg1 = create_legend();   
  TString legendheader = target_model+" "+target_flavor;
  
  //Decoration, Normalization and finding the tallest peak:
  float peak = -1;
  int tallest = -1;
  for(int i=0; i<(int)hists.size(); i++){
    bool null = (hists[i].hist == nullptr);
    if(hists[i].model == target_model && hists[i].flavor == target_flavor && !null){

      SetHistoStyle(hists[i].hist, plotname, plottitle, hists[i].color);
      hists[i].hist->Scale(1/hists[i].hist->Integral());
      hists[i].hist->Rebin(nbins);
      TString name = "  M"+hists[i].mass;
      lg1->AddEntry(hists[i].hist, name, "f");
      
      for(int bin=1; bin<=(int)hists[i].hist->GetNbinsX(); bin++){
	float yvalue = hists[i].hist->GetBinContent(bin);
	if(yvalue > peak){
	  peak = yvalue;
	  tallest = i;
	}
      }
    }
  }
  //cout<<"Tallest histogram is the hist no. "<<tallest<<" with a peak of "<<peak<<endl;

  if(tallest != -1){ //Flag for null hists

    //Plotting:
    TCanvas *c1 = new TCanvas(plotname,plotname,700,600);
    hists[tallest].hist->SetStats(0);
    hists[tallest].hist->Draw("hist");
    for(int i=0; i<(int)hists.size(); i++){
      if(hists[i].model == target_model && hists[i].flavor == target_flavor){
	hists[i].hist->Draw("hist same");
      }
    }
    lg1->Draw("same");

    //Saving:
    if(toSave && tallest != -1){
      plotname = target_model+"_"+target_flavor+"_"+plotname;
      TString date_stamp = todays_date();
      TString dump_folder = "plots/"+date_stamp+"/"+target_model+"_"+target_flavor;
      cout<<dump_folder<<endl;
      createFolder(dump_folder);
      c1->SaveAs(dump_folder+"/"+plotname+".png");

      delete c1;
      delete lg1;
      for(int i=0; i<(int)hists.size(); i++){
	if(hists[i].model == target_model && hists[i].flavor == target_flavor){
	  delete hists[i].hist;
	}
      }
    }
  }

  else cout<<"Null histogram for "<<plotname<<endl;
  
  
}
