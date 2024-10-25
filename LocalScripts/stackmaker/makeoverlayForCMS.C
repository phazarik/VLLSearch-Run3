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
TString tag, tag2, tag3; //Additional info while saving the plots
TString channel;

void reset_if_not_null(TH1F *hist){
  if(hist != nullptr)
    hist->Reset();
}

vector<TH1F*> return_hist(TString var);

//---------------
// Overlay maker 
//---------------

void makeoverlayForCMS(){

  time_t start, end;
  time(&start);

  //SET GLOBAL SETTINGS HERE:
  channel = "ee";
  //TString jobname = "hist_2LSS_dyEnhanced_chwt_Oct24_"+channel;
  TString jobname = "hist_eeZwindow_2018UL_Sept24_"+channel;
  input_path = "../input_hists/"+jobname;
  globalSbyB = 0;
  toSave = true;
  toLog = true;
  toOverlayData = true;
  toZoom = false; //forcefully zooms on the x axis.
  tag = "DY"+channel; //Don't use special symbols (folder name)
  tag2 = "76 < M_{ee} < 106";
  tag3 = "p_{T1}/p_{T0} > 0.70";
  
  QCDscale = 1.0;
  TOPscale = 1.0;
  DYscale  = 1.0;
  rebin = 1;

  //----------------2024-10-03----------------//
  if(channel == "mm")      QCDscale = 0.155758;
  else if(channel == "me") QCDscale = 0.094731;
  else if(channel == "em") QCDscale = 0.192381;
  else if(channel == "ee") QCDscale = 0.303599;

  //CUSTOMIZE THE FOLLOWING ACCORDING TO YOUR NEEDS:
  //Extract histograms:
  
  vector<TH1F*> num = return_hist("chargeflip_num");
  vector<TH1F*> den = return_hist("chargeflip_den");

  TH1F *num_data, *num_dy, *num_others;
  for(int i=0; i<(int)num.size(); i++){
    if(i==0){
      num_others = (TH1F *)num[i]->Clone();
      num_others->Reset();
    }
    if(TString(num[i]->GetName())=="DY")        num_dy   = num[i];
    else if(TString(num[i]->GetName())=="Data") num_data = num[i];
    else num_others->Add(num[i]);
  }

  TH1F *den_data, *den_dy, *den_others;
  for(int i=0; i<(int)den.size(); i++){
    if(i==0){
      den_others = (TH1F *)den[i]->Clone();
      den_others->Reset();
    }
    if(TString(den[i]->GetName())=="DY")        den_dy   = den[i];
    else if(TString(den[i]->GetName())=="Data") den_data = den[i];
    else den_others->Add(den[i]);
  }

  //Corrected numerator and denominators after bacground substraction:
  TH1F *num_data_sub = (TH1F*)num_data->Clone();
  num_data_sub->Add(num_others, -1);
  TH1F *den_data_sub = (TH1F*)den_data->Clone();
  den_data_sub->Add(den_others, -1); 
  
  //CHarge misID rates:
  TH1F *chMisID_dy;
  chMisID_dy = (TH1F*)num_dy->Clone();
  chMisID_dy->Divide(den_dy);
  chMisID_dy->SetLineColor(kBlue);
  chMisID_dy->SetMarkerColor(kBlue);
  TH1F *chMisID_data;
  chMisID_data = (TH1F*)num_data_sub->Clone();
  chMisID_data->Divide(den_data_sub);
  TH1F *scalefactor;
  scalefactor = (TH1F*)chMisID_data->Clone();
  scalefactor->Divide(chMisID_dy);
  
  //Print out information:
  int nbins = den_data->GetNbinsX();
  cout<<"\nbin \tmisID (data) \tErr (data) \tmisId (MC) \tErr (mc) \tScalefactor \tErr (sf)"<<endl;
  cout<<fixed<<setprecision(6);
  for(int bin=1; bin<=nbins+1; bin++){
    cout<<bin<<"\t";
    cout<< chMisID_data->GetBinContent(bin)<<"\t"<< chMisID_data->GetBinError(bin)<<"\t";
    cout<< chMisID_dy  ->GetBinContent(bin)<<"\t"<< chMisID_dy  ->GetBinError(bin)<<"\t";
    cout<< scalefactor ->GetBinContent(bin)<<"\t"<< scalefactor ->GetBinError(bin)<<endl;   
  }
  cout<<defaultfloat<<endl;

  /*
  //HT binned scale-factors:
  vector<TH1F*> hst = return_hist("HT");
  TH1F *hst_data, *hst_others, *hst_dy;
  for(int i=0; i<(int)hst.size(); i++){
    if(i==0){
      hst_others = (TH1F *)hst[i]->Clone();
      hst_others->Reset();
    }
    if(TString(hst[i]->GetName())=="DY")        hst_dy   = hst[i];
    else if(TString(hst[i]->GetName())=="Data") hst_data = hst[i];
    else hst_others->Add(hst[i]);
  }

  //Print out information:
  int nbins = hst_data->GetNbinsX();
  cout<<"nbin \tData \tOthers \tDY"<<endl;
  cout<<fixed<<setprecision(1);
  for(int bin=0; bin<=nbins+2; bin++){
    cout<<bin<<"\t";
    cout<< hst_data   ->GetBinContent(bin)<<"\t";
    cout<< hst_others ->GetBinContent(bin)<<"\t";
    cout<< hst_dy     ->GetBinContent(bin)<<endl;
  }
  cout<<defaultfloat<<endl;*/

  
  //---------
  // Drawing 
  //---------

  canvas = create_canvas("HT", "HT", 600, 600); // Square canvas

  // Create the mainPad to occupy the top 70% of the canvas
  mainPad = create_mainPad(0.0, 0.3, 1.0, 1.0); // Full width, 70% height
  mainPad->SetRightMargin(0.05); // Standard CMS right margin
  mainPad->SetLeftMargin(0.15); // Standard CMS left margin
  mainPad->SetBottomMargin(0.05); // Small bottom margin to separate from ratio pad
  mainPad->SetLogy(0);
  mainPad->Draw();

  // Create the ratioPad to occupy the bottom 30% of the canvas
  ratioPad = create_ratioPad(0.0, 0.0, 1.0, 0.3); // Full width, 30% height
  ratioPad->SetRightMargin(0.05); // Standard CMS right margin
  ratioPad->SetLeftMargin(0.15); // Standard CMS left margin, same as mainPad
  ratioPad->SetTopMargin(0.03); // Small top margin to separate from main pad
  ratioPad->SetBottomMargin(0.35); // Typical ratio pad bottom margin
  ratioPad->SetLogy(0);
  ratioPad->Draw();

  //Drawing on mainpad
  mainPad->cd();
  mainPad->SetGrid(0,0);
  dummy = (TH1F *)chMisID_data->Clone(); dummy->Reset();
  dummy->SetTitle("");
  dummy->GetYaxis()->SetTitle("Electron charge-flip rate");
  dummy->GetYaxis()->SetTitleSize(0.07);
  dummy->GetYaxis()->SetTitleOffset(1.00);
  dummy->GetYaxis()->SetLabelSize(0.05);
  dummy->GetYaxis()->SetTickSize(0.01);
  dummy->GetYaxis()->SetRangeUser(0, 0.07);
  dummy->GetXaxis()->SetTickSize(0.01);
  dummy->GetXaxis()->SetRangeUser(0, 1000);
  dummy->SetStats(0);
  dummy->Draw("hist");
  
  chMisID_data -> Draw("AP SAME");
  chMisID_dy   -> Draw("AP SAME");
  dummy->Draw("sameaxis");
  canvas->Update();

  //Drawing on ratiopad
  ratioPad->cd();
  //scalefactor->SetStats(0);
  SetRatioStyle(scalefactor, "SF");
  scalefactor->GetXaxis()->SetTitle("p_{T}^{LL} (GeV)");
  scalefactor->GetYaxis()->SetTitle("SF");
  scalefactor->GetYaxis()->SetTitleSize(0.15);
  scalefactor->GetYaxis()->SetTitleOffset(0.43);
  scalefactor->GetYaxis()->SetLabelSize(0.13); 
  float xlow  = scalefactor->GetXaxis()->GetBinLowEdge(1);
  float xhigh = scalefactor->GetXaxis()->GetBinUpEdge(scalefactor->GetNbinsX());
  TLine *line = new TLine(xlow, 1, xhigh, 1);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  
  scalefactor->Draw("EP");
  line->Draw("same");
  scalefactor->Draw("EP SAME");
  canvas->Update();

  //Putting texts:
  mainPad->cd();
  put_text("CMS", 0.17, 0.83, 62, 0.07);
  put_text("Preliminary", 0.27, 0.83, 52, 0.05);
  put_latex_text("59.8 fb^{-1} (2018)", 0.74, 0.94, 42, 0.05);
  put_latex_text(tag2, 0.17, 0.78, 42, 0.04);
  put_latex_text(tag3, 0.17, 0.73, 42, 0.04);
  canvas->Update();

  //Creating legend:
  TLegend *lg = create_legend(0.80, 0.70, 0.94, 0.89);
  lg->SetNColumns(1);
  lg->AddEntry(chMisID_data, "data", "f");
  lg->AddEntry(chMisID_dy, "MC", "f");
  lg->SetHeader("");
  lg->Draw();

  createFolder("overlays_and_others");
  canvas->SaveAs("overlays_and_others/electron_charge-flip_rate.png");
  
  //Done!
  time(&end);
  double time_taken = double(end-start);
  TString report = "\nDone!!\nTime taken : "+to_string((int)time_taken)+" second(s).";
}

//---------------------------------------------
// Reading histograms (runs for each variable)
//---------------------------------------------
vector<TH1F*> return_hist(TString var){

  vector<TH1F*> bkg;
  bkg.clear();

  //Reading histograms:
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
  //Bakcgrounds with single-top:
  vector<TH1F *>ST = {
    get_hist(var, "SingleTop", "s-channel_LeptonDecays",            5178074.5989), //old = 5456748.098),
    get_hist(var, "SingleTop", "t-channel_AntiTop_InclusiveDecays", 1407728.544),
    get_hist(var, "SingleTop", "t-channel_Top_InclusiveDecays",     1558659.612),
    get_hist(var, "SingleTop", "tW_AntiTop_InclusiceDecays",         238357.428),
    get_hist(var, "SingleTop", "tW_Top_InclusiveDecays",             245177.196)
  };
  vector<TH1F *>THX = {
    get_hist(var, "Rare", "THQ",     93446130.4484), //39983860.190527),
    get_hist(var, "Rare", "THW",     98902195.6088), //101122448.979592),
  };
  vector<TH1F *>TZq = {
    get_hist(var, "Rare", "TZq_ll",  12649681.5287) //157598201.29612),
  };
  //Backgrounds with multiple tops:
  vector<TH1F *>TTBar={
    get_hist(var, "TTBar", "TTTo2L2Nu",        1642541.624),
    get_hist(var, "TTBar", "TTToSemiLeptonic", 1304012.700),
  };
  vector<TH1F *>TTW ={
    get_hist(var, "TTW", "TTWToLNu", 48514391.8292) //old:48627268.5
  };
  vector<TH1F *>TTZ ={
    get_hist(var, "TTZ", "TTZToLL", 393271344.4028) //old:107659472.141
  };
  vector<TH1F *>TTX={
    get_hist(var, "Rare", "TTHH",   751314800.9016), //745156482.861401),
    get_hist(var, "Rare", "TTTJ",  1258178158.0272), //1246882793.01746),
    get_hist(var, "Rare", "TTTT",  1589918422.0139), //1613891978.74181),
    get_hist(var, "Rare", "TTTW",   672841187.6873), // 678385059.049712),
    get_hist(var, "Rare", "TTWH",   435582822.0859), //435201401.050788),
    get_hist(var, "Rare", "TTWW",   134953538.2416), //134857142.857143),
    get_hist(var, "Rare", "TTWZ",   203431372.5490), //202686202.686203),
    get_hist(var, "Rare", "TTZH",   442869796.2799), //440528634.361234),
    get_hist(var, "Rare", "TTZZ",   359307359.3074), //358273381.29496),
  };
  //Backgrounds with single V:
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
  vector<TH1F *>WGamma={
    //get_hist(var, "WGamma", "WGToLNuG_Inclusive", 23896.3683),
    get_hist(var, "WGamma", "WGToLNuG_01J", 23450.9243)
  };
  vector<TH1F *> DY = {
    get_hist(var, "DYJetsToLL", "M10to50", 5075.3797), //5925.522),
    get_hist(var, "DYJetsToLL", "M50",    34451.2441)//30321.155)
  };
  vector<TH1F *>ZGamma={
    get_hist(var, "ZGamma", "ZGToLLG_01J", 592588.5918)
  }; 
  //Backgrounds with VV:
  vector<TH1F *>VV={
    get_hist(var, "WW", "WWTo1L1Nu2Q", 787485.589),
    get_hist(var, "WW", "WWTo2L2Nu",   901172.227),
    get_hist(var, "WW", "WWTo4Q",      773049.853),
    get_hist(var, "WZ", "WZTo1L1Nu2Q", 805257.731),
    get_hist(var, "WZ", "WZTo2Q2L",   4499605.731),
    get_hist(var, "WZ", "WZTo3LNu",   1889798.538),
    get_hist(var, "ZZ", "ZZTo2L2Nu", 58416512.631),
    get_hist(var, "ZZ", "ZZTo2Q2L",   7928149.608),
    get_hist(var, "ZZ", "ZZTo2Q2Nu",  4405016.452),
    get_hist(var, "ZZ", "ZZTo4L",    74330566.038),
  };
  vector<TH1F *>WpWp={
    get_hist(var, "WpWp", "WpWpJJEWK", 3041362.5304),
    //get_hist(var, "WpWp", "WpWpJJQCD", 6313131.3131),
  };
  //Backgrounds with VVV:
  vector<TH1F *>VVV={
    get_hist(var, "WWW", "Inclusive", 1112140.8712),
    get_hist(var, "WWZ", "Inclusive", 1452841.24194),
    get_hist(var, "WZZ", "Inclusive", 5254860.74619),
    get_hist(var, "ZZZ", "Inclusive", 16937669.376694),
  };
  //Higgs processes:
  vector<TH1F *>Higgs={
    get_hist(var, "Higgs", "bbH_HToZZTo4L",      3533384497.3139),
    get_hist(var, "Higgs", "GluGluHToZZTo4L",   28693528693.5287),
    get_hist(var, "Higgs", "GluGluToZH_HToZZTo4L", 75554356.2066),
    get_hist(var, "Higgs", "GluGluZH_HToWW_ZTo2L",344444108.7613),
    get_hist(var, "Higgs", "ttHToNonbb",           34603366.3834),
    get_hist(var, "Higgs", "VBF_HToZZTo4L",       472277227.7228),
    get_hist(var, "Higgs", "VHToNonbb",            13166481.4209)
  };
  //Data:
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

  //---------------
  //Managing data:
  //---------------
  TH1F *hst_smuon = merge_and_decorate(SingleMuon, "SingleMuon", kBlack);
  TH1F *hst_egamma= merge_and_decorate(EGamma,     "EGamma",     kBlack);
  TH1F *hst_data = (TH1F *)hst_smuon->Clone();
  hst_data = (TH1F *)hst_smuon->Clone();
  if (hst_egamma) {
    for (int bin = 1; bin <= hst_data->GetNbinsX(); bin++) {
      Double_t content_smuon = hst_data->GetBinContent(bin);
      Double_t error_smuon = hst_data->GetBinError(bin);
      Double_t content_egamma = hst_egamma->GetBinContent(bin);
      Double_t error_egamma = hst_egamma->GetBinError(bin);
      hst_data->SetBinContent(bin, content_smuon + content_egamma);
      hst_data->SetBinError(bin, sqrt(error_smuon * error_smuon + error_egamma * error_egamma));
    }
  }
  hst_data->SetName("Data");
  hst_data->SetTitle("Data");

  //Return Data and bkg histograms for this variable.
  vector<TH1F*> hst_collection = bkg;
  hst_collection.push_back(hst_data);
  cout<<"Collections loaded for "<<var<<" :"<<endl;
  for(int i=0; i<(int)hst_collection.size(); i++) cout<<hst_collection[i]->GetName()<<endl;
  cout<<endl;
  return hst_collection;
}
