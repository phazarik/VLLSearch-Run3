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
#include "CMSdecorations.h"
using namespace std;

int ind, indplt;
struct LimitData {
  double mass; double obs; double exp; double exp_1u;
  double exp_1d; double exp_2u; double exp_2d; double theory;
};
struct plotinfo {TString filename; TString modelname; TString campaign; TString channel;};
vector<LimitData> ReadDataFromFile(const TString& filename);
void makeOneLimitPlot(TString infile, TString outfile, TString modelname, TString campaign, TString channel, float ymin, float ymax, bool bottomleft);
string todays_date();

void makeBrazilianPlots(){

  TString timetag = "2025-07-04";
  TString indir   = gSystem->ConcatFileName("sigmaBlimits", timetag);
  TString outdir = gSystem->ConcatFileName("plots", todays_date().c_str());
  gSystem->mkdir(outdir, true);

  ind=0; indplt=0;
  vector<plotinfo> limitdict = {
    //e-type:
    {"sigmaB_VLLD_ele_2016preVFP_UL_ee.txt", "VLLD-e", "2016preVFP_UL", "ee"},
    {"sigmaB_VLLD_ele_2016preVFP_UL_em.txt", "VLLD-e", "2016preVFP_UL", "e#mu"},
    {"sigmaB_VLLD_ele_2016preVFP_UL_me.txt", "VLLD-e", "2016preVFP_UL", "#mue"},
    {"sigmaB_VLLD_ele_2016preVFP_UL_mm.txt", "VLLD-e", "2016preVFP_UL", "#mu#mu"},
    {"sigmaB_VLLD_ele_2016postVFP_UL_ee.txt", "VLLD-e", "2016postVFP_UL", "ee"},
    {"sigmaB_VLLD_ele_2016postVFP_UL_em.txt", "VLLD-e", "2016postVFP_UL", "e#mu"},
    {"sigmaB_VLLD_ele_2016postVFP_UL_me.txt", "VLLD-e", "2016postVFP_UL", "#mue"},
    {"sigmaB_VLLD_ele_2016postVFP_UL_mm.txt", "VLLD-e", "2016postVFP_UL", "#mu#mu"},
    {"sigmaB_VLLD_ele_2017_UL_ee.txt", "VLLD-e", "2017_UL", "ee"},
    {"sigmaB_VLLD_ele_2017_UL_em.txt", "VLLD-e", "2017_UL", "e#mu"},
    {"sigmaB_VLLD_ele_2017_UL_me.txt", "VLLD-e", "2017_UL", "#mue"},
    {"sigmaB_VLLD_ele_2017_UL_mm.txt", "VLLD-e", "2017_UL", "#mu#mu"},
    {"sigmaB_VLLD_ele_2018_UL_ee.txt", "VLLD-e", "2018_UL", "#ee"},
    {"sigmaB_VLLD_ele_2018_UL_em.txt", "VLLD-e", "2018_UL", "e#mu"},
    {"sigmaB_VLLD_ele_2018_UL_me.txt", "VLLD-e", "2018_UL", "#mue"},
    {"sigmaB_VLLD_ele_2018_UL_mm.txt", "VLLD-e", "2018_UL", "#mu#mu"},
    {"sigmaB_VLLD_ele_combined_Run2.txt", "VLLD-e",   "Run2_UL", "combined"},

    {"sigmaB_VLLD_ele_Run3Summer22_ee.txt", "VLLD-e", "Run3Summer22", "#ee"},
    {"sigmaB_VLLD_ele_Run3Summer22_em.txt", "VLLD-e", "Run3Summer22", "e#mu"},
    {"sigmaB_VLLD_ele_Run3Summer22_me.txt", "VLLD-e", "Run3Summer22", "#mue"},
    {"sigmaB_VLLD_ele_Run3Summer22_mm.txt", "VLLD-e", "Run3Summer22", "#mu#mu"},
    {"sigmaB_VLLD_ele_Run3Summer22EE_ee.txt", "VLLD-e", "Run3Summer22EE", "#ee"},
    {"sigmaB_VLLD_ele_Run3Summer22EE_em.txt", "VLLD-e", "Run3Summer22EE", "e#mu"},
    {"sigmaB_VLLD_ele_Run3Summer22EE_me.txt", "VLLD-e", "Run3Summer22EE", "#mue"},
    {"sigmaB_VLLD_ele_Run3Summer22EE_mm.txt", "VLLD-e", "Run3Summer22EE", "#mu#mu"},
    {"sigmaB_VLLD_ele_combined_2022.txt", "VLLD-e", "2022", "combined"},
    
    //mu-type:
    {"sigmaB_VLLD_mu_2016preVFP_UL_ee.txt", "VLLD-#mu", "2016preVFP_UL", "ee"},
    {"sigmaB_VLLD_mu_2016preVFP_UL_em.txt", "VLLD-#mu", "2016preVFP_UL", "e#mu"},
    {"sigmaB_VLLD_mu_2016preVFP_UL_me.txt", "VLLD-#mu", "2016preVFP_UL", "#mue"},
    {"sigmaB_VLLD_mu_2016preVFP_UL_mm.txt", "VLLD-#mu", "2016preVFP_UL", "#mu#mu"},
    {"sigmaB_VLLD_mu_2016postVFP_UL_ee.txt", "VLLD-#mu", "2016postVFP_UL", "ee"},
    {"sigmaB_VLLD_mu_2016postVFP_UL_em.txt", "VLLD-#mu", "2016postVFP_UL", "e#mu"},
    {"sigmaB_VLLD_mu_2016postVFP_UL_me.txt", "VLLD-#mu", "2016postVFP_UL", "#mue"},
    {"sigmaB_VLLD_mu_2016postVFP_UL_mm.txt", "VLLD-#mu", "2016postVFP_UL", "#mu#mu"},
    {"sigmaB_VLLD_mu_2017_UL_ee.txt",     "VLLD-#mu", "2017_UL", "ee"},
    {"sigmaB_VLLD_mu_2017_UL_em.txt",     "VLLD-#mu", "2017_UL", "e#mu"},
    {"sigmaB_VLLD_mu_2017_UL_me.txt",     "VLLD-#mu", "2017_UL", "#mue"},
    {"sigmaB_VLLD_mu_2017_UL_mm.txt",     "VLLD-#mu", "2017_UL", "#mu#mu"},
    {"sigmaB_VLLD_mu_2018_UL_ee.txt",     "VLLD-#mu", "2017_UL", "ee"},
    {"sigmaB_VLLD_mu_2018_UL_em.txt",     "VLLD-#mu", "2018_UL", "e#mu"},
    {"sigmaB_VLLD_mu_2018_UL_me.txt",     "VLLD-#mu", "2018_UL", "#mue"},
    {"sigmaB_VLLD_mu_2018_UL_mm.txt",     "VLLD-#mu", "2018_UL", "#mu#mu"},
    {"sigmaB_VLLD_mu_combined_Run2.txt", " VLLD-#mu", "Run2_UL", "combined"},

    {"sigmaB_VLLD_mu_Run3Summer22_ee.txt", "VLLD-#mu", "Run3Summer22", "#ee"},
    {"sigmaB_VLLD_mu_Run3Summer22_em.txt", "VLLD-#mu", "Run3Summer22", "e#mu"},
    {"sigmaB_VLLD_mu_Run3Summer22_me.txt", "VLLD-#mu", "Run3Summer22", "#mue"},
    {"sigmaB_VLLD_mu_Run3Summer22_mm.txt", "VLLD-#mu", "Run3Summer22", "#mu#mu"},
    {"sigmaB_VLLD_mu_Run3Summer22EE_ee.txt", "VLLD-#mu", "Run3Summer22EE", "#ee"},
    {"sigmaB_VLLD_mu_Run3Summer22EE_em.txt", "VLLD-#mu", "Run3Summer22EE", "e#mu"},
    {"sigmaB_VLLD_mu_Run3Summer22EE_me.txt", "VLLD-#mu", "Run3Summer22EE", "#mue"},
    {"sigmaB_VLLD_mu_Run3Summer22EE_mm.txt", "VLLD-#mu", "Run3Summer22EE", "#mu#mu"},
    {"sigmaB_VLLD_mu_combined_2022.txt", "VLLD-#mu", "2022", "combined"},
  };

  

  for (size_t i = 0; i < limitdict.size(); i++) {
    TString infile   = gSystem->ConcatFileName(indir,  limitdict[i].filename);
    if (gSystem->AccessPathName(infile, kFileExists)) {
      cout << "Skipping absent file: " << infile << endl;
      continue;
    }

    TString out = limitdict[i].filename; out.ReplaceAll("sigmaB", "limit"); out.ReplaceAll(".txt", "");
    TString outfile   = gSystem->ConcatFileName(outdir, out);
    TString channel   = limitdict[i].channel;
    if (limitdict[i].channel != "combined") channel += " channel";
    TString modelname = limitdict[i].modelname;
    TString campaign  = limitdict[i].campaign;
    float ymin = 10e-4; float ymax = 10e3;
    bool bottomleft = false;

    //Exceptions:
    /*
    if (infile.Contains("VLLD_ele_2017_UL_mm") ||
        infile.Contains("VLLD_mu_2017_UL_ee") ||
        infile.Contains("VLLD_mu_2018_UL_ee")) {
        bottomleft = true;
	}*/

    makeOneLimitPlot(infile, outfile, modelname, campaign, channel, ymin, ymax, bottomleft);
    //break;
  }
  cout<<"Done! Total images created = "<<indplt<<"\n"<<endl;  
}

void makeOneLimitPlot(
		   TString infile="sigmaBlimits/2025-04-16/sigmaB_VLLD_mu_combined_2017_2018.txt",
		   TString outfile="test.png",
		   TString modelname="VLLD-#mu",
		   TString campaign="2017+2018",
		   TString channel="#mu#mu channel",
		   float ymin=10e-4,
		   float ymax=10e2,
		   bool bottomleft = false
		   )
{
  ind += 1;
  TString cname = Form("c_%d", ind);
  bool showdata = false;
  bool tosave = true;

  //-----------------------------------------------------------------------
  // Loading data from input file in the following format:
  // mass >> obs >> exp_2d >> exp_1d >> exp >> exp_1u >> exp_2u >> theory 
  //-----------------------------------------------------------------------
  vector<LimitData> limits = ReadDataFromFile(infile); if(limits.empty()) return;
  int nPoints = limits.size();
  if (nPoints < 3) {
    cout << "\033[31mInsufficient points (" << nPoints << ") in file: " << infile.Data();
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

  //Define graphs:
  TGraph* exp    = new TGraph(nPoints);
  TGraph* obs    = new TGraph(nPoints);
  TGraph* theory = new TGraph(nPoints);
  TGraph* yellow = new TGraph(2 * nPoints);
  TGraph* green  = new TGraph(2 * nPoints);

  // Apply styles:
  Int_t color_green  = kOrange;  //TColor::GetColor(13, 203, 7);
  Int_t color_yellow = kGreen+1; //TColor::GetColor(239, 201, 7);
  StyleGraph(exp,    kBlack, 7,2,cname);
  StyleGraph(obs,    kBlack, 1,2,cname); obs->SetMarkerStyle(20);
  StyleGraph(theory, kRed,   1,3,cname); theory->SetMarkerStyle(20);
  StyleUncertaintyBand(yellow, color_green);
  StyleUncertaintyBand(green, color_yellow);

  //Filling the graphs:
  for (int i = 0; i < nPoints; ++i) {
    double mass   = limits[i].mass;
    double expval = limits[i].exp;    double obsval = limits[i].obs;
    double exp_1u = limits[i].exp_1u; double exp_1d = limits[i].exp_1d;
    double exp_2u = limits[i].exp_2u; double exp_2d = limits[i].exp_2d;
    double pred   = limits[i].theory;
    exp   ->SetPoint(i, mass, expval);
    obs   ->SetPoint(i, mass, obsval);
    theory->SetPoint(i, mass, pred);
    green ->SetPoint(i, mass, exp_1u);
    yellow->SetPoint(i, mass, exp_2u);
    green ->SetPoint(2*nPoints-1-i, mass, exp_1d);
    yellow->SetPoint(2*nPoints-1-i, mass, exp_2d);
  }

  // Drawing one canvas
  SetAxisTitlesAndRange(theory, ymin, ymax); theory->Draw("AL"); //This decides the decorations
  yellow->Draw("F same");
  green ->Draw("F same");
  exp   ->Draw("L same");
  if(showdata) obs->Draw("LP same");
  theory->Draw("L same");
  
  c1->Update();

  // Create and draw legend
  TLegend* legend = CreateLegend(bottomleft);
  if(showdata) legend->AddEntry(obs, "Observed", "lp");
  legend->AddEntry(exp,    "Expected", "l");
  legend->AddEntry(yellow, "68% Expected", "f");
  legend->AddEntry(green,  "95% Expected", "f");
  legend->AddEntry(theory, modelname, "l");
  legend->Draw();

  // Add CMS label
  AddCMSLabel(c1, campaign, channel, bottomleft);;
  c1->Update();
  
  if(tosave){
    c1->SaveAs(outfile+".svg");
    c1->SaveAs(outfile+".png");
  }
  //delete c1;
  indplt += 1;
}

//_____________________________________________________________________________________
//_____________________________________________________________________________________

vector<LimitData> ReadDataFromFile(const TString& infile) {
  ifstream txtfile(infile.Data());
  if (!txtfile.is_open()) {
    cerr << "\033[31mError: Unable to open txtfile " << infile.Data() <<"\033[0m"<< endl;
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
