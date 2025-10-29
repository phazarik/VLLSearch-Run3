#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <array> 
#include <memory>
#include <algorithm>
#include <TString.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TSystem.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "CMSdecorations.h"
using namespace std;

int ind, indplt;
struct LimitData {
  double mass; double obs; double exp; double exp_1u;
  double exp_1d; double exp_2u; double exp_2d; double theory;
};
struct plotinfo {TString filename; TString modelname; TString campaign; TString channel;};
vector<LimitData> ReadDataFromFile(const TString& filename);
TGraph* ReadTheoryGraph(const TString& infile, const TString &modelname);
string todays_date();

struct plotdata {
  TString modelname;
  TString finalstate;
  TString campaign;
  TString channel;
  TString energy;
};

void makeOneLimitPlot(
		      TString infile   = "limits_sigmaB_13TeV/sigmaB_VLLD_mu_Run3_combined.txt",
		      TString outfile  = "test_run3_vlldmu",
		      TString modelname= "VLLD-#mu",
		      TString campaign = "Run3",
		      TString channel  = "combined",
		      TString energy   = "13p6TeV",
		      TString final_state = "2L",
		      float ymin       = 1e-4,
		      float ymax       = 1e3,
		      bool systematics = false,
		      bool bottomleft  = false
		      )
{
  ind += 1;
  TString cname = Form("c_%d", ind);
  bool showdata = false;
  bool tosave = true;

  //Quick fix:
  if(modelname == "VLLD_mu")  modelname= "VLLD-#mu";
  if(modelname == "VLLD_ele") modelname= "VLLD-e";
  //-----------------------------------------------------------------------------------------
  // Loading data from input file in the following format:
  // Mass >> Observed >> Exp_2sigDn >> Exp_1sigDn >> Exp_nominal >> Exp_1sigUp >> Exp_2sigUp
  //-----------------------------------------------------------------------------------------
  vector<LimitData> limits = ReadDataFromFile(infile); if(limits.empty()) return;
  int nPoints = limits.size();
  if (nPoints < 3) {
    cout << "\033[31;3mInsufficient points (" << nPoints << ") in file: " << infile.Data();
    cout << "; Skipping plot generation.\033[0m" << std::endl;
    return;
  }
  float xmin = limits[0].mass;
  float xmax = limits[nPoints-1].mass;
  gStyle->Reset(); //important
  gStyle->SetOptStat(0);

  //---------------------------
  // Drawing on cancas:
  //---------------------------
  TCanvas* c1 = CreateCanvas(cname); c1->Clear();

  // Filter limit values
  vector<LimitData> filteredLimits;
  int minmass = 100; int maxmass = 1200; if(final_state=="2LOS") minmass=125;
  for (const auto& l : limits) { if (minmass <= l.mass && l.mass <= maxmass) filteredLimits.push_back(l);}
  limits = filteredLimits;
  nPoints = limits.size();
  if (nPoints == 0) return;

  //Define graphs:
  TGraph* exp    = new TGraph(nPoints);
  TGraph* obs    = new TGraph(nPoints);
  TGraph* yellow = new TGraph(2 * nPoints);
  TGraph* green  = new TGraph(2 * nPoints);

  // Apply styles:
  Int_t color_green  = kOrange;  //TColor::GetColor(13, 203, 7);
  Int_t color_yellow = kGreen+1; //TColor::GetColor(239, 201, 7);
  StyleGraph(exp,    kBlack, 7,2,cname);
  StyleGraph(obs,    kBlack, 1,2,cname); obs->SetMarkerStyle(20);
  StyleUncertaintyBand(yellow, color_green);
  StyleUncertaintyBand(green, color_yellow);

  //Filling the graphs:
  for (int i = 0; i < nPoints; ++i) {
    double mass   = limits[i].mass;
    double expval = limits[i].exp;    double obsval = limits[i].obs;
    double exp_1u = limits[i].exp_1u; double exp_1d = limits[i].exp_1d;
    double exp_2u = limits[i].exp_2u; double exp_2d = limits[i].exp_2d;
    exp   ->SetPoint(i, mass, expval);
    obs   ->SetPoint(i, mass, obsval);
    green ->SetPoint(i, mass, exp_1u);
    yellow->SetPoint(i, mass, exp_2u);
    green ->SetPoint(2*nPoints-1-i, mass, exp_1d);
    yellow->SetPoint(2*nPoints-1-i, mass, exp_2d);
  }

  //Get theory graphs:
  TString modelname_doublet = modelname;
  TString modelname_singlet = modelname;
  modelname_singlet.ReplaceAll("VLLD", "VLLS");
  TGraph* theory_vlld = ReadTheoryGraph("xsec/sigmaB_VLLD_" + energy + ".txt", modelname_doublet);
  TGraph* theory_vlls = ReadTheoryGraph("xsec/sigmaB_VLLS_" + energy + ".txt", modelname_singlet);

  // Drawing one canvas
  SetAxisTitlesAndRange(theory_vlld, ymin, ymax, 0, 1400); theory_vlld->Draw("AL"); //This decides the decorations
  yellow->Draw("F same");
  green ->Draw("F same");
  exp   ->Draw("L same");
  if(showdata) obs->Draw("LP same");
  theory_vlld->Draw("L same");
  theory_vlls->Draw("L same");
  
  c1->Update();

  // Create and draw legend
  TLegend* legend = CreateLegend(bottomleft);
  if(showdata) legend->AddEntry(obs, "Observed", "lp");
  legend->AddEntry(exp,    "Expected", "l");
  legend->AddEntry(yellow, "68% Expected", "f");
  legend->AddEntry(green,  "95% Expected", "f");
  legend->AddEntry(theory_vlld, modelname_doublet, "l");
  legend->AddEntry(theory_vlls, modelname_singlet, "l");
  legend->Draw();

  // Add CMS label
  AddCMSLabel(c1, campaign, channel, energy, final_state, bottomleft, systematics);;
  c1->Update();
  
  if(tosave){
    c1->SaveAs(outfile+".svg");
    c1->SaveAs(outfile+".png");
  }
  //delete c1;
  indplt += 1;
}

void step7_makeBrazilianPlots(){

  vector<plotdata> limitfiles = {
    //mu-like models:
    {"VLLD_mu",  "2L",   "FullDataset", "combined", "13TeV"},
    {"VLLD_mu",  "2L",   "FullDataset", "combined", "13p6TeV"},
    {"VLLD_mu",  "2LSS", "FullDataset", "combined", "13TeV"},
    {"VLLD_mu",  "2LSS", "FullDataset", "combined", "13p6TeV"},
    {"VLLD_mu",  "2LOS", "FullDataset", "combined", "13TeV"},
    {"VLLD_mu",  "2LOS", "FullDataset", "combined", "13p6TeV"},
    //e-like models:
    {"VLLD_ele",  "2L",   "FullDataset", "combined", "13TeV"},
    {"VLLD_ele",  "2L",   "FullDataset", "combined", "13p6TeV"},
    {"VLLD_ele",  "2LSS", "FullDataset", "combined", "13TeV"},
    {"VLLD_ele",  "2LSS", "FullDataset", "combined", "13p6TeV"},
    {"VLLD_ele",  "2LOS", "FullDataset", "combined", "13TeV"},
    {"VLLD_ele",  "2LOS", "FullDataset", "combined", "13p6TeV"},
  };

  for(int i=0; i<(int)limitfiles.size(); i++){
    TString modelname   = limitfiles[i].modelname;
    TString finalstate  = limitfiles[i].finalstate;
    TString campaign    = limitfiles[i].campaign;
    TString channel     = limitfiles[i].channel;
    TString energy      = limitfiles[i].energy;
    TString outname     = "limits_"+modelname+"_"+finalstate+"_"+energy; 
    TString infile = "limits_sigmaB_"+energy+"/sigmaB_"+finalstate+"_"+modelname+"_"+campaign+"_"+channel+".txt";
    cout<<"Processing .. "<<infile<<endl;
    makeOneLimitPlot(infile, "plots/"+outname, modelname, campaign, channel, energy, finalstate);
  }
  
}

//_____________________________________________________________________________________
//_____________________________________________________________________________________

vector<LimitData> ReadDataFromFile(const TString& infile) {
  ifstream txtfile(infile.Data());
  if (!txtfile.is_open()) {
    cerr << "\033[31;1mError: Unable to open txtfile " << infile.Data() <<"\033[0m"<< endl;
    return {};
  }
  vector<LimitData> limits;
  string line;  
  getline(txtfile, line); //Skipping header
  while (getline(txtfile, line)) {
    istringstream iss(line);
    double mass_, obs_, exp_2down_, exp_1down_, exp_, exp_1up_, exp_2up_, theory_;
    if (!(iss >> mass_ >> obs_ >> exp_2down_ >> exp_1down_ >> exp_ >> exp_1up_ >> exp_2up_ >> theory_)) {
      cerr << "Warning: Skipping line: " << line << endl;
      continue;
    }
    limits.push_back({mass_, obs_, exp_, exp_1up_, exp_1down_, exp_2up_, exp_2down_, theory_});
  }
  txtfile.close();
  return limits;
}
string todays_date(){
  string processline = "date +%Y-%m-%d";
  array<char, 128> buffer;
  string result;
  unique_ptr<FILE, decltype(&pclose)> pipe(popen(processline.c_str(), "r"), pclose);
  if(!pipe) throw runtime_error("Failed to run Bash script.");
  while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) result += buffer.data();
  while(!result.empty() && (result.back() == '\n' || result.back() == '\r')) result.pop_back();
  return result;
}
TGraph* ReadTheoryGraph(const TString& infile, const TString &modelname) {
    ifstream fin(infile.Data());
    if (!fin.is_open()) {
        cerr << "Error: cannot open " << infile << endl;
        return nullptr;
    }
    vector<double> mass, xsec;
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        double m, xs;
        if (!(iss >> m >> xs)) continue;
        //if (m > 800) continue;  // skip points beyond 800 GeV
        mass.push_back(m);
        xsec.push_back(xs);
    }
    fin.close();
    int n = mass.size();
    if (n == 0) {
        cerr << "No data in " << infile << endl;
        return nullptr;
    }
    TGraph* gTheory = new TGraph(n);
    for (int i = 0; i < n; ++i) gTheory->SetPoint(i, mass[i], xsec[i]);
    gTheory->SetLineColor(kRed);
    gTheory->SetLineWidth(2);
    gTheory->SetMarkerStyle(20);
    if (modelname.Contains("VLLS")) gTheory->SetLineStyle(2); // dotted line for VLLS
    gTheory->SetMarkerColor(kRed);
    return gTheory;
}
