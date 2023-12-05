#include <iostream>
#include <vector>
#include <string>
#include "TFile.h"
#include "TH1.h"
#include "TString.h"
#include "functions.h"
using namespace std;
void plotmaker(TString plotname, TString xtitle, int nbins, TString target_model, TString target_flavor);

void make_overlay(){
  TString target_model = "VLLS";
  TString target_flavor = "ele";

  struct plots{
    TString name;
    int nbins=1;
    TString xtitle="xtitle";
  };

  vector<plots> plot = {
    //{.name = "allvlnu_charge",  .nbins = 1},
    //{.name = "allvll_decay",    .nbins = 1},
    {.name = "nvll",              .nbins = 1,  .xtitle="nLep"},
    {.name = "allvll_pt",         .nbins = 20, .xtitle="Lep pT (GeV)"},
    {.name = "allvll_eta",        .nbins = 5,  .xtitle="Lep eta"},
    {.name = "allvll_phi",        .nbins = 5,  .xtitle="Lep phi"},
    {.name = "allvll_mass",       .nbins = 1,  .xtitle="Lep mass (GeV)"},
    {.name = "allvll_charge",     .nbins = 1,  .xtitle="Lep charge"},
    {.name = "allvll_decay",      .nbins = 1,  .xtitle="Lep decaymodes (W, Z, H)"},
    {.name = "allvll_daughters",  .nbins = 1,  .xtitle="Lep daughters (pdgId)"},
    {.name = "nvlnu",             .nbins = 1,  .xtitle="nNu"},
    {.name = "allvlnu_pt",        .nbins = 20, .xtitle="Nu pT (GeV)"},
    {.name = "allvlnu_eta",       .nbins = 5,  .xtitle="Nu eta"},
    {.name = "allvlnu_phi",       .nbins = 5,  .xtitle="Nu phi"},
    {.name = "allvlnu_mass",      .nbins = 1,  .xtitle="Nu mass (GeV)"},
    {.name = "allvlnu_charge",    .nbins = 1,  .xtitle="Nu charge"},
    {.name = "allvlnu_decay",     .nbins = 1,  .xtitle="Nu decaymodes (W, Z, H)"},
    {.name = "allvlnu_daughters", .nbins = 1,  .xtitle="Nu daughters (pdgId)"},
    {.name = "finalstates_for_LL",.nbins = 1,  .xtitle="LL finalstates"},
    {.name = "finalstates_for_LN",.nbins = 1,  .xtitle="LN finalstates"},
    {.name = "finalstates_for_NN",.nbins = 1,  .xtitle="NN finalstates"}
  };
  
  for(int i=0; i<(int)plot.size(); i++){
    plotmaker(plot[i].name, plot[i].xtitle, plot[i].nbins, target_model, target_flavor);
    //break;
  }
}

void plotmaker(TString plotname, TString xtitle, int nbins, TString target_model, TString target_flavor){
  
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

      SetHistoStyle(hists[i].hist, xtitle, plottitle, hists[i].color);
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
    TCanvas *c1 = create_canvas(plotname);
    
    hists[tallest].hist->SetStats(0);
    hists[tallest].hist->Draw("hist");
    for(int i=0; i<(int)hists.size(); i++){
      if(hists[i].model == target_model && hists[i].flavor == target_flavor){
	hists[i].hist->Draw("hist same");
      }
    }
    lg1->Draw("same");
    put_text(target_model, target_flavor);

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
