#ifndef READ_HISTS_H
#define READ_HISTS_H

#include "../include_headers.h"
#include "decorations.h"
#include "functions.h"
#include <nlohmann/json.hpp>
#include <fstream>
using json = nlohmann::json;

void SetLastBinAsOverflow(TH1D *hst){
  //Handle overlow:
  int lastBin = hst->GetNbinsX();
  double content  = hst->GetBinContent(lastBin);
  double error    = hst->GetBinError(lastBin);
  double overflow = hst->GetBinContent(lastBin + 1);
  double overflow_err = hst->GetBinError(lastBin + 1);
  double updated_content = content+overflow;
  double updated_error = std::sqrt(error * error + overflow_err * overflow_err);
  hst->SetBinContent(lastBin, updated_content);
  hst->SetBinError(lastBin, updated_error);
  //Handle underflow:
  double content_first  = hst->GetBinContent(1);
  double error_first    = hst->GetBinError(1);
  double underflow      = hst->GetBinContent(0);
  double underflow_err  = hst->GetBinError(0);
  double updated_content_first = content_first + underflow;
  double updated_error_first = std::sqrt(error_first*error_first + underflow_err*underflow_err);
  hst->SetBinContent(1, updated_content_first);
  hst->SetBinError(1, updated_error_first);
  //Making multiple calls safe by reseting overflow/underflow:
  hst->SetBinContent(lastBin + 1, 0);
  hst->SetBinError(lastBin + 1, 0);
  hst->SetBinContent(0, 0);
  hst->SetBinError(0, 0);
}

//-------------------------------------------------------------------------
TH1D *get_hist(
	       const TString& var,
	       const TString& input_path,
	       const TString& sample,
	       const TString& subsample,
	       int rebin=1.0
	       ){
  TString filename = input_path+"/"+"hst_"+sample+"_"+subsample+".root";

  //Overlap check for data:
  //if ((sample.Contains("Muon")) || (sample.Contains("EGamma"))) cout<<"Reading data file: "<<filename<<endl;
  
  //Skip samples here:
  //if (sample.Contains("ZGamma"))   return nullptr; //For Run-3 only
  if ((sample.Contains("WtoLNu")) && !sample.Contains("Inclusive")) return nullptr;
  
  if(!file_exists(filename)){
    DisplayText("Not found: "+filename, 31);
    return nullptr;
  }
  TFile *file = TFile::Open(filename, "READ");
  TH1D *hst = (TH1D *)file->Get(var);
  if(!hst){
    DisplayText("Hist not found for "+filename, 31);
    file->Close();
    delete file;
    return nullptr;
  }
  SetLastBinAsOverflow(hst);
  //cout<<"Yield: "<<sample<<"_"<<subsample<<" = "<<(int)hst->Integral()<<endl;

  //--------------------------------------
  //Rebinning:
  vector<TString> rebin_keys = {"nnscore", "STfrac", "sip3d", "eta", "phi", "dR", "ptratio"};
  for (auto& key : rebin_keys){if (var.Contains(key)){rebin = 5; break;}}
  hst->Rebin(rebin);
  //Force rebin here.
  //--------------------------------------

  return hst;
}

TH1D* merge_and_decorate(vector<TH1D*>sample, TString samplename, int color) {
  //Combine histograms from the same samples:
  //1. Clone a non-null element from the database:
  TH1D *hist;
  for(int i=0; i<(int)sample.size(); i++){
    if(sample[i]){
      hist = (TH1D *)sample[i]->Clone();
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

bool compareHists(const TH1D* a, const TH1D* b) {
  return a->Integral() < b->Integral();
}

void SetFillColorFromLineColor(THStack *stack) {
  TList *histList = stack->GetHists();
  TIter next(histList);
  TObject *obj;
  while ((obj = next())) {
    if (obj->InheritsFrom("TH1")) {
      TH1 *hist = static_cast<TH1*>(obj);
      hist->SetFillColor(hist->GetLineColor());
      hist->SetLineColor(hist->GetLineColor());
    }
  }
}

//Names and colors:
struct SampleConfig {
  TString sample;
  TString latex_name;
  Color_t color;
};
SampleConfig default_config = {"", "Other", kGray+2};

vector<TH1D *> return_hist_collection(
				      TString var,
				      TString input_path,
				      TString campaign,
				      int rebin = 1
				      ){

  TString json_path = "../LumiJsons/lumidata_"+campaign+".json";
  std::ifstream infile(json_path.Data());
  if (!infile.is_open())  cerr << "Error: Could not open JSON file: " << json_path << endl;
  infile.seekg(0, std::ios::end);
  if (infile.tellg() == 0) cerr << "Error: JSON file is empty: " << json_path << endl;
  infile.seekg(0, std::ios::beg);
  json sample_info;
  try {
    infile >> sample_info;
  } catch (const std::exception& e) {
    cerr << "Error parsing JSON file: " << json_path << endl;
    cerr << e.what() << endl;
    exit(1);
  }

  vector<SampleConfig> config = {
    {"DYto2L",         "DY",        kRed-7},
    {"DYJetsToLL",     "DY",        kRed-7},
    {"Higgs",          "Higgs",     kMagenta},
    {"QCDMu",          "QCD (#mu)", kYellow},
    {"QCD_MuEnriched", "QCD (#mu)", kYellow},
    {"QCDEM",          "QCD (e#gamma)", kYellow+1},
    {"QCD_EMEnriched", "QCD (e#gamma)", kYellow+1},
    {"RareTop",        "multi-top", kCyan-2},
    {"Rare",           "multi-top", kCyan-2},
    {"ST",             "tX",      kCyan-7},
    {"SingleTop",      "tX",      kCyan-7},
    {"TT",    "t#bar{t}",      kAzure+1},
    {"TTBar", "t#bar{t}",      kAzure+1},
    {"TTV",   "t#bar{t}V",     kAzure+2},
    {"TTW",   "t#bar{t}W",     kAzure+2}, //Disable for Run3
    {"TTZ",   "t#bar{t}Z",     kAzure+2}, //Disable for Run3
    {"TW", "tW", kBlue-3},
    {"VV", "VV", kGreen+1},
    {"WW", "WW", kGreen+1},
    {"WZ", "WZ", kGreen+1},
    {"ZZ", "ZZ", kGreen+1},
    {"VVSS", "WWss", kGreen-5},
    {"WpWp", "WWss", kGreen-5},
    {"VVV",  "VVV", kGreen+3},
    {"WWW",  "WWW", kGreen+3},
    {"WWZ",  "WWZ", kGreen+3},
    {"WZZ",  "WZZ", kGreen+3},
    {"ZZZ",  "ZZZ", kGreen+3},
    {"WGtoLNuG", "W#gamma", kGray+2},
    {"WGamma",   "W#gamma", kGray+2},
    {"WtoLNu",        "W+jets", kGray+1},
    {"HTbinnedWJets", "W+jets", kGray+1},
    {"ZGamma",   "Z#gamma", kRed-9},
    {"DYGToLLG", "Z#gamma",   kRed-9},
  };
  
  vector<TH1D *> hist_collection;

  //Apply config by looping over samples:
  for (auto& [sample_str, subsamples] : sample_info.items()) {
    TString sample = sample_str.c_str();    
    if (sample.Contains("VLLS") || sample.Contains("VLLD"))   continue;
    if (sample.Contains("Muon") || sample.Contains("EGamma")) continue;
    
    SampleConfig* scfg = nullptr;
    for (auto& c : config) {
      if (c.sample == sample) {
        scfg = &c;
        break;
      }
    }
    if (!scfg) {
      DisplayText("Using generic config for " + sample, 33);
      default_config.sample = sample;
      scfg = &default_config;
    }
    
    vector<TH1D*> hists;

    //Looping over subsamples:
    for (auto& [subsample_str, _] : subsamples.items()) {
      TString subsample = subsample_str.c_str();
      TH1D* hst = get_hist(var, input_path, sample, subsample);
      if (hst) {
        SetLastBinAsOverflow(hst);
        //hst->Rebin(rebin);
        //hst->SetName(scfg->latex_name + "_" + subsample);
	hst->SetName(scfg->latex_name);
	hst->SetTitle(sample+"_"+subsample);
        hists.push_back(hst);
      }
      else{ 
        //DisplayText("Could not read hst for: " + sample + "_" + subsample, 31);
	continue;
      }
    }
    if (!hists.empty()) hist_collection.push_back(merge_and_decorate(hists, scfg->latex_name, scfg->color));
    else DisplayText("Warning: Hist collection empty: "+sample, 31);
  }
  
  // Remove nulls if any:
  vector<TH1D*> cleaned;
  for (auto* h : hist_collection) {
    if (h) cleaned.push_back(h);
    else DisplayText("Removed null histogram", 31);
  }
  return cleaned;
}

void combine_hists(vector<TH1D *> &hst_collection, vector<TString> innames, TString outname, int color) {
  vector<TH1D *> to_combine;
  vector<TString> match;

  //Searching for matching backgrounds:
  for (auto *hist : hst_collection) {
    TString name = hist->GetName();
    for (const auto &target : innames) {
      if (name == target) {
	to_combine.push_back(hist);
	match.push_back(name);
      }
    }
  }
  if (to_combine.empty()) return;

  cout << "\033[33;3m==> Combining : " << outname << " = ";
  for (size_t i = 0; i < match.size(); ++i) {
    cout << match[i]; if (i != match.size() - 1) cout << " + ";
  }
  cout <<"\033[0m"<< endl;

  TH1D *combined = (TH1D *)to_combine[0]->Clone(outname);
  combined->SetDirectory(0);
  combined->SetName(outname);

  for (size_t i = 1; i < to_combine.size(); ++i) combined->Add(to_combine[i]);
  SetHistoStyle(combined, color);
  
  //cout << "Replacing the following hists: ";
  //for (const auto &h : to_combine) cout << h->GetName() << " ";
  //cout << "\nWith: " << outname << endl;

   hst_collection.erase(remove_if(hst_collection.begin(), hst_collection.end(),
                                 [&](TH1D *h) {
                                   for (const auto &target : innames)
                                     if (TString(h->GetName()) == target) return true;
                                   return false;
                                 }),
                       hst_collection.end());
  
  hst_collection.push_back(combined);
}
  
#endif // READ_HISTS_H
