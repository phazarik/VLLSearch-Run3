#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib> // Required for running shell script
#include <array>   
#include <memory> // for keeping the output of the shell script

struct database{
  TString model;
  TString flavor;
  TString mass;
  int color;
  TH1F *hist;
};

vector<database> inputs = {
  {.model="VLLS", .flavor="ele", .mass="100",  .color=kRed},
  {.model="VLLS", .flavor="ele", .mass="125",  .color=kOrange},
  {.model="VLLS", .flavor="ele", .mass="150",  .color=kYellow},
  {.model="VLLS", .flavor="ele", .mass="200",  .color=kGreen},
  {.model="VLLS", .flavor="ele", .mass="250",  .color=kSpring-1},
  {.model="VLLS", .flavor="ele", .mass="300",  .color=kCyan},
  {.model="VLLS", .flavor="ele", .mass="350",  .color=kAzure-2},
  {.model="VLLS", .flavor="ele", .mass="400",  .color=kBlue},
  {.model="VLLS", .flavor="ele", .mass="450",  .color=kViolet},
  {.model="VLLS", .flavor="ele", .mass="500",  .color=kMagenta-9},
  {.model="VLLS", .flavor="ele", .mass="750",  .color=kPink-4},
  {.model="VLLS", .flavor="ele", .mass="1000", .color=kBlack},

  {.model="VLLS", .flavor="mu", .mass="100",   .color=kRed},
  {.model="VLLS", .flavor="mu", .mass="125",   .color=kOrange},
  {.model="VLLS", .flavor="mu", .mass="150",   .color=kYellow},
  {.model="VLLS", .flavor="mu", .mass="200",   .color=kGreen},
  {.model="VLLS", .flavor="mu", .mass="250",   .color=kSpring-1},
  {.model="VLLS", .flavor="mu", .mass="300",   .color=kCyan},
  //{.model="VLLS", .flavor="mu", .mass="350",   .color=kAzure-2},
  {.model="VLLS", .flavor="mu", .mass="400",   .color=kBlue},
  {.model="VLLS", .flavor="mu", .mass="450",   .color=kViolet},
  {.model="VLLS", .flavor="mu", .mass="500",   .color=kMagenta-9},
  {.model="VLLS", .flavor="mu", .mass="750",   .color=kPink-4},
  {.model="VLLS", .flavor="mu", .mass="1000",  .color=kBlack},

  {.model="VLLD", .flavor="ele", .mass="100",  .color=kRed},
  {.model="VLLD", .flavor="ele", .mass="200",  .color=kOrange},
  {.model="VLLD", .flavor="ele", .mass="300",  .color=kYellow},
  {.model="VLLD", .flavor="ele", .mass="400",  .color=kGreen},
  {.model="VLLD", .flavor="ele", .mass="600",  .color=kSpring-1},
  {.model="VLLD", .flavor="ele", .mass="800",  .color=kCyan},
  {.model="VLLD", .flavor="ele", .mass="1000", .color=kAzure-2},
  {.model="VLLD", .flavor="ele", .mass="1200", .color=kBlue},
  {.model="VLLD", .flavor="ele", .mass="1400", .color=kViolet},
  {.model="VLLD", .flavor="ele", .mass="1600", .color=kMagenta-9},
  {.model="VLLD", .flavor="ele", .mass="1800", .color=kPink-4},
  {.model="VLLD", .flavor="ele", .mass="2000", .color=kGray+2},
  {.model="VLLD", .flavor="ele", .mass="2200", .color=kBlack},

  {.model="VLLD", .flavor="mu", .mass="100",   .color=kRed},
  {.model="VLLD", .flavor="mu", .mass="200",   .color=kOrange},
  {.model="VLLD", .flavor="mu", .mass="300",   .color=kYellow},
  {.model="VLLD", .flavor="mu", .mass="400",   .color=kGreen},
  {.model="VLLD", .flavor="mu", .mass="600",   .color=kSpring-1},
  {.model="VLLD", .flavor="mu", .mass="800",   .color=kCyan},
  //{.model="VLLD", .flavor="mu", .mass="1000",  .color=kAzure-2},
  {.model="VLLD", .flavor="mu", .mass="1200",   .color=kBlue},
  {.model="VLLD", .flavor="mu", .mass="1400",   .color=kViolet},
  {.model="VLLD", .flavor="mu", .mass="1600",   .color=kMagenta-9},
  {.model="VLLD", .flavor="mu", .mass="1800",   .color=kPink-4},
  {.model="VLLD", .flavor="mu", .mass="2000",   .color=kGray+2},
  {.model="VLLD", .flavor="mu", .mass="2200",   .color=kBlack}
};


vector<database> read_plots(vector<database>inputs, TString plotname){
  vector<database> hists;
  hists.clear();
  
  for(int i=0; i<(int)inputs.size(); i++){
    database temp;
    TString filename = "input_files/hst_"+inputs[i].model+"_"+inputs[i].flavor+"_M"+inputs[i].mass+".root";
    //cout<<filename<<endl;
    TFile *tfile = new TFile(filename);
    TH1F *histogram = (TH1F *)tfile->Get(plotname);
    if(histogram == nullptr) cout<<"Histogram not found for "+inputs[i].model+"_"+inputs[i].flavor+"_M"+inputs[i].mass+".root";
    else{
      temp.model  = inputs[i].model;
      temp.flavor = inputs[i].flavor;
      temp.mass   = inputs[i].mass;
      temp.color  = inputs[i].color;
      temp.hist   = histogram;
      hists.push_back(temp);
    }
  }
  return hists;
}

TLegend *create_legend(){
  //TLegend *lg1 = new TLegend(0.85, 0.45, 0.75,0.86);
  TLegend *lg1 = new TLegend(0.78, 0.40, 0.95, 0.88);
  lg1->SetTextFont(62);		
  lg1->SetFillStyle(0);
  lg1->SetBorderSize(0);
  lg1->SetTextSize(0.025);
  return lg1;
}

void SetHistoStyle(TH1F *h, TString plotname, TString plottitle, int color){
  h->SetLineColor(color);
  h->SetLineWidth(2);
  h->SetMarkerStyle(20);
  h->SetMarkerSize(1.2);
  h->SetTitle(plottitle);

  //Y-axis
  h->GetYaxis()->SetTitle("Events (normalized)");
  h->GetYaxis()->SetTitleFont(43);
  h->GetYaxis()->SetTitleSize(18);
  h->GetYaxis()->SetTitleOffset(1.7);
  h->GetYaxis()->SetLabelFont(43);
  h->GetYaxis()->SetLabelSize(15);
  h->GetYaxis()->SetLabelOffset(0.02);
  h->GetYaxis()->CenterTitle();
  //h->GetYaxis()->SetTickLength(-0.02);

  //X-axis
  h->GetXaxis()->SetTitle(plotname);
  h->GetXaxis()->SetTitleFont(43);
  h->GetXaxis()->SetTitleSize(18);
  h->GetXaxis()->SetTitleOffset(1.5);
  h->GetXaxis()->SetLabelFont(43);
  h->GetXaxis()->SetLabelSize(15);
  h->GetXaxis()->SetLabelOffset(0.02);
  //h->GetXaxis()->CenterTitle();
  //h->GetXaxis()->SetTickLength(-0.02);
}

bool ifexists(TString file_){
  std::string filepath = (std::string)file_;
  std::ifstream file(filepath);
  return file.good();
}

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
