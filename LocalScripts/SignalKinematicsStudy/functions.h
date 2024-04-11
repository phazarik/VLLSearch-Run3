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
    //if(histogram == nullptr) cout<<"Histogram not found for "+inputs[i].model+"_"+inputs[i].flavor+"_M"+inputs[i].mass+".root -- ";
    if(histogram != nullptr){
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

TCanvas *create_canvas(TString plotname){
  TCanvas *c1 = new TCanvas(plotname,plotname,800,600);
  c1->SetGrid();
  c1->SetFrameLineWidth(2);
  //c1->SetTopMargin(0.05);
  c1->SetBottomMargin(0.12);
  c1->SetLeftMargin(0.12);
  c1->SetRightMargin(0.15);
  return c1;
}

TLegend *create_legend(){
  TLegend *lg1 = new TLegend(0.87, 0.45, 0.95, 0.90);
  lg1->SetTextFont(62);		
  lg1->SetFillStyle(0);
  lg1->SetBorderSize(0);
  lg1->SetTextSize(0.025);
  return lg1;
}

void SetHistoStyle(TH1F *h, TString xtitle, TString plottitle, int color){
  h->SetLineColor(color);
  h->SetLineWidth(2);
  h->SetMarkerStyle(20);
  h->SetMarkerSize(1.2);
  h->SetTitle("");
  //Y-axis
  //h->GetYaxis()->SetRangeUser(0, 1); //This is overruled by the tallest hist.
  h->GetYaxis()->SetTitle("Events (normalized)");
  h->GetYaxis()->SetTitleSize(0.04);
  h->GetYaxis()->SetTitleOffset(1.38);
  h->GetYaxis()->CenterTitle();
  h->GetYaxis()->SetNdivisions(606);
  h->GetYaxis()->SetLabelSize(0.04);
  //X-axis
  h->GetXaxis()->SetTitle(xtitle);
  h->GetXaxis()->SetTitleSize(0.04);
  h->GetXaxis()->SetTitleOffset(1.15);
  h->GetXaxis()->CenterTitle();
  h->GetXaxis()->SetNdivisions(606);
  //h->GetXaxis()->SetDecimals();
  h->GetXaxis()->SetLabelSize(0.04);
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

void put_text(TString model, TString flavor){
  
  TText* cmsText = new TText(0.12, 0.93, "CMS");
  cmsText->SetTextSize(0.06);
  cmsText->SetNDC();
  cmsText->SetTextFont(62); // Bold
  cmsText->Draw();
  
  TText* preliminaryText = new TText(0.22, 0.93, "preliminary");
  preliminaryText->SetTextSize(0.05);
  preliminaryText->SetNDC();
  preliminaryText->SetTextFont(52); // Italics
  preliminaryText->Draw();

  TString modeltext = model + " " + flavor;
  TText* modelText = new TText(0.72, 0.93, modeltext);
  modelText->SetTextSize(0.05);
  modelText->SetNDC();
  modelText->SetTextFont(42); // Bold
  modelText->Draw();
  /*
  TLatex* latex = new TLatex(); 
  latex->SetTextFont(42);
  latex->SetTextSize(0.04);
  latex->SetNDC();
  latex->DrawLatex(0.71, 0.93, "(2018) 59.8 fb^{-1}");*/
  
}
