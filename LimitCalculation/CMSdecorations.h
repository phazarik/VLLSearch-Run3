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

void SetAxisTitlesAndRange(TGraph* graph, double ymin, double ymax, double xmin, double xmax) {
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
  graph->GetXaxis()->SetRangeUser(xmin, xmax);
  gStyle->SetCanvasPreferGL(true);  // Enabling anti-aliasing
}

void put_text(TString text, float x, float y, int style, float size){
  TText* t = new TText(x, y, text);
  t->SetTextSize(size);
  t->SetNDC();
  t->SetTextFont(style); // Bold
  t->Draw();
}

void put_latex_text(TString text, float x, float y, int style, float size, bool rightAlign=false){
    TLatex* latex = new TLatex();
    latex->SetTextFont(style);
    latex->SetTextSize(size);
    latex->SetNDC();
    if(rightAlign) latex->SetTextAlign(31);  // Right-aligned (x,y)
    else           latex->SetTextAlign(11);  // Left-aligned (x,y)
    latex->DrawLatex(x, y, text);
}

void AddCMSLabel(TCanvas* canvas, TString campaign, TString channel, TString energy, TString final_state, bool bottomleft = false, bool systematics=false) {

  TString methodname = "Asymptotic, stat only";
  if (systematics) methodname = "Asymptotic, stat+syst";

  put_text("CMS", 0.15, 0.85, 62, 0.07);            // Larger, bold CMS label
  put_text("Preliminary", 0.30, 0.85, 52, 0.05);    // Smaller preliminary label
  put_latex_text(methodname, 0.15, 0.80, 42, 0.04); // Additional information
  put_latex_text(final_state, 0.15, 0.75, 42, 0.04); // Additional information

  float xright = 0.95; float yup = 0.94;
  /*
  if(campaign == "2016preVFP_UL")    put_latex_text("19.7 fb^{-1} (2016-preVFP)",   xright, yup, 42, 0.05, true);
  if(campaign == "2016postVFP_UL")   put_latex_text("16.2 fb^{-1} (2016-postVFP)",  xright, yup, 42, 0.05, true);
  if(campaign == "2017_UL")          put_latex_text("41.5 fb^{-1} (2017)",          xright, yup, 42, 0.05, true);
  if(campaign == "2018_UL")          put_latex_text("59.8 fb^{-1} (2018)",          xright, yup, 42, 0.05, true);
  if(campaign == "Run2")             put_latex_text("137.2 fb^{-1} (Run-2)",        xright, yup, 42, 0.05, true);
  if(campaign == "Run3Summer22")     put_latex_text("7.98 fb^{-1} (2022-preEE)",    xright, yup, 42, 0.05, true);
  if(campaign == "Run3Summer22EE")   put_latex_text("26.7 fb^{-1} (2022-postEE)",   xright, yup, 42, 0.05, true);
  if(campaign == "Run3Summer23")     put_latex_text("17.8 fb^{-1} (2023-preBPix)",  xright, yup, 42, 0.05, true);
  if(campaign == "Run3Summer23BPix") put_latex_text("9.45 fb^{-1} (2023-postBPix)", xright, yup, 42, 0.05, true);
  if(campaign == "Run3")             put_latex_text("61.9 fb^{-1} (2022+2023)",     xright, yup, 42, 0.05, true);
  if(campaign == "FullDataset")   put_latex_text("199.1 fb^{-1} (Run-2+2022+2023)", xright, yup, 42, 0.05, true);
  else std::cout << "Put correct campaign name!" << std::endl;*/
  if      (energy=="13TeV")   put_latex_text("13 TeV",   xright, yup, 42, 0.05, true);
  else if (energy=="13p6TeV") put_latex_text("13.6 TeV", xright, yup, 42, 0.05, true);
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
