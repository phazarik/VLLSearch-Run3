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
double TOPscale;
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

void makeplotForCMS(TString _var = "dilep_mass", TString _name = "Dilep mass (GeV)", int _nbins = 200, float _xmin = 0.0, float _xmax = 200, int _rebin = 2){
//void makeplotForCMS(){
  time_t start, end;
  time(&start);

  //SET GLOBAL SETTINGS HERE:
  channel = "mm";
  TString jobname = "hst_2LSS_baselinewithnn_Sept24_"+channel;
  input_path = "../input_hists/"+jobname;
  globalSbyB = 0;
  toSave = true;
  toLog = true;
  toOverlayData = false;
  toZoom = false; //forcefully zooms on the x axis.
  tag = "baseline_withnn_"+channel; //Don't use special symbols (folder name)
  TString info = "baseline";

  //DON'T TOUCH BELOW:
  if     (channel == "ee") tag2 = info+" (e-e)"; //This appears on the plot.
  else if(channel == "em") tag2 = info+" (e-#mu)";
  else if(channel == "me") tag2 = info+" (#mu-e)";
  else if(channel == "mm") tag2 = info+" (#mu-#mu)";
  else DisplayText("Error! Please provide correct channel name!", 31);

  QCDscale = 1.0;
  TOPscale = 1.0;
  /*
  //------------2024-08-07---------------//
  if     (channel == "ee") QCDscale = 0.70572926; //ee-channel
  else if(channel == "em") QCDscale = 0.25988226; //em-channel
  else if(channel == "mm") QCDscale = 0.18630010; //mm-channel
  else DisplayText("Error! Please provide correct channel name!", 31);
  //-------------------------------------//
  //------------2024-08-09---------------//
  if     (channel == "ee") QCDscale = 0.49775447; //ee-channel
  else if(channel == "em") QCDscale = 0.27948709; //em-channel
  else if(channel == "mm") QCDscale = 0.20128545; //mm-channel
  else DisplayText("Error! Please provide correct channel name!", 31);
  //-------------------------------------//
  //------------2024-08-09-v2------------//
  if     (channel == "ee") QCDscale = 0.15881616; //ee-channel
  else if(channel == "em") QCDscale = 0.23838132; //em-channel
  else if(channel == "me") QCDscale = 0.23838132; //em-channel
  else if(channel == "mm") QCDscale = 0.18179923; //mm-channel
  else DisplayText("Error! Please provide correct channel name!", 31);*/
  //-------------------------------------//

  //------------2024-09-16------------//
  //if(channel == "mm") TOPscale = 1.309116; //mm-channel
  
  struct plotdata {
    TString var;
    TString name;
    int nbins;
    float xmin;
    float xmax;
    int rebin;
  };

  vector<plotdata> p = {
    {.var=_var, .name= _name, _nbins, _xmin, _xmax, _rebin}
    //Rebin can be overwritten inside the plot loop.
    //{.var="dilep_mass", .name="M_{ll} (GeV)",  200, 0, 200, 2},
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
  //QCD processes:
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
  
  //DisplayText("Reading done.", 33);
  
  //Merging the histograms from each samples and storing in a collection:
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
  //for(int i=0; i<(int)bkg.size(); i++) allbkg->Add(bkg[i]);
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
  }
  allbkg->SetFillStyle(3013);
  allbkg->SetFillColor(kGray+3);
  allbkg->SetMarkerSize(0); //No marker

  //Debugging yield and uncertainty:
  //cout << fixed << setprecision(2);
  //cout<<"test: "<<allbkg->Integral()<<" +- "<<GetStatUncertainty(allbkg)<<endl;
  //cout << defaultfloat << endl;

  //Stacking:
  std::sort(bkg.begin(), bkg.end(), compareHists);
  bkgstack = new THStack("Stacked",var+";"+var+";Events");
  for(int i=0; i<(int)bkg.size(); i++) bkgstack->Add(bkg[i]);
  //bkgstack->Add(hst_qcd);
  SetFillColorFromLineColor(bkgstack);
  bkgstack->SetTitle("");
 
  //######################################################################
  // Managing signal:
  if(channel == "ee"){
    sig1 = get_hist(var, "VLLD", "ele_M100", 6560.41);   if(sig1) {SetHistoStyle(sig1, kRed+2); sig1->SetTitle("VLLD e_{100}");}
    sig2 = get_hist(var, "VLLD", "ele_M400", 270022.05); if(sig2) {SetHistoStyle(sig2, kRed+0); sig2->SetTitle("VLLD e_{400}");}
  }
  else if (channel == "em"){
    sig1 = get_hist(var, "VLLD", "mu_M100",  6622.84);   if(sig1) {SetHistoStyle(sig1, kRed+2); sig1->SetTitle("VLLD #mu_{100}");}
    sig2 = get_hist(var, "VLLD", "ele_M100", 6560.41);   if(sig2) {SetHistoStyle(sig2, kRed+0); sig2->SetTitle("VLLD e_{100}");}
  }
  else if (channel == "me"){
    sig1 = get_hist(var, "VLLD", "mu_M100",  6622.84);   if(sig1) {SetHistoStyle(sig1, kRed+2); sig1->SetTitle("VLLD #mu_{100}");}
    sig2 = get_hist(var, "VLLD", "ele_M100", 6560.41);   if(sig2) {SetHistoStyle(sig2, kRed+0); sig2->SetTitle("VLLD e_{100}");}
  }
  else if (channel == "mm"){
    sig1 = get_hist(var, "VLLD", "mu_M100", 6560.41);    if(sig1) {SetHistoStyle(sig1, kRed+2); sig1->SetTitle("VLLD #mu_{100}");}
    sig2 = get_hist(var, "VLLD", "mu_M400", 267905.18);  if(sig2) {SetHistoStyle(sig2, kRed+0); sig2->SetTitle("VLLD #mu_{400}");}
  }
  sig3 = nullptr;//get_hist(var, "VLLD", "ele_M300", 85061.86);
  //if(sig1) {SetHistoStyle(sig1, kRed+2); sig1->SetTitle("VLLD e_{100}");}
  //if(sig2) {SetHistoStyle(sig2, kRed+0); sig2->SetTitle("VLLD #mu_{100}");}
  //if(sig3) {SetHistoStyle(sig3, kRed+2); sig3->SetTitle("VLLD e_{300}");}

  vector<TH1F*> sigvec = {sig1, sig2, sig3};

  //######################################################################
  //Managing data:
  TH1F *hst_smuon = merge_and_decorate(SingleMuon, "SingleMuon", kBlack);
  TH1F *hst_egamma= merge_and_decorate(EGamma,     "EGamma",     kBlack);

  hst_data = (TH1F *)hst_smuon->Clone();
  //if(hst_egamma) hst_data->Add(hst_egamma);
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
  }
  hst_data->SetName("Data (2018)");
  
  //----------------------------------------------------------------------------
  //ON SCREEN DISPLAYS:
  //For the data and background file: nObs, nExp, nExpErr
  //For the signal file: nSig, nSigErr
  if(var == "dilep_mass"){
    cout << fixed << setprecision(2);

    // Output for Obs/Exp file
    cout << "\nObs/Exp file:" << endl;
    cout << setw(10) << left << "Obs"
	 << setw(10) << left << "Exp"
	 << setw(10) << left << "ExpErr" << endl;
    cout << setw(10) << left << allbkg->Integral()
	 << setw(10) << left << allbkg->Integral()
	 << setw(10) << left << GetStatUncertainty(allbkg) << endl;
    // Output for the data and signal file: name, nSig, nSigErr
    cout <<"\nSignal yields:"  << endl;
    cout << setw(15) << left  << "Name"
	 << setw(15) << right << "nSig"
	 << setw(10) << right << "nSigErr" << endl;

    for (int s = 0; s<(int)sigvec.size(); s++) {
      if(sigvec[s]){
	cout << setw(15) << left <<  sigvec[s]->GetTitle()
	     << setw(15) << right << sigvec[s]->Integral()
	     << setw(10) << right << GetStatUncertainty(sigvec[s]) << endl;
      }
    }
    cout << defaultfloat << endl;

    //Yield for each background:
    DisplayText("Yields with uncertainty for each sample:", 33);
    cout<<fixed<<setprecision(2);
    Double_t sum_bkg = 0; Double_t sum_bkg_sqerr = 0;
    for(int i=(int)bkg.size()-1; i>=0; i--){
      cout<<setw(15)<<left<<bkg[i]->GetTitle()<<"\t"<<bkg[i]->Integral()<<"\\pm"<<GetStatUncertainty(bkg[i])<<endl;
      sum_bkg += bkg[i]->Integral();
      sum_bkg_sqerr += pow(GetStatUncertainty(bkg[i]), 2);
    }
    cout<<setw(15)<<left<<"Total bkg\t"<<sum_bkg<<"\\pm"<<sqrt(sum_bkg_sqerr)<<endl;
    if(toOverlayData) cout<<setw(15)<<left<<"Data\t"<<right<<hst_data->Integral()<<"\\pm"<<GetStatUncertainty(hst_data)<<endl;
    if(sig1) cout<<setw(15)<<left<<sig1->GetTitle()<<"\t"<<sig1->Integral()<<"\\pm"<<GetStatUncertainty(sig1)<<endl;
    if(sig2) cout<<setw(15)<<left<<sig2->GetTitle()<<"\t"<<sig2->Integral()<<"\\pm"<<GetStatUncertainty(sig2)<<endl;
    if(sig3) cout<<setw(15)<<left<<sig3->GetTitle()<<"\t"<<sig3->Integral()<<"\\pm"<<GetStatUncertainty(sig3)<<endl;
    cout<<defaultfloat<<endl;

    DisplayYieldsInBins(allbkg, allbkg);
    DisplaySignalYieldsInBins(sig2);

  }
  //-----------------------------------------------------------------------------
  //---------------------------------------------
  // Plotting:
  //---------------------------------------------
  canvas = create_canvas(var, filename, 600, 600); // Square canvas

  // Create the mainPad to occupy the top 70% of the canvas
  mainPad = create_mainPad(0.0, 0.3, 1.0, 1.0); // Full width, 70% height
  mainPad->SetRightMargin(0.05); // Standard CMS right margin
  mainPad->SetLeftMargin(0.15); // Standard CMS left margin
  mainPad->SetBottomMargin(0.05); // Small bottom margin to separate from ratio pad
  mainPad->Draw();

  // Create the ratioPad to occupy the bottom 30% of the canvas
  ratioPad = create_ratioPad(0.0, 0.0, 1.0, 0.3); // Full width, 30% height
  ratioPad->SetRightMargin(0.05); // Standard CMS right margin
  ratioPad->SetLeftMargin(0.15); // Standard CMS left margin, same as mainPad
  ratioPad->SetTopMargin(0.03); // Small top margin to separate from main pad
  ratioPad->SetBottomMargin(0.35); // Typical ratio pad bottom margin
  ratioPad->Draw();

  mainPad->cd();
  mainPad->SetGrid(0,0);
  
  //Setting up a dummy hist to enforce certain things in the plot.
  //It should alywas be drawn first.
  //dummy->Reset();
  dummy = (TH1F *)bkg[0]->Clone(); dummy->Reset();
  dummy->SetTitle("");
  dummy->GetYaxis()->SetTitle("Events");
  dummy->GetYaxis()->SetTitleSize(0.07);
  dummy->GetYaxis()->SetTitleOffset(1.00);
  dummy->GetYaxis()->SetLabelSize(0.05);
  dummy->GetYaxis()->SetTickSize(0.02);
  dummy->GetYaxis()->SetRangeUser(0.1, 10E6);
  dummy->GetXaxis()->SetTickSize(0.02);
  if(toZoom) dummy->GetXaxis()->SetRangeUser(xmin, xmax);
  dummy->SetStats(0);
  dummy->Draw("hist");

  //Now draw the rest.
  //if(toOverlayData) hst_data->Draw("ep same");
  bkgstack->Draw("hist same"); //Stacked (total) background
  allbkg->Draw("E2 same");     //Uncertainty on the total background
  TH1F *allbkg2 = (TH1F*) allbkg->Clone();
  allbkg2->SetFillColor(0);
  allbkg2->SetLineColor(kGray+3);
  allbkg2->Draw("hist same");  //Black hist over the stack
  if(sig1) sig1->Draw("HIST same");
  if(sig2) sig2->Draw("HIST same");
  if(sig3) sig3->Draw("HIST same");
  if(toOverlayData) hst_data->Draw("ep same");

  dummy->Draw("sameaxis"); //Drawing axis ticks on top

  canvas->Update();
  ratioPad->cd();

  //SoverB
  globalSbyB = 0;
  sbyrb = nullptr;
  if     (sig1) sbyrb = GetSbyRootB(sig1, bkg);
  else if(sig2) sbyrb = GetSbyRootB(sig2, bkg);
  else if(sig3) sbyrb = GetSbyRootB(sig3, bkg);
  if(sbyrb){
    SetRatioStyle(sbyrb, name);
    sbyrb->GetYaxis()->SetTitle("S/\\sqrt{B}");
    sbyrb->GetYaxis()->SetTitleSize(0.15);
    sbyrb->GetYaxis()->SetTitleOffset(0.43);
    sbyrb->GetYaxis()->SetLabelSize(0.13);
    if(toZoom) sbyrb->GetXaxis()->SetRangeUser(xmin, xmax);
    if(!toOverlayData) sbyrb->Draw("ep");
    canvas->Update();
  }
  else DisplayText("Warning: SbyRootB is null!");
  
  //obs/exp
  globalObsbyExp =0;
  if(toOverlayData){
    ratiohist = GetRatio(hst_data, bkg);
    SetRatioStyle(ratiohist, name);
    ratiohist->GetYaxis()->SetTitle("obs/exp");
    ratiohist->GetYaxis()->SetTitleSize(0.15);
    ratiohist->GetYaxis()->SetTitleOffset(0.43);
    ratiohist->GetYaxis()->SetLabelSize(0.13);
    ratiohist->GetYaxis()->SetRangeUser(0, 2.2);
    if(toZoom) ratiohist->GetXaxis()->SetRangeUser(xmin, xmax);
  
    //Setting up a horizontal line on the ratiopad:
    float xlow  = ratiohist->GetXaxis()->GetBinLowEdge(1);
    float xhigh = ratiohist->GetXaxis()->GetBinUpEdge(ratiohist->GetNbinsX());
    TLine *line = new TLine(xlow, 1, xhigh, 1);
    line->SetLineColor(kRed);
    line->SetLineWidth(2);
    
    //Calculating the uncertainty on the background in each bin:
    TGraphErrors *err = GetUncertainty(allbkg);
    err->GetYaxis()->SetNdivisions(5, kTRUE);
    err->SetStats(0);
    if(toZoom) err->GetXaxis()->SetRangeUser(xmin, xmax);

    //Drawing everything in the proper order:
    ratiohist->Draw("ep"); //Inheriting the settings from the ratio hist.
    err->Draw("SAME P E2");
    line->Draw("same");
    ratiohist->Draw("ep same"); //I want the ratio to be on top.
    canvas->Update();
  }
  if(channel=="ee" && var=="dilep_mass") draw_veto_region(ratioPad, 76, 106);
  
  mainPad->cd();
  
  //put_text("CMS", 0.10, 0.93, 62, 0.06);
  //put_text("preliminary", 0.18, 0.93, 52, 0.05);
  //put_latex_text(tag2+"", 0.33, 0.93, 42, 0.04);
  //put_latex_text("59.8 fb^{-1} (2018)", 0.61, 0.93, 42, 0.04);

  // Adjusted text positions and sizes for the CMS plot style
  put_text("CMS", 0.17, 0.83, 62, 0.07);            // Larger, bold CMS label
  put_text("Preliminary", 0.27, 0.83, 52, 0.05);    // Smaller preliminary label
  put_latex_text("59.8 fb^{-1} (2018)", 0.74, 0.94, 42, 0.05); // Integrated luminosity on the right
  put_latex_text(tag2, 0.17, 0.78, 42, 0.04); //Additional information

  TLegend *lg = create_legend(0.48, 0.52, 0.94, 0.89);
  lg->SetNColumns(2);
  if(toOverlayData){
    TString data_yield = Form("%d", (int)hst_data->Integral());
    TString text = "Observed ["+data_yield+"]";
    lg->AddEntry(hst_data, text, "f");
    if(hst_smuon)  SetLegendEntry(lg, hst_smuon);
    if(hst_egamma) SetLegendEntry(lg, hst_egamma);
  }

  for(int i=(int)bkg.size()-1; i>=0; i--) SetLegendEntry(lg, bkg[i]);
  if(sig1) SetLegendEntry(lg, sig1);
  if(sig2) SetLegendEntry(lg, sig2);
  if(sig3) SetLegendEntry(lg, sig3);
  TString legendheader = Form("Global S/#sqrt{B} = %.3f", globalSbyB);
  if(toOverlayData){
    TString val = Form("Global obs/exp = %.3f", globalObsbyExp);
    TString err = Form("%.3f", globalObsbyExpErr);
    legendheader = val+" #pm "+err;
    cout<<defaultfloat<<"Obs/Exp = "<<globalObsbyExp<<" ± "<<globalObsbyExpErr<<"\n"<<endl;
  }
  else cout<<legendheader<<"\n"<<endl; 
  lg->SetHeader(legendheader);
  lg->Draw();
  canvas->Update();

  //DisplayText("Made plot for "+var, 0);
  //cout<<"\033[33mMade plot for "+var+"\033[0m\n"<<endl;
  
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
