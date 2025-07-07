#ifndef CMSDECORATIONS_H
#define CMSDECORATIONS_H

#include <iostream>
#include <string>
#include <TString.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TAxis.h>
using namespace std;

void StyleGraph(TGraph* graph, int color, int ls=1, int lw=2, TString name="") {
  graph->SetLineColor(color);
  graph->SetLineStyle(ls);
  graph->SetLineWidth(lw);
  graph->SetName(Form("%s", name.Data()));
}

void StyleUncertaintyBand(TGraph* graph, int color) {
  graph->SetFillColor(color);
  graph->SetLineColor(kBlack);
  graph->SetFillStyle(1001);
  graph->SetLineWidth(1);
}

void SetAxisTitlesAndRange(TGraph* graph, double ymin, double ymax) {
  graph->SetTitle("");
  //Y-axis:
  graph->GetYaxis()->SetTitle("95% CL limit on #sigmaB [pb]");
  //graph->GetYaxis()->CenterTitle();
  graph->GetYaxis()->SetTitleSize(0.05);
  graph->GetYaxis()->SetLabelSize(0.04);
  graph->GetYaxis()->SetTitleOffset(1.18);
  graph->GetYaxis()->SetTitleOffset(1.12);
  graph->GetYaxis()->SetTickSize(0.02);
  graph->GetYaxis()->SetRangeUser(ymin, ymax);
  //X-axis:
  graph->GetXaxis()->SetTitle("M_{VLL} [GeV]");
  graph->GetXaxis()->SetNdivisions(508);
  graph->GetXaxis()->SetTitleSize(0.05);
  graph->GetXaxis()->SetLabelSize(0.04);
  graph->GetXaxis()->SetTitleOffset(0.98);
  graph->GetXaxis()->SetTickSize(0.02);
  gStyle->SetCanvasPreferGL(true);  // Enabling anti-aliasing
}

void AddCMSLabel(TCanvas* canvas, TString campaign, TString channel, bool bottomleft = false) {
  TLatex latex;
  latex.SetNDC();
  latex.SetTextFont(61);
  latex.SetTextSize(0.06);
  float xpos = 0.15; float ypos = bottomleft ? 0.26 : 0.86;
  latex.DrawLatex(xpos, ypos, "CMS");
  latex.SetTextFont(52);
  latex.SetTextSize(0.05);
  latex.DrawLatex(xpos + 0.12, ypos, "Preliminary");
  latex.SetTextFont(42);
  latex.SetTextSize(0.040);
  latex.DrawLatex(xpos, ypos - 0.05, "Asymptotic, stat only");
  latex.DrawLatex(xpos, ypos - 0.10, channel);
  if      (campaign == "2016preVFP_UL")    latex.DrawLatex(0.59, 0.95, "19.7 fb^{-1} (2016-preVFP)");
  else if (campaign == "2016postVFP_UL")   latex.DrawLatex(0.57, 0.95, "16.2 fb^{-1} (2016-postVFP)");
  else if (campaign == "2017_UL")          latex.DrawLatex(0.71, 0.95, "41.5 fb^{-1} (2017)");
  else if (campaign == "2018_UL")          latex.DrawLatex(0.71, 0.95, "59.8 fb^{-1} (2018)");
  else if (campaign == "2017+2018")        latex.DrawLatex(0.59, 0.95, "101.3 fb^{-1} (2017+2018)");
  else if (campaign == "Run2_UL")          latex.DrawLatex(0.69, 0.95, "137 fb^{-1} (13 TeV)");
  else if (campaign == "Run3Summer22")     latex.DrawLatex(0.61, 0.95, "7.98 fb^{-1} (2022-preEE)");
  else if (campaign == "Run3Summer22EE")   latex.DrawLatex(0.57, 0.95, "26.67 fb^{-1} (2022-postEE)");
  else if (campaign == "Run3Summer23")     latex.DrawLatex(0.56, 0.95, "17.79 fb^{-1} (2023-preBPix)");
  else if (campaign == "Run3Summer23BPix") latex.DrawLatex(0.57, 0.95, "9.45 fb^{-1} (2023-postBPix)");
  else if (campaign == "Run3")             latex.DrawLatex(0.69, 0.95, "62 fb^{-1} (13.6 TeV)");
  else if (campaign == "2022")             latex.DrawLatex(0.71, 0.95, "34.65 fb^{-1} (2022)");
  else std::cout << "Put correct campaign name!" << std::endl;
}

TLegend* CreateLegend(bool bottomleft=false) {
  TLegend* legend;
  if(bottomleft) legend = new TLegend(0.62, 0.15, 0.95, 0.40);
  else           legend = new TLegend(0.62, 0.65, 0.95, 0.90);
  legend->SetTextSize(0.035);
  legend->SetTextFont(42);
  legend->SetBorderSize(0);
  legend->SetFillColor(0);
  legend->SetFillStyle(3013);
  return legend;
}

TCanvas* CreateCanvas(TString name) {
  float W = 500, H = 500;
  float T = 0.08 * H, B = 0.12 * H;
  float L = 0.12 * W, R = 0.04 * W;
  TCanvas* c1 = new TCanvas(name, name, 100, 100, W, H);
  c1->SetFillColor(0);
  c1->SetBorderMode(0);
  c1->SetFrameFillStyle(0);
  c1->SetFrameBorderMode(0);
  c1->SetLeftMargin(L / W);
  c1->SetRightMargin(R / W);
  c1->SetTopMargin(T / H);
  c1->SetBottomMargin(B / H);
  c1->SetTickx(1);
  c1->SetTicky(1);
  c1->SetGrid(0, 0);
  c1->SetLogy();
  return c1;
}

#endif //#CMSDECORATIONS_H
