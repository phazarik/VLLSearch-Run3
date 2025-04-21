#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <TString.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <TAxis.h>
#include <TLegend.h>
#include <TColor.h>
#include <TLine.h>
#include <TLatex.h>
#include <TSystem.h>
using namespace std;
int ind;

// Struct to hold limit data
struct LimitData {
  double mass;
  double obs;
  double exp;
  double exp_1up;
  double exp_1down;
  double exp_2up;
  double exp_2down;
  double theory;
};

struct plotinfo {
    TString filename;
    TString modelname;
    TString campaign;
    TString channel;
    TString outname;
    float ymin=10e-4;
    float ymax=10e2;
};

vector<LimitData> ReadDataFromFile(const TString& filename);
void StyleGraph(TGraph* graph, int color, int lineStyle = 1, int lineWidth = 2);
void StyleUncertaintyBand(TGraphAsymmErrors* graph, int fillColor);
void SetAxisTitlesAndRange(TGraph* graph, double ymin, double ymax);
void AddCMSLabel(TCanvas* canvas, TString campaign, TString channel);
TLegend* CreateLegend();
TCanvas* CreateCanvas(TString name);
void makeOneLimitPlot(TString infile, TString outfile, TString modelname, TString campaign, TString channel, float ymin, float ymax);

//##########################
//          Main
//##########################
void makeLimitPlots(){

  TString timetag = "2025-04-21";
  TString indir   = gSystem->ConcatFileName("sigmaBlimits", timetag);
  TString outdir  = gSystem->ConcatFileName("plots", timetag);
  gSystem->mkdir(outdir, true);

  ind=0;
  vector<plotinfo> limitdict = {
    
    {"sigmaB_VLLD_ele_2016preVFP_UL_ee.txt", "VLLD-e", "2016preVFP_UL", "ee",     "limit_VLLD_ele_2016preVFP_UL_ee.png"},
    {"sigmaB_VLLD_ele_2016preVFP_UL_em.txt", "VLLD-e", "2016preVFP_UL", "e#mu",   "limit_VLLD_ele_2016preVFP_UL_em.png"},
    {"sigmaB_VLLD_ele_2016preVFP_UL_me.txt", "VLLD-e", "2016preVFP_UL", "#mue",   "limit_VLLD_ele_2016preVFP_UL_me.png"},
    {"sigmaB_VLLD_ele_2016preVFP_UL_mm.txt", "VLLD-e", "2016preVFP_UL", "#mu#mu", "limit_VLLD_ele_2016preVFP_UL_mm.png"},
    
    {"sigmaB_VLLD_ele_2016preVFP_UL_ee.txt", "VLLD-e", "2016preVFP_UL", "ee",     "limit_VLLD_ele_2016preVFP_UL_ee.png"},
    {"sigmaB_VLLD_ele_2016preVFP_UL_em.txt", "VLLD-e", "2016preVFP_UL", "e#mu",   "limit_VLLD_ele_2016preVFP_UL_em.png"},
    {"sigmaB_VLLD_ele_2016preVFP_UL_me.txt", "VLLD-e", "2016preVFP_UL", "#mue",   "limit_VLLD_ele_2016preVFP_UL_me.png"},
    {"sigmaB_VLLD_ele_2016preVFP_UL_mm.txt", "VLLD-e", "2016preVFP_UL", "#mu#mu", "limit_VLLD_ele_2016preVFP_UL_mm.png"},

    {"sigmaB_VLLD_ele_2017_UL_ee.txt", "VLLD-e", "2017_UL", "ee",     "limit_VLLD_ele_2017_UL_ee.png"},
    {"sigmaB_VLLD_ele_2017_UL_em.txt", "VLLD-e", "2017_UL", "e#mu",   "limit_VLLD_ele_2017_UL_em.png"},
    {"sigmaB_VLLD_ele_2017_UL_me.txt", "VLLD-e", "2017_UL", "#mue",   "limit_VLLD_ele_2017_UL_me.png"},
    {"sigmaB_VLLD_ele_2017_UL_mm.txt", "VLLD-e", "2017_UL", "#mu#mu", "limit_VLLD_ele_2017_UL_mm.png"},

    {"sigmaB_VLLD_ele_2018_UL_ee.txt", "VLLD-e", "2018_UL", "#ee",    "limit_VLLD_ele_2017_UL_ee.png"},
    {"sigmaB_VLLD_ele_2018_UL_em.txt", "VLLD-e", "2018_UL", "e#mu",   "limit_VLLD_ele_2018_UL_em.png"},
    {"sigmaB_VLLD_ele_2018_UL_me.txt", "VLLD-e", "2018_UL", "#mue",   "limit_VLLD_ele_2018_UL_me.png"},
    {"sigmaB_VLLD_ele_2018_UL_mm.txt", "VLLD-e", "2018_UL", "#mu#mu", "limit_VLLD_ele_2018_UL_mm.png"},

    {"sigmaB_VLLD_ele_combined_Run2.txt", "VLLD-e",   "Run2_UL", "combined", "limit_VLLD_ele_combined_Run2.png"},

    {"sigmaB_VLLD_mu_2016preVFP_UL_ee.txt", "VLLD-#mu", "2016preVFP_UL", "ee",     "limit_VLLD_mu_2016preVFP_UL_ee.png"},
    {"sigmaB_VLLD_mu_2016preVFP_UL_em.txt", "VLLD-#mu", "2016preVFP_UL", "e#mu",   "limit_VLLD_mu_2016preVFP_UL_em.png"},
    {"sigmaB_VLLD_mu_2016preVFP_UL_me.txt", "VLLD-#mu", "2016preVFP_UL", "#mue",   "limit_VLLD_mu_2016preVFP_UL_me.png"},
    {"sigmaB_VLLD_mu_2016preVFP_UL_mm.txt", "VLLD-#mu", "2016preVFP_UL", "#mu#mu", "limit_VLLD_mu_2016preVFP_UL_mm.png"},
    
    {"sigmaB_VLLD_mu_2016preVFP_UL_ee.txt", "VLLD-#mu", "2016preVFP_UL", "ee",     "limit_VLLD_mu_2016preVFP_UL_ee.png"},
    {"sigmaB_VLLD_mu_2016preVFP_UL_em.txt", "VLLD-#mu", "2016preVFP_UL", "e#mu",   "limit_VLLD_mu_2016preVFP_UL_em.png"},
    {"sigmaB_VLLD_mu_2016preVFP_UL_me.txt", "VLLD-#mu", "2016preVFP_UL", "#mue",   "limit_VLLD_mu_2016preVFP_UL_me.png"},
    {"sigmaB_VLLD_mu_2016preVFP_UL_mm.txt", "VLLD-#mu", "2016preVFP_UL", "#mu#mu", "limit_VLLD_mu_2016preVFP_UL_mm.png"},

    {"sigmaB_VLLD_mu_2017_UL_ee.txt",     "VLLD-#mu", "2017_UL", "ee",       "limit_VLLD_mu_2017_UL_ee.png"},
    {"sigmaB_VLLD_mu_2017_UL_em.txt",     "VLLD-#mu", "2017_UL", "e#mu",     "limit_VLLD_mu_2017_UL_em.png"},
    {"sigmaB_VLLD_mu_2017_UL_me.txt",     "VLLD-#mu", "2017_UL", "#mue",     "limit_VLLD_mu_2017_UL_me.png"},
    {"sigmaB_VLLD_mu_2017_UL_mm.txt",     "VLLD-#mu", "2017_UL", "#mu#mu",   "limit_VLLD_mu_2017_UL_mm.png"},

    {"sigmaB_VLLD_mu_2018_UL_ee.txt",     "VLLD-#mu", "2017_UL", "ee",       "limit_VLLD_mu_2018_UL_ee.png"},
    {"sigmaB_VLLD_mu_2018_UL_em.txt",     "VLLD-#mu", "2018_UL", "e#mu",     "limit_VLLD_mu_2018_UL_em.png"},
    {"sigmaB_VLLD_mu_2018_UL_me.txt",     "VLLD-#mu", "2018_UL", "#mue",     "limit_VLLD_mu_2018_UL_me.png"},
    {"sigmaB_VLLD_mu_2018_UL_mm.txt",     "VLLD-#mu", "2018_UL", "#mu#mu",   "limit_VLLD_mu_2018_UL_mm.png"},

    {"sigmaB_VLLD_mu_combined_Run2.txt", " VLLD-#mu", "Run2_UL", "combined", "limit_VLLD_mu_combined_Run2.png"},
  };

  for (size_t i = 0; i < limitdict.size(); i++) {
    TString infile    = gSystem->ConcatFileName(indir,  limitdict[i].filename);
    TString outfile   = gSystem->ConcatFileName(outdir, limitdict[i].outname);
    TString channel   = limitdict[i].channel;
    if (limitdict[i].channel != "combined") channel += " channel";
    TString modelname = limitdict[i].modelname;
    TString campaign  = limitdict[i].campaign;
    float ymin = limitdict[i].ymin; float ymax = limitdict[i].ymax;
    makeOneLimitPlot(infile, outfile, modelname, campaign, channel, ymin, ymax);
    //break;
  }

  cout<<"Done!"<<endl;
  
}

void makeOneLimitPlot(
		   TString infile="sigmaBlimits/2025-04-16/sigmaB_VLLD_mu_combined_2017_2018.txt",
		   TString outfile="test.png",
		   TString modelname="VLLD-#mu",
		   TString campaign="2017_2018",
		   TString channel="#mu#mu",
		   float ymin=10e-4,
		   float ymax=10e2
		   )
{
  ind += 1;
  TString cname = Form("c_%d", ind);
  bool showdata = false;
  bool tosave = true;
  
  vector<LimitData> limits = ReadDataFromFile(infile);
  int nPoints = limits.size();
  if (limits.empty()) return;
  
  gStyle->Reset(); //important
  gStyle->SetOptStat(0);

  //Declarations:
  TCanvas* c1 = CreateCanvas(cname); c1->Clear();
  TGraphAsymmErrors* graph_1sigma = new TGraphAsymmErrors(nPoints);
  TGraphAsymmErrors* graph_2sigma = new TGraphAsymmErrors(nPoints);
  TGraph* graph_exp = new TGraph(nPoints);
  TGraph* graph_obs = new TGraph(nPoints);
  TGraph* graph_theory = new TGraph(nPoints);

  //Assign unique names:
  graph_1sigma->SetName(Form("%s_graph_1sigma", cname.Data()));
  graph_2sigma->SetName(Form("%s_graph_2sigma", cname.Data()));
  graph_exp->SetName(Form("%s_graph_exp", cname.Data()));
  graph_obs->SetName(Form("%s_graph_obs", cname.Data()));
  graph_theory->SetName(Form("%s_graph_theory", cname.Data()));

  // Apply styles before drawing any graph
  Int_t color_1sigma = TColor::GetColor(13, 203, 7);
  Int_t color_2sigma = TColor::GetColor(239, 201, 7);
  StyleGraph(graph_exp, kBlack, 7, 2);
  StyleGraph(graph_obs, kBlack, 1, 2);  graph_obs->SetMarkerStyle(20);
  StyleGraph(graph_theory, kRed, 1, 3); graph_theory->SetMarkerStyle(20);
  StyleUncertaintyBand(graph_2sigma, color_2sigma);
  StyleUncertaintyBand(graph_1sigma, color_1sigma);

  //Filling the points
  for (int i = 0; i < nPoints; ++i) {
    double mass      = limits[i].mass;
    double exp       = limits[i].exp;
    double obs       = limits[i].obs;
    double exp_1up   = limits[i].exp_1up;
    double exp_1down = limits[i].exp_1down;
    double exp_2up   = limits[i].exp_2up;
    double exp_2down = limits[i].exp_2down;
    double theory    = limits[i].theory;

    // Setting sigma-bands:
    graph_1sigma->SetPoint(i, mass, exp);
    graph_1sigma->SetPointError(i, 0, 0, exp - exp_1down, exp_1up - exp);
    graph_2sigma->SetPoint(i, mass, exp);
    graph_2sigma->SetPointError(i, 0, 0, exp - exp_2down, exp_2up - exp);

    // Seting expected, observed, and theory lines:
    graph_exp->SetPoint(i, mass, exp);
    graph_obs->SetPoint(i, mass, obs);
    graph_theory->SetPoint(i, mass, theory);
  }

  // Draw first one on canvas
  graph_theory->Draw("AL");
  graph_theory->SetTitle("");
  graph_theory->GetYaxis()->SetTitle("95% CL limit on #sigma B [pb]");
  SetAxisTitlesAndRange(graph_theory, ymin, ymax);

  // Draw others:
  graph_2sigma->Draw("3 same");
  graph_1sigma->Draw("3 same");
  graph_exp->Draw("L same");
  if(showdata) graph_obs->Draw("LP same");
  graph_theory->Draw("L same");
  c1->Update();

  // Create and draw legend
  TLegend* legend = CreateLegend();
  if(showdata) legend->AddEntry(graph_obs, "Observed", "lp");
  legend->AddEntry(graph_exp, "Expected", "l");
  legend->AddEntry(graph_1sigma, "68% Expected", "f");
  legend->AddEntry(graph_2sigma, "95% Expected", "f");
  legend->AddEntry(graph_theory, modelname, "l");
  legend->Draw();

  // Add CMS label
  AddCMSLabel(c1, campaign, channel);

  // Set logarithmic scale and update canvas
  c1->SetLogy();
  c1->Update();
  if(tosave) c1->SaveAs(outfile);
  //delete c1;

}

//_____________________________________________________________________________________

//                                Custom functions
//_____________________________________________________________________________________

vector<LimitData> ReadDataFromFile(const TString& infile) {
  std::ifstream txtfile(infile.Data());
  if (!txtfile.is_open()) {
    std::cerr << "Error: Unable to open txtfile " << infile.Data() << std::endl;
    return {};
  }

  vector<LimitData> limits;
  std::string line;

  // Skip the header line
  std::getline(txtfile, line);

  // Read the data
  while (std::getline(txtfile, line)) {
    std::istringstream iss(line);
    double mass_, obs_, exp_2down_, exp_1down_, exp_, exp_1up_, exp_2up_, theory_;
    if (!(iss >> mass_ >> obs_ >> exp_2down_ >> exp_1down_ >> exp_ >> exp_1up_ >> exp_2up_ >> theory_)) {
      std::cerr << "Warning: Skipping line: " << line << std::endl;
      continue;
    }
    limits.push_back({mass_, obs_, exp_, exp_1up_, exp_1down_, exp_2up_, exp_2down_, theory_});
  }
  txtfile.close();
  return limits;
}

void StyleGraph(TGraph* graph, int color, int lineStyle, int lineWidth) {
  graph->SetLineColor(color);
  graph->SetLineStyle(lineStyle);
  graph->SetLineWidth(lineWidth);
}

void StyleUncertaintyBand(TGraphAsymmErrors* graph, int fillColor) {
  graph->SetFillColor(fillColor);
  graph->SetFillStyle(1001);  // Solid fill
}

void SetAxisTitlesAndRange(TGraph* graph, double ymin, double ymax) {
  graph->GetYaxis()->SetTitle("95% CL limit on #sigma B [pb]");
  graph->GetYaxis()->SetTitleSize(0.05);
  graph->GetYaxis()->SetTitleOffset(1.15);
  graph->GetYaxis()->SetTickSize(0.02);
  graph->GetYaxis()->SetRangeUser(ymin, ymax);
  graph->GetXaxis()->SetTitle("M_{VLL} (GeV)");
  graph->GetXaxis()->SetTitleSize(0.05);
  graph->GetXaxis()->SetTitleOffset(0.98);
  graph->GetXaxis()->SetLabelSize(0.04);
  graph->GetXaxis()->SetTickSize(0.02);
  gStyle->SetCanvasPreferGL(true);  // Enabling anti-aliasing
}

void AddCMSLabel(TCanvas* canvas, TString campaign, TString channel) {
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.06);
  latex.DrawLatex(0.17, 0.86, "CMS");
  latex.SetTextFont(52);
  
  latex.SetTextSize(0.05);
  latex.DrawLatex(0.30, 0.86, "Preliminary");
  latex.SetTextFont(42);
  latex.SetTextSize(0.04);

  if     (string(campaign) == "2016preVFP_UL")  latex.DrawLatex(0.62, 0.96, "19.7 fb^{-1} (2016-preVFP)");
  else if(string(campaign) == "2016postVFP_UL") latex.DrawLatex(0.60, 0.96, "16.2 fb^{-1} (2016-postVFP)");
  else if(string(campaign) == "2017_UL")        latex.DrawLatex(0.74, 0.96, "41.5 fb^{-1} (2017)");
  else if(string(campaign) == "2018_UL")        latex.DrawLatex(0.74, 0.96, "59.8 fb^{-1} (2018)");
  else if(string(campaign) == "2017+2018")      latex.DrawLatex(0.62, 0.96, "101.3 fb^{-1} (2017+2018)");
  else if(string(campaign) == "Run2_UL")        latex.DrawLatex(0.72, 0.96, "137 fb^{-1} (13 TeV)");
  else if(string(campaign) == "Run3Summer22")   latex.DrawLatex(0.74, 0.96, "7.98 fb^{-1} (2022)");
  else cout<<"Put correct campaign name!"<<endl;
  
  latex.DrawLatex(0.17, 0.81, "Asymptotic. Stat only");
  latex.DrawLatex(0.17, 0.76, channel);
}

TLegend* CreateLegend() {
  TLegend* legend = new TLegend(0.56, 0.65, 0.96, 0.90);
  legend->SetTextSize(0.035);
  legend->SetTextFont(42);
  legend->SetBorderSize(0);
  legend->SetFillStyle(0);
  return legend;
}

TCanvas* CreateCanvas(TString name) {
  TCanvas* c1 = new TCanvas(name, name, 500, 500);
  c1->SetLeftMargin(0.14);
  c1->SetRightMargin(0.02);
  c1->SetBottomMargin(0.12);
  c1->SetTopMargin(0.06);
  c1->SetTicks(1, 1);
  c1->SetFillColor(0);
  c1->SetFrameFillColor(0);
  return c1;
}
