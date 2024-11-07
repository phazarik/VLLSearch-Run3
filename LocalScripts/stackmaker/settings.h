#ifndef SETTINGS_H
#define SETTINGS_H

#include "decorations.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <string>

//Global variables being used by the class:
extern TString input_path;
extern int nbins;
extern float xmin;
extern float xmax;
extern int rebin;
extern float globalSbyB;
extern float globalObsbyExp;
extern float globalObsbyExpErr;
extern double QCDscale, TOPscale, DYscale;
extern bool toZoom;
extern nlohmann::json lumiData;
extern double datalumi;

//The following function are used by get_hist()
extern TTree* GetFilteredTree(TTree *intree); //defined in the main code

void SetLastBinAsOverflow(TH1F *hst){

  //Handle overlow:
  int lastBin = hst->GetNbinsX();
  double content  = hst->GetBinContent(lastBin);
  double error    = hst->GetBinError(lastBin);
  double overflow = hst->GetBinContent(lastBin + 1);

  double updated_content = content+overflow;
  double updated_error = std::sqrt(error*error + overflow*overflow);
  
  hst->SetBinContent(lastBin, updated_content);
  hst->SetBinError(lastBin, updated_error);

  //Handle underflow:
  double content_first  = hst->GetBinContent(1);
  double error_first    = hst->GetBinError(1);
  double underflow      = hst->GetBinContent(0);

  double updated_content_first = content_first + underflow;
  double updated_error_first = std::sqrt(error_first*error_first + underflow*underflow);

  hst->SetBinContent(1, updated_content_first);
  hst->SetBinError(1, updated_error_first);
  
}

bool file_exists(TString filename){
  std::string filepath = (std::string)filename;
  std::ifstream file(filepath);
  return file.good();
}

void DisplayText(TString text, int color=31){
  //ANSI COLOR CODE:
  //Black   = 30
  //Red     = 31
  //Green   = 32
  //Yellow  = 33
  //Blue    = 34
  //Magenta = 35
  //Cyan    = 36
  //White   = 37
  TString set_color = "\033["+to_string(color)+"m";
  cout<<set_color;
  cout<<text<<endl;
  cout<<"\033[0m"; //0=default
}

TH1F *get_hist(
	       const TString& var,
	       const TString& sample,
	       const TString& subsample,
	       const double& lumi
	       ){

  //Accessing the hist:
  TString filename = input_path+"/"+"hst_"+sample+"_"+subsample+".root";
  if(!file_exists(filename)){
    DisplayText("Not found: "+filename, 31); //31 is the ANSI color code for red
    return nullptr;
  }
  //TFile *file = new TFile(filename, "READ");
  TFile *file = TFile::Open(filename, "READ");
  TH1F *hst = (TH1F *)file->Get(var);
  if(!hst){
    DisplayText("Hist not found for "+filename, 31);
    delete file;
    return nullptr;
  }

  //Note: Handle overflow before scaling.
  SetLastBinAsOverflow(hst);
  hst->Rebin(rebin);

  float scalefactor = 0;
  //float scalefactor = ((TH1F *)file->Get("wt_lumi"))->GetMean();

  //------------------------------------------------------------------
  //Extract luminsoity from json file and scale the histogram:
  //------------------------------------------------------------------
  
  double jsonlumi = 0;
  try    {jsonlumi = lumiData.at(sample.Data()).at(subsample.Data()).get<double>();}
  catch (const json::out_of_range& e) {
    DisplayText("Luminosity data not found for " + sample + " " + subsample, 31);
    return nullptr;
  }
  catch (const json::exception& e) {
    DisplayText("Luminosity data not found for " + sample + " " + subsample, 31);
    return nullptr;
  }
  if(sample != "SingleMuon" || sample != "EGamma")         hst->Scale( datalumi/lumi);

  //Additional scaling on MC:
  if (sample == "QCD_MuEnriched" || sample == "QCD_EMEnriched") hst->Scale(QCDscale);
  else if (sample == "TTBar")                                   hst->Scale(TOPscale);
  else if (sample == "DYJetsToLL")                              hst->Scale(DYscale);
  /*
  if (sample == "QCD_MuEnriched" || sample == "QCD_EMEnriched") hst->Scale((datalumi/lumi)*QCDscale);
  else if (sample == "TTBar")                                   hst->Scale((datalumi/lumi)*TOPscale);
  else if (sample == "DYJetsToLL")                              hst->Scale((datalumi/lumi)*DYscale);
  else if(sample != "SingleMuon" || sample != "EGamma")         hst->Scale( datalumi/lumi);*/

  //cout<<"Hist "+var+" for "+sample+"_"+subsample+" loaded and scaled to : "+scalefactor<<endl;
  return hst;
}

void removeNullPointers(std::vector<TH1F*>& vec) {
  vec.erase(std::remove(vec.begin(), vec.end(), nullptr), vec.end());
}

//--------------------------------------
//Other functions used by the main code:
//--------------------------------------

void createFolder(TString foldername){
  std::string processline = "mkdir -p "+(std::string)foldername;
  int result = system(processline.c_str());
  if (result!=0) cout<<"Error : Failed to create folder."<<endl;
}

std::string todays_date(){
  std::string processline = "date +%Y-%m-%d";
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(processline.c_str(), "r"), pclose);
  if(!pipe) throw std::runtime_error("Failed to run Bash script.");
  //Storing the buffer data in 'result'
  while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) result += buffer.data();
  // Remove trailing newline characters
  while(!result.empty() && (result.back() == '\n' || result.back() == '\r')) result.pop_back();
  return result;
}

TH1F* merge_and_decorate(vector<TH1F*>sample, TString samplename, int color) {
  //Clone a non-null element from the database:
  TH1F *hist;
  for(int i=0; i<(int)sample.size(); i++){
    if(sample[i]){
      hist = (TH1F *)sample[i]->Clone();
      hist->Reset();
      break;
    }
  }
  //Add all the non-null entry to the clone:
  bool allnull = true;
  for(int i=0; i<(int)sample.size(); i++){
    if(sample[i]){
      allnull = false;
      hist->Add(sample[i]);
    }
  }

  if(allnull){
    DisplayText("Null warning : All "+samplename+" hists are null!", 33);
    return nullptr;
  }
  
  if(hist) {
    SetHistoStyle(hist, color);
    hist->SetName(samplename);
    hist->SetTitle(samplename);
    return hist;
  }
  else return nullptr;
}

bool compareHists(const TH1F* a, const TH1F* b) {
    return a->Integral() < b->Integral();
}

void SetFillColorFromLineColor(THStack *stack) {
  TList *histList = stack->GetHists();
  TIter next(histList);
  TObject *obj;
  //iterating over the hists
  while ((obj = next())) {
    if (obj->InheritsFrom("TH1")) {
      TH1 *hist = static_cast<TH1*>(obj);
      //if(!hist) DisplayText("Warning: nullptr for "+(TString *)hist->GetName(), 31);
      hist->SetFillColor(hist->GetLineColor());
      hist->SetLineColor(hist->GetLineColor());
      //hist->SetLineColor(kBlack);
      //hist->SetLineWidth(1);
    }
  }
}

Double_t GetStatUncertainty(TH1F *hist) {
  if (!hist) return 0.0;

  Double_t sum_sq_err = 0.0;
  for (int i = 1; i <= hist->GetNbinsX(); ++i) {
    Double_t bin_content = hist->GetBinContent(i);
    Double_t bin_error = hist->GetBinError(i);
    sum_sq_err += bin_error * bin_error;
  }
  return sqrt(sum_sq_err);
}

TH1F *DivideHists(TH1F *hst_num, TH1F*hst_den){

  int nbins = hst_num->GetNbinsX();

  //Idiot proofing:
  if (!hst_num || !hst_den)          return nullptr;  
  if (nbins != hst_den->GetNbinsX()) return nullptr;

  TH1F* hst_result = (TH1F*)hst_num->Clone("hst_result"); hst_result->Reset();

  for (int i = 1; i <= nbins; ++i) {
    double num = hst_num->GetBinContent(i);
    double den = hst_den->GetBinContent(i);
    double num_err = hst_num->GetBinError(i);
    double den_err = hst_den->GetBinError(i);
    if (den != 0) {
      double bin_content = num / den;
      double bin_error = bin_content * sqrt(pow(num_err/num, 2) + pow(den_err/den, 2));
      hst_result->SetBinContent(i, bin_content);
      hst_result->SetBinError(i, bin_error);
    }
    else {
      hst_result->SetBinContent(i, 0);
      hst_result->SetBinError(i, 0);
    }
  }
  
  return hst_result;
}

TH1F *GetSbyRootB(TH1F *sig, vector<TH1F*> bkg){
  //First, get a sum of all the backgrounds:
  TH1F *rootb = (TH1F *)bkg[0]->Clone(); rootb->Reset();
  for(int i=0; i<(int)bkg.size(); i++) rootb->Add(bkg[i]);
  //Taking the square root:
  for(int bin=0; bin<rootb->GetNbinsX(); bin++){
    double val = rootb->GetBinContent(bin);
    double err = rootb->GetBinError(bin);
    rootb->SetBinContent(bin, std::sqrt(val));
    rootb->SetBinError(bin, err/(2.0*std::sqrt(val)));
  }
  if(!rootb){
    cout<<"Error: Background is null!"<<endl;
    return nullptr;
  }
  TH1F * srb = (TH1F *)sig->Clone("copy");
  srb->Divide(rootb);
  //TH1F *srb = DivideHists(sig, rootb);

  //print global S/sqrtB
  float nsig = sig->Integral();
  float nbkg = 0; for(int i=0; i<(int)bkg.size(); i++) nbkg = nbkg + bkg[i]->Integral();
  float sqrtB = sqrt(nbkg);
  globalSbyB = nsig/sqrtB;
  //cout<<"Global significance = "<<globalSbyB<<endl;
  
  return srb;
}

TH1F *GetRatio(TH1F *data, vector<TH1F*> bkg){
  //First, get a sum of all the backgrounds:
  TH1F *allbkg = (TH1F *)bkg[0]->Clone(); allbkg->Reset();
  for(int i=0; i<(int)bkg.size(); i++) allbkg->Add(bkg[i]);
  if(!allbkg){
    cout<<"Error: Background is null!"<<endl;
    return nullptr;
  }
  //TH1F *ratio = (TH1F *)data->Clone("copy");
  //ratio->Divide(allbkg);
  TH1F *ratio = DivideHists(data, allbkg);

  //calculating global ratio:
  float nobs = data->Integral();
  float nbkg = 0;  for(int i=0; i<(int)bkg.size(); i++) nbkg = nbkg + bkg[i]->Integral();
  globalObsbyExp = nobs/nbkg;
  float nobserr_frac = GetStatUncertainty(data)/nobs;
  float nbkgerr_frac = GetStatUncertainty(allbkg)/nbkg;
  float sumsqerr =  nobserr_frac*nobserr_frac + nbkgerr_frac*nbkgerr_frac;
  globalObsbyExpErr = globalObsbyExp*sqrt(sumsqerr);
  
  // Remove bins where data is zero:
  for (int bin = 1; bin <= data->GetNbinsX(); bin++) {
    if (data->GetBinContent(bin) == 0) {
      ratio->SetBinContent(bin, 0);
      ratio->SetBinError(bin, 0);
    }
  }

  return ratio;
}

TGraphErrors *GetUncertainty(TH1F* hist){
  //Returns a TGraph containing uncertainties per bin on a hist.
  
  //Preparing the points:
  int nBins = hist->GetNbinsX();
  Double_t x[nBins];
  Double_t y[nBins];
  Double_t ex[nBins];
  Double_t ey[nBins];
  for (int i = 0; i < nBins; i++) {
    int bin = i+1;
    Double_t nevt = hist->GetBinContent(bin);
    Double_t binlow = hist->GetBinLowEdge(bin);
    Double_t binhi  = binlow + hist->GetBinWidth(bin);
    Double_t nevtErr= hist->GetBinError(bin);
    x[i]  = (binlow+binhi)/2;
    ex[i] = (binhi-binlow)/2;
    y[i]  = 1;
    ey[i] = 0; if(nevt !=0) ey[i] = nevtErr/nevt;
    //cout<<bin<<"\t"<<x[i]<<"\t"<<y[i]<<"\t"<<ex[i]<<"\t"<<ey[i]<<endl;
  }

  TGraphErrors *err = new TGraphErrors(nBins, x, y, ex, ey);
  //if(!err) DisplayText("Warning: nullptr for "+err->GetName(), 31);
  //Decoration:
  err->SetMarkerStyle(0);
  err->SetFillColor(kGray+1);
  err->SetLineColor(kGray+1);
  err->SetFillStyle(3001);
  return err;
}

void GetBinwiseSF(TString var, TH1F *hst_data, TH1F *hst_qcd, vector<TH1F*>bkg){
  if(var == "HT"){
    DisplayText("\nScaleFactors in each HT bin:", 33);
    cout<<"bin\tnData\tnQCD\tnOthers\tScaleFactor\tbin-range"<<endl;

    //For each bin:
    for(int bin=0; bin<(int)hst_qcd->GetNbinsX(); bin++){
      float ndata = hst_data->GetBinContent(bin+1);
      float nqcd  = hst_qcd ->GetBinContent(bin+1);
      float nothers = 0;
      float binxlow = hst_data->GetXaxis()->GetBinLowEdge(bin+1);
      float binxup  = hst_data->GetXaxis()->GetBinUpEdge(bin+1);
      for(int i=0; i<(int)bkg.size(); i++){
	if(bkg[i]->GetTitle() == (TString)"QCD") continue;
	else nothers = nothers + bkg[i]->GetBinContent(bin+1);
      }
      float sf_bin = 0; if(nqcd !=0) sf_bin = (ndata-nothers)/nqcd;

      cout<<bin+1<<"\t"<<(int)ndata<<"\t"<<(int)nqcd<<"\t"<<(int)nothers<<"\t";
      cout<<fixed<<setprecision(7)<<sf_bin<<defaultfloat<<"\t";
      cout<<binxlow<<"-"<<binxup<<endl;
      
    }

    float nothers_total = 0;
    for(int i=0; i<(int)bkg.size(); i++){
      if(bkg[i]->GetTitle() == (TString)"QCD") continue;
      else nothers_total = nothers_total + bkg[i]->Integral();
    }

    //Global:
    float globalsf = 0;
    if(hst_qcd->Integral()!=0) globalsf = (hst_data->Integral()-nothers_total)/hst_qcd->Integral();
      
    cout<<"Global\t";
    cout<<(int)hst_data->Integral()<<"\t";
    cout<<(int)hst_qcd ->Integral()<<"\t";
    cout<<(int)nothers_total<<"\t";
    cout<<fixed<<setprecision(7)<<globalsf<<defaultfloat<<"\t";
    cout<<"all\n"<<endl;
  }  
}

void DisplayYieldsInBins(TH1F *data, TH1F*bkg){
  if(data==nullptr) cout<<"Data is empty!"<<endl;
  if(bkg ==nullptr) cout<<"Bkg is empty!" <<endl; 
  nbins = bkg->GetNbinsX();
  if(nbins != data->GetNbinsX()) cout<<"Shape mismatch between data and bkg!"<<endl;
  cout << fixed << setprecision(2);
  cout<<"--------------------"<<endl;
  cout<<"Data and backgrounds"<<endl;
  cout<<"--------------------"<<endl;
  cout<<"nbin"<<"\t"<<"nobs"<<"\t"<<"nexp"<<"\t"<<"experr"<<endl;
  for(int bin=1; bin<=nbins; bin++){
    float nobs   = data->GetBinContent(bin);
    float nexp   = bkg ->GetBinContent(bin);
    float experr = bkg ->GetBinError(bin);
    cout<<bin<<"\t"<<nobs<<"\t"<<nexp<<"\t"<<experr<<endl;
  }
  cout << defaultfloat << endl;
}

void DisplaySignalYieldsInBins(TH1F *sig){
  if(sig ==nullptr) cout<<"Signal is empty!" <<endl;
  nbins = sig->GetNbinsX();
  cout << fixed << setprecision(2);
  cout<<"------------------"<<endl;
  cout<<sig->GetTitle()<<endl;
  cout<<"------------------"<<endl;
  cout<<"nbin"<<"\t"<<"nsig"<<"\t"<<"sigerr"<<endl;
  for(int bin=1; bin<=nbins; bin++){
    float nsig   = sig->GetBinContent(bin);
    float sigerr = sig->GetBinError(bin);
    cout<<bin<<"\t"<<nsig<<"\t"<<sigerr<<endl;
  }
  cout << defaultfloat << endl;
}

void DisplayYieldsInBins(TH1F *hst){
  if(hst ==nullptr) cout<<"Hist is empty!" <<endl;
  nbins = hst->GetNbinsX();
  cout << fixed << setprecision(2);
  cout<<"------------------"<<endl;
  cout<<hst->GetName()<<endl;
  cout<<"------------------"<<endl;
  float total = 0;
  /*
  cout<<"nbin"<<"\t"<<"nhst"<<"\t"<<"hsterr"<<endl;
  for(int bin=1; bin<=nbins; bin++){
    float nhst   = hst->GetBinContent(bin);
    float hsterr = hst->GetBinError(bin);
    cout<<bin<<"\t"<<nhst<<"\t"<<hsterr<<endl;
    }*/
  for(int bin=0; bin<=nbins; bin++){
    float nhst   = hst->GetBinContent(bin);
    cout<<nhst<<endl;
    total = total + nhst;
  }
  cout<<"Total = "<<total<<endl;
  cout << defaultfloat << endl;
}

// Function to initialize the JSON data
void loadLuminosityData(string jsonfilepath) {
  std::ifstream jsonFile(jsonfilepath);
  if (jsonFile.is_open()) {
    jsonFile >> lumiData;
    jsonFile.close(); 
  }
  else DisplayText("Failed to open JSON file for luminosity data.", 31);
}

#endif // SETTINGS_H
