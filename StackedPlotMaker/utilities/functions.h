#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "../include_headers.h"
extern float globalSbyB, globalObsbyExp, globalObsbyExpErr;

//--------------------------------------------------
// File handling:
//--------------------------------------------------

inline bool file_exists(TString filename){
  std::string filepath = (std::string)filename;
  std::ifstream file(filepath);
  return file.good();
}

void createFolder(TString foldername){
  std::string processline = "mkdir -p "+(std::string)foldername;
  int result = system(processline.c_str());
  if (result!=0) cout<<"Error : Failed to create folder."<<endl;
}

string todays_date(){
  string processline = "date +%Y-%m-%d";
  array<char, 128> buffer;
  string result;
  unique_ptr<FILE, decltype(&pclose)> pipe(popen(processline.c_str(), "r"), pclose);
  if(!pipe) throw runtime_error("Failed to run Bash script.");
  //Storing the buffer data in 'result'
  while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) result += buffer.data();
  // Remove trailing newline characters
  while(!result.empty() && (result.back() == '\n' || result.back() == '\r')) result.pop_back();
  return result;
}

//--------------------------------------------------
// Statistics tools:
//--------------------------------------------------

Double_t GetStatUncertainty(TH1D *hist) {
  if (!hist) return 0.0;
  Double_t sum_sq_err = 0.0;
  for (int i = 1; i <= hist->GetNbinsX(); ++i) {
    Double_t bin_content = hist->GetBinContent(i);
    Double_t bin_error = hist->GetBinError(i);
    sum_sq_err += bin_error * bin_error;
  }
  return sqrt(sum_sq_err);
}

TGraphErrors *GetUncertainty(TH1D* hist){
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
  }
  TGraphErrors *err = new TGraphErrors(nBins, x, y, ex, ey);
  //Decoration:
  err->SetMarkerStyle(0);
  err->SetFillColor(kGray+1);
  err->SetLineColor(kGray+1);
  err->SetFillStyle(3001);
  return err;
}

//--------------------------------------------------
// Extracting scale factors:
//--------------------------------------------------

void GetBinwiseSF(TString var, TH1D *hst_data, TH1D *hst_qcd, vector<TH1D*>bkg){
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

//--------------------------------------------------
// Operations on histograms:
//--------------------------------------------------

TH1D *DivideHists(TH1D *hst_num, TH1D*hst_den){
  int nbins = hst_num->GetNbinsX();
  if (!hst_num || !hst_den)          return nullptr;  
  if (nbins != hst_den->GetNbinsX()) return nullptr;
  TH1D* hst_result = (TH1D*)hst_num->Clone("hst_result"); hst_result->Reset();

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

TH1D *GetSbyRootB(TH1D *sig, vector<TH1D*> bkg){
  TH1D *rootb = (TH1D *)bkg[0]->Clone(); rootb->Reset();
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
  //TH1D * srb = (TH1D *)sig->Clone("copy");
  //srb->Divide(rootb);
  TH1D *srb = DivideHists(sig, rootb);
  
  float nsig = sig->Integral();
  float nbkg = 0; for(int i=0; i<(int)bkg.size(); i++) nbkg = nbkg + bkg[i]->Integral();
  float sqrtB = sqrt(nbkg);
  globalSbyB = nsig/sqrtB;

  SetHistoStyle(srb, kBlack);
  return srb;
}

TH1D *GetRatio(TH1D *data, vector<TH1D*> bkg){
  //First, get a sum of all the backgrounds:
  TH1D *allbkg = (TH1D *)bkg[0]->Clone(); allbkg->Reset();
  for(int i=0; i<(int)bkg.size(); i++) allbkg->Add(bkg[i]);
  if(!allbkg){
    cout<<"Error: Background is null!"<<endl;
    return nullptr;
  }
  TH1D *ratio = DivideHists(data, allbkg);

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

  SetHistoStyle(ratio, kBlack);
  return ratio;
}

//--------------------------------------------------
// On-screen displays:
//--------------------------------------------------

void DisplayYieldsInBins(TH1D *data, TH1D*bkg){
  if(data==nullptr) cout<<"Data is empty!"<<endl;
  if(bkg ==nullptr) cout<<"Bkg is empty!" <<endl; 
  int nbins = bkg->GetNbinsX();
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

void DisplaySignalYieldsInBins(TH1D *sig){
  if(sig ==nullptr) cout<<"Signal is empty!" <<endl;
  int nbins = sig->GetNbinsX();
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

void DisplayYieldsInBins(TH1D *hst){
  if(hst ==nullptr) cout<<"Hist is empty!" <<endl;
  int nbins = hst->GetNbinsX();
  cout << fixed << setprecision(2);
  cout<<"------------------"<<endl;
  cout<<hst->GetName()<<endl;
  cout<<"------------------"<<endl;
  float total = 0;
  for(int bin=0; bin<=nbins; bin++){
    float nhst   = hst->GetBinContent(bin);
    float err    = hst->GetBinError(bin);
    cout<<bin<<"\t"<<nhst<<" ± "<<err<<endl;
    total = total + nhst;
  }
  cout<<"Total = "<<total<<endl;
  cout << defaultfloat << endl;
}

void GetBinwiseSF(TString var, TH1D *hst_data, vector<TH1D*>bkg, TString targetname){

  //Find the target histogram:
  TH1D *hst_target;
  for(int i=0; i<(int)bkg.size(); i++){
    if(bkg[i]->GetName() == (TString)targetname){
      hst_target = bkg[i];
      DisplayText("Calculating scale-factors for "+string(bkg[i]->GetName()), 33);
      //cout<<"Calculating scale-factors for "<<bkg[i]->GetName()<<endl;
      break;
    }
  }
  
  //DisplayText("\nScaleFactors in each "+var+"  bin:", 33);
  cout<<"bin\tnData\tnTarget\tnOthers\tScaleFactor\tbin-range"<<endl;

  //For each bin:
  for(int bin=0; bin<(int)hst_target->GetNbinsX(); bin++){
    float ndata   = hst_data->GetBinContent(bin+1);
    float ntarget = hst_target ->GetBinContent(bin+1);
    float nothers = 0;
    float binxlow = hst_data->GetXaxis()->GetBinLowEdge(bin+1);
    float binxup  = hst_data->GetXaxis()->GetBinUpEdge(bin+1);
    for(int i=0; i<(int)bkg.size(); i++){
      if(bkg[i]->GetTitle() == (TString)targetname) continue;
      else nothers = nothers + bkg[i]->GetBinContent(bin+1);
    }
    float sf_bin = 0; if(ntarget !=0) sf_bin = (ndata-nothers)/ntarget;
      
    cout<<bin+1<<"\t"<<(int)ndata<<"\t"<<(int)ntarget<<"\t"<<(int)nothers<<"\t";
    cout<<fixed<<setprecision(7)<<sf_bin<<defaultfloat<<"\t";
    cout<<binxlow<<"-"<<binxup<<endl;
  }
  
  float nothers_total = 0;
  for(int i=0; i<(int)bkg.size(); i++){
    if(bkg[i]->GetTitle() == (TString)targetname) continue;
    else nothers_total = nothers_total + bkg[i]->Integral();
  }
  
  //Global:
  float globalsf = 0;
  if(hst_target->Integral()!=0) globalsf = (hst_data->Integral()-nothers_total)/hst_target->Integral();
  cout<<"Global\t";
  cout<<(int)hst_data   ->Integral()<<"\t";
  cout<<(int)hst_target ->Integral()<<"\t";
  cout<<(int)nothers_total<<"\t";
  cout<<fixed<<setprecision(7)<<globalsf<<defaultfloat<<"\t";
  cout<<"all\n"<<endl;
}

void refine_hist(TH1D* hst) {
  //Getting rif of nan bins in a histogram.
  for (int i = 1; i <= hst->GetNbinsX(); ++i) {
    double content = hst->GetBinContent(i);
    double error = hst->GetBinError(i);
    if (std::isnan(content) || std::isnan(error)) {
      hst->SetBinContent(i, 0);
      hst->SetBinError(i, 0);
    }
  }
}
#endif // FUNCTIONS_H
