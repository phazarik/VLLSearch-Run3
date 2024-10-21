#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <TString.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <TAxis.h>
#include <TLegend.h>
#include <TColor.h>
#include <TLine.h>
#include <TLatex.h>

using namespace std;

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

vector<LimitData> ReadDataFromFile(const TString& filename);
void StyleGraph(TGraph* graph, int color, int lineStyle = 1, int lineWidth = 2);
void StyleUncertaintyBand(TGraphAsymmErrors* graph, int fillColor);
void SetAxisTitlesAndRange(TGraph* graph, double ymin, double ymax);
void AddCMSLabel(TCanvas* canvas);
TLegend* CreateLegend();
TCanvas* CreateCanvas();

//##########################
//          Main
//##########################
void makeLimitPlot(TString tag="scaled_mm", TString modelname="VLLD-#mu") {

  TString filename = "combinelimits_sigmaB/"+tag+"/limits_sigmaB_VLLD_mu_"+tag+".txt";
  TString outname = "demo_"+tag+".png";

  //.x makeLimitPlot.C("scaled_mm", "VLLD-#mu")
   
  vector<LimitData> limits = ReadDataFromFile(filename);
  int nPoints = limits.size();
  if (limits.empty()) return;

  TCanvas* c1 = CreateCanvas();
  TGraphAsymmErrors* graph_1sigma = new TGraphAsymmErrors(nPoints);
  TGraphAsymmErrors* graph_2sigma = new TGraphAsymmErrors(nPoints);
  TGraph* graph_exp = new TGraph(nPoints);
  TGraph* graph_obs = new TGraph(nPoints);
  TGraph* graph_theory = new TGraph(nPoints);

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

    // Seting expected, observed and thoery lines:
    graph_exp->SetPoint(i, mass, exp);
    graph_obs->SetPoint(i, mass, obs);
    graph_theory->SetPoint(i, mass, theory);
  }

  // Applying styles
  Int_t color_1sigma = TColor::GetColor(13, 203, 7);
  Int_t color_2sigma = TColor::GetColor(239, 201, 7);
  StyleUncertaintyBand(graph_2sigma, color_2sigma);
  StyleUncertaintyBand(graph_1sigma, color_1sigma);
  StyleGraph(graph_exp, kBlack, 7, 2);
  StyleGraph(graph_obs, kBlack, 1, 2); graph_obs->SetMarkerStyle(20);
  StyleGraph(graph_theory, kRed, 1, 3);
  graph_theory->SetMarkerStyle(20);

  // Draw on canvas: dummy.
  graph_theory->Draw("AL");
  graph_theory->SetTitle("");
  graph_theory->GetYaxis()->SetTitle("95% CL limit on #sigma B [pb]");
  SetAxisTitlesAndRange(graph_theory, 10e-3, 10e2);

  // Draw other graphs
  graph_2sigma->Draw("3 same");
  graph_1sigma->Draw("3 same");
  graph_exp->Draw("L same");
  graph_obs->Draw("LP same");
  graph_theory->Draw("L same");

  // Create and draw legend
  TLegend* legend = CreateLegend();
  legend->AddEntry(graph_obs, "Observed", "lp");
  legend->AddEntry(graph_exp, "Expected", "l");
  legend->AddEntry(graph_1sigma, "68% Expected", "f");
  legend->AddEntry(graph_2sigma, "95% Expected", "f");
  legend->AddEntry(graph_theory, modelname, "l");
  legend->Draw();

  // Add CMS label
  AddCMSLabel(c1);

  // Set logarithmic scale and update canvas
  c1->SetLogy();
  c1->Update();
  c1->SaveAs("demo.png");
}

//###################
// Custom functions:
//###################

vector<LimitData> ReadDataFromFile(const TString& filename) {
  std::ifstream infile(filename.Data());
  if (!infile.is_open()) {
    std::cerr << "Error: Unable to open file " << filename << std::endl;
    return {};
  }

  vector<LimitData> limits;
  std::string line;

  // Skip the header line
  std::getline(infile, line);

  // Read the data
  while (std::getline(infile, line)) {
    std::istringstream iss(line);
    double mass_, obs_, exp_2down_, exp_1down_, exp_, exp_1up_, exp_2up_, theory_;
    if (!(iss >> mass_ >> obs_ >> exp_2down_ >> exp_1down_ >> exp_ >> exp_1up_ >> exp_2up_ >> theory_)) {
      std::cerr << "Warning: Skipping line: " << line << std::endl;
      continue;
    }
    limits.push_back({mass_, obs_, exp_, exp_1up_, exp_1down_, exp_2up_, exp_2down_, theory_});
  }
  infile.close();
  return limits;
}

void StyleGraph(TGraph* graph, int color, int lineStyle = 1, int lineWidth = 2) {
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
  gStyle->SetCanvasPreferGL(true);//Enabling anti-aliasing
}

void AddCMSLabel(TCanvas* canvas) {
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.06);
  latex.DrawLatex(0.17, 0.86, "CMS");
  latex.SetTextFont(52);
  
  latex.SetTextSize(0.05);
  latex.DrawLatex(0.30, 0.86, "Preliminary");
  latex.SetTextFont(42);

  latex.SetTextSize(0.04);
  latex.DrawLatex(0.70, 0.96, "59.8 fb^{-1} (13 TeV)");
  latex.DrawLatex(0.17, 0.81, "Asymptotic. Stat only");
}

TLegend* CreateLegend() {
  TLegend* legend = new TLegend(0.56, 0.65, 0.96, 0.90);
  legend->SetTextSize(0.04);
  legend->SetBorderSize(0);  // No border
  legend->SetFillStyle(0);   // Transparent background
  return legend;
}

TCanvas* CreateCanvas() {
  TCanvas* c1 = new TCanvas("canvas", "Limit Plot", 500, 500);
  c1->SetLeftMargin(0.14);
  c1->SetRightMargin(0.02);
  c1->SetBottomMargin(0.12);
  c1->SetTopMargin(0.06);
  c1->SetTicks(1, 1);
  return c1;
}
