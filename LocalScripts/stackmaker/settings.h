#ifndef SETTINGS_H
#define SETTINGS_H

#include "decorations.h"

//Global variables being used by the class:
extern TString input_path;
extern int nbins;
extern float xmin;
extern float xmax;

void SetLastBinAsOverflow(TH1F *hst){    
  int lastBin = hst->GetNbinsX();
  double lastBinContent = hst->GetBinContent(lastBin);
  double lastBinError = hst->GetBinError(lastBin);
  double overflow = hst->GetBinContent(lastBin + 1);
  hst->SetBinContent(lastBin, lastBinContent + overflow);
}

bool file_exists(TString filename){
  std::string filepath = (std::string)filename;
  std::ifstream file(filepath);
  return file.good();
}

//--------------------------------------------------------------------
// Reading the branches into histograms and scaling them appropriately
//--------------------------------------------------------------------
TH1F *get_hist(
	       const TString& var,
	       const TString& sample,
	       const TString& subsample,
	       const float& lumi
	       ){

  TH1F *hst = new TH1F("temp", "temp", nbins, xmin, xmax);

  //Accessing the file:
  TString filename = input_path+"/"+"tree_"+sample+"_"+subsample+".root";
  if(!file_exists(filename)){cout<<"Not found: "<<filename<<endl; return nullptr;}
  TFile *file = new TFile(filename, "READ");

  //Accessing the branch and filling the histogram:
  TTree* tree = (TTree *)file->Get("myEvents");    
  TBranch *br = tree->GetBranch(var);
  int nevt = (int)tree->GetEntries(); //Events
  Float_t branchData; br->SetAddress(&branchData); //This stores the data event-wise.
  for (int i=0; i < nevt; i++) { 
    tree->GetEntry(i);
    hst->Fill(branchData);
  }    

  //Tweaking the histogram:
  //SetLastBinAsOverflow(hst);
  hst->Scale(59800/lumi);

  cout<<"Success reading : "<<sample<<" "<<subsample<<endl;
  cout<<"Integral = "<<hst->Integral()<<endl;

  if(!hst) cout<<"Warning : nullhist for "<<sample<<"_"<<subsample<<endl;

  //To avoid memory leak, I am deleting the file as well as the hst.
  //Before that, I am cloning and returning a different hist. 
  file->Close(); delete file;
  TH1F *result = (TH1F *)hst->Clone();
  result->SetName(TString(var));
  delete hst;

  return result;
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

TH1F* merge_and_decorate(vector<TH1F*>sample, TString var, TString name, int color) {
  TH1F *hist = (TH1F *)sample[0]->Clone();
  for(int i=1; i<(int)sample.size(); i++) hist->Add(sample[i]);
  SetHistoStyle(hist, var, name, color);
  return hist;
}

bool compareHists(const TH1F* a, const TH1F* b) {
    return a->Integral() < b->Integral();
}

void setFillColorFromLineColor(THStack *stack) {
  TList *histList = stack->GetHists();
  TIter next(histList);
  TObject *obj;
  //iterating over the hists
  while ((obj = next())) {
    if (obj->InheritsFrom("TH1")) {
      TH1 *hist = static_cast<TH1*>(obj);
      hist->SetFillColor(hist->GetLineColor());
      hist->SetLineColor(kBlack);
    }
  }
}

#endif // SETTINGS_H
