#ifndef SETTINGS_H
#define SETTINGS_H

#include "decorations.h"

//Global variables being used by the class:
extern TString input_path;
extern int nbins;
extern float xmin;
extern float xmax;
extern int rebin;
extern float globalSbyB;

//The following function are used by get_hist()
extern TTree* GetFilteredTree(TTree *intree); //defined in the main code

void SetLastBinAsOverflow(TH1F *hst){    
  int lastBin = hst->GetNbinsX();
  double content  = hst->GetBinContent(lastBin);
  double error    = hst->GetBinError(lastBin);
  double overflow = hst->GetBinContent(lastBin + 1);

  double updated_content = content+overflow;
  double updated_error = std::sqrt(error*error + overflow*overflow);
  
  hst->SetBinContent(lastBin, updated_content);
  hst->SetBinError(lastBin, updated_error);
}

bool file_exists(TString filename){
  std::string filepath = (std::string)filename;
  std::ifstream file(filepath);
  return file.good();
}

void DisplayText(TString text, int color){
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

//--------------------------------------------------------------------
// Reading the branches into histograms and scaling them appropriately
//--------------------------------------------------------------------

TH1F *get_hist(
	       const TString& var,
	       const TString& sample,
	       const TString& subsample,
	       const float& lumi
	       ){

  TH1F *hst = new TH1F("tmphst", "tmphst", nbins, xmin, xmax);
  hst->Rebin(rebin);
  
  //Accessing the file:
  TString filename = input_path+"/"+"tree_"+sample+"_"+subsample+".root";
  if(!file_exists(filename)){
    DisplayText("Not found: "+filename, 31); //31 is the ANSI color code for red
    delete hst;
    return nullptr;
  }

  TFile *file = new TFile(filename, "READ");

  //Event-wise filtering:
  /*
  TTree *tree = (TTree *)file->Get("myEvents");
  if(!tree) DisplayText("Tree not found for file : "+filename, 31);
  Float_t lep0_pt; tree->SetBranchAddress("lep0_pt", &lep0_pt);
  Float_t HT;      tree->SetBranchAddress("HT", &HT);

  //Setting up the map
  std::map<TString, Float_t*> varMap; 
  varMap["lep0_pt"] = &lep0_pt;
  varMap["HT"] = &HT;

  Float_t myvar;
  
  //Event loop:
  Long64_t nevt = tree->GetEntries();
  for (Long64_t i = 0; i < nevt; i++) {
    tree->GetEntry(i);

    //Cuts are defined here.
    bool cuts = lep0_pt > 30;
    
    if(cuts){
      myvar = *varMap[var];
      hst->Fill(myvar);
    }
    }*/

  //Alternative filtering:
  TTree *intree = (TTree *)file->Get("myEvents");
  TTree *tree = GetFilteredTree(intree);

  Float_t myvar; tree->SetBranchAddress(var, &myvar);

  //Event loop:
  Long64_t nevt = tree->GetEntries();
  for (Long64_t i = 0; i < nevt; i++) {
    tree->GetEntry(i);
    hst->Fill(myvar);
  }
  
  //cout<<"mean = "<<hst->GetMean()<<endl;;
  cout<<sample<<"_"<<subsample<<" Raw: "<<hst->Integral()<<endl;

  //Tweaking the histogram:
  SetLastBinAsOverflow(hst);
  hst->Scale(59800/lumi);

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
  for(int i=0; i<(int)sample.size(); i++){if(sample[i]) hist->Add(sample[i]);}
  SetHistoStyle(hist, color);
  hist->SetName(samplename);

  if(hist) return hist;
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
      hist->SetFillColor(hist->GetLineColor());
      hist->SetLineColor(kBlack);
    }
  }
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
  TH1F * srb = (TH1F *)sig->Clone("copy");
  if(!rootb){
    cout<<"Error: Background is null!"<<endl;
    srb = nullptr;
  }
  else srb->Divide(rootb);

  //print global S/sqrtB
  float nsig = sig->Integral();
  float nbkg = 0; for(int i=0; i<(int)bkg.size(); i++) nbkg = nbkg + bkg[i]->Integral();
  float sqrtB = sqrt(nbkg);
  globalSbyB = nsig/sqrtB;
  cout<<"Global S/sqrt{B} = "<<globalSbyB<<endl;
  
  return srb;
}

#endif // SETTINGS_H