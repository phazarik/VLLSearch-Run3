#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "../include_headers.h"
extern float globalSbyB, globalSbyBErr, globalObsbyExp, globalObsbyExpErr;

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
  int nBins = hist->GetNbinsX();
  std::vector<Double_t> x(nBins), y(nBins), ex(nBins), ey(nBins);
  for (int i = 0; i < nBins; i++) {
    int bin = i + 1;
    Double_t nevt    = hist->GetBinContent(bin);
    Double_t binlow  = hist->GetBinLowEdge(bin);
    Double_t binhi   = binlow + hist->GetBinWidth(bin);
    Double_t nevtErr = hist->GetBinError(bin);
    x[i]  = (binlow + binhi) / 2;
    ex[i] = (binhi - binlow) / 2;
    y[i]  = 1;
    ey[i] = (nevt != 0) ? nevtErr / nevt : 0;
  }
  TGraphErrors *err = new TGraphErrors(nBins, x.data(), y.data(), ex.data(), ey.data());
  err->SetMarkerStyle(0);
  err->SetFillColor(kGray + 1);
  err->SetLineColor(kGray + 1);
  err->SetFillStyle(3001);
  return err;
}

//--------------------------------------------------
// Operations on histograms:
//--------------------------------------------------

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

TH1D *DivideHists(TH1D *hst_num, TH1D*hst_den){
  int nbins = hst_num->GetNbinsX();
  if (!hst_num || !hst_den)          return nullptr;  
  if (nbins != hst_den->GetNbinsX()) return nullptr;
  TH1D* hst_result = (TH1D*)hst_num->Clone("hst_result"); hst_result->Reset();

  for (int i = 0; i <= nbins + 1; ++i) {
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
  for(int bin=0; bin<rootb->GetNbinsX()+1; bin++){
    double val = rootb->GetBinContent(bin);
    double err = rootb->GetBinError(bin);
    if(val > 0){
      rootb->SetBinContent(bin, sqrt(val));
      rootb->SetBinError(bin, err / (2.0 * sqrt(val)));
    } else {
      rootb->SetBinContent(bin, 0);
      rootb->SetBinError(bin, 0);
    }
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

  double serr2 = 0;
  for(int bin=1; bin<=srb->GetNbinsX(); bin++){
    double err = srb->GetBinError(bin);
    if(std::isnan(err) || std::isinf(err)) continue;
    serr2 += err * err;
  }
  globalSbyBErr = sqrt(serr2);

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

void GetBinwiseSF(TString var, TString targetvar, TH1D *hst_data, vector<TH1D*> bkg, TString targetname) {
  if (var != targetvar) return;
  vector<int> w = {6, 9, 9, 9, 28, 11};
  
  TH1D *hst_target = nullptr;
  for (int i = 0; i < (int)bkg.size(); i++) {
    if (bkg[i]->GetName() == targetname) {
      hst_target = bkg[i];
      cout<<"\nScale Factors for "<<bkg[i]->GetName()<<":"<<endl;
      break;
    }
  }
  if (!hst_target) {
    DisplayText("SF: target not found: " + targetvar);
    return;
  }

  auto print_line = [&]() {
    printf("+");
    for (int i = 0; i < (int)w.size(); i++) {
      for (int j = 0; j < w[i]; j++) printf("-");
      printf("+");
    }
    printf("\n");
  };

  print_line();
  printf("|%*s|%*s|%*s|%*s|%*s|%*s|\n",
       w[0], "bin", w[1], "nData", w[2], "nOthers",
       w[3], "nTarget", w[4], "ScaleFactor ± Error", w[5], "bin-range");
  print_line();

  double total_ndata = 0, total_ntarget = 0, total_nothers = 0;
  float total_ndata_error = 0, total_ntarget_error = 0, total_nothers_error = 0;

  for (int bin = 0; bin < hst_data->GetNbinsX(); bin++) {
    float ndata   = hst_data->GetBinContent(bin + 1);
    float ntarget = hst_target->GetBinContent(bin + 1);
    float binxlow = hst_data->GetXaxis()->GetBinLowEdge(bin + 1);
    float binxup  = hst_data->GetXaxis()->GetBinUpEdge(bin + 1);
    float nothers = 0;
    float ndata_error = 0;
    for (int i = 0; i < (int)bkg.size(); i++) {
      if (bkg[i]->GetName() == targetname) continue;
      nothers += bkg[i]->GetBinContent(bin + 1);
    }

    ndata_error = hst_data->GetBinError(bin + 1); // Get the error on data
    float ntarget_error = hst_target->GetBinError(bin + 1); // Get the error on the target

    // Calculate bin error for SF
    float sf_bin = 0;
    float sf_error = 0;
    if (ntarget != 0) {
      sf_bin = (ndata - nothers) / ntarget;
      sf_error = sf_bin * sqrt(pow(ndata_error / (ndata - nothers), 2) + pow(ntarget_error / ntarget, 2));
    }

    total_ndata   += ndata;
    total_ntarget += ntarget;
    total_nothers += nothers;
    total_ndata_error += pow(ndata_error, 2);
    total_ntarget_error += pow(ntarget_error, 2);

    TString sf_string = TString::Format("%.6f ± %.6f", sf_bin, sf_error);
    TString range = TString::Format("%d–%d", (int)binxlow, (int)binxup);
    printf("|%*d|%*.1f|%*.1f|%*.1f|%*s|%*s  |\n",
	   w[0], bin + 1,
	   w[1], ndata,
	   w[2], nothers,
	   w[3], ntarget,
	   w[4], sf_string.Data(),
	   w[5], range.Data());
  }

  print_line();

  // Calculate global SF error
  float global_sf = (float)(total_ndata - total_nothers) / total_ntarget;
  float global_sf_error = global_sf * sqrt(
    pow(sqrt(total_ndata_error) / (total_ndata - total_nothers), 2) + 
    pow(sqrt(total_ntarget_error) / total_ntarget, 2)
  );
  
  TString global_sf_string = TString::Format("%.6f ± %.6f", global_sf, global_sf_error);
  printf("|%*s|%*.1f|%*.1f|%*.1f|%*s|%*s|\n",
	 w[0], "all",
	 w[1], total_ndata,
	 w[2], total_nothers,
	 w[3], total_ntarget,
	 w[4], global_sf_string.Data(),
	 w[5], " ");
  
  print_line();
  cout<<endl;
  cout<<"Global SF = "<<global_sf_string.Data()<<endl;
}

void DisplayBinwiseSF(TString var, TString targetvar, TH1D *hst_data, vector<TH1D*> bkg, TString targetname) {
  if (var != targetvar) return;

  TH1D *hst_target = nullptr;
  for (int i = 0; i < (int)bkg.size(); i++) {
    if (bkg[i]->GetName() == targetname) {
      hst_target = bkg[i];
      break;
    }
  }
  if (!hst_target) {
    DisplayText("SF: target not found: " + targetvar);
    return;
  }

  printf("{\n");
  printf("  \"bins\": [\n");

  int nbins = hst_data->GetNbinsX();
  for (int bin = 0; bin < nbins; bin++) {
    float ndata   = hst_data->GetBinContent(bin + 1);
    float ntarget = hst_target->GetBinContent(bin + 1);
    float binxlow = hst_data->GetXaxis()->GetBinLowEdge(bin + 1);
    float binxup  = hst_data->GetXaxis()->GetBinUpEdge(bin + 1);
    float nothers = 0;
    float ndata_error = hst_data->GetBinError(bin + 1);
    float ntarget_error = hst_target->GetBinError(bin + 1);

    for (int i = 0; i < (int)bkg.size(); i++) {
      if (bkg[i]->GetName() == targetname) continue;
      nothers += bkg[i]->GetBinContent(bin + 1);
    }

    float sf_bin = 0, sf_error = 0;
    if (ntarget != 0) {
      sf_bin = (ndata - nothers) / ntarget;
      sf_error = sf_bin * sqrt(pow(ndata_error / (ndata - nothers), 2) + pow(ntarget_error / ntarget, 2));
    }
    
    TString high_str = (bin == nbins - 1) ? "\"inf\"" : TString::Format("%.1f", binxup);
    printf("    { \"low\": %.1f, \"high\": %s, \"scale\": [%.6f, %.6f] }%s\n",
           binxlow,
           high_str.Data(),
           sf_bin,
           sf_error,
	   (bin == nbins - 1 ? "" : ","));
  }

  printf("  ]\n");
  printf("}\n");
}

#endif // FUNCTIONS_H
