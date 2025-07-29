#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "THStack.h"
#include "TF1.h"
#include "TString.h"
#include <TLatex.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TAxis.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>

using namespace std;

struct plotdata{
  TString file;
  TString name;
  int color;
  int ls;
};

TCanvas* CreateCanvas(TString name);
TLegend* CreateLegend();
void AddCMSLabel(bool bottomleft = false);
void overlay_one(TString var, TString name, int rebin=1);
void plothist(TH1* h, plotdata pd, float ymin, float ymax, bool first, TString xtitle);
void sort_legends(TLegend* leg);

void overlay(){
  overlay_one("vllep_all_pt", "p_{T}(E)", 10);
  //return;
  overlay_one("vllep_all_eta", "#eta(E)", 10);
  overlay_one("vllep_all_phi", "#phi(E)", 10);
  overlay_one("vllep_all_M", "M(E)");
  overlay_one("vllep_all_pdgid", "pdgID (E)");
  overlay_one("vllep_all_dau_pdgid", "daughters of E");
  overlay_one("vlnu_all_pt", "p_{T}(N)", 10);
  overlay_one("vlnu_all_eta", "#eta(N)", 10);
  overlay_one("vlnu_all_phi", "#phi(N)", 10);
  overlay_one("vlnu_all_M", "M(N)");
  overlay_one("vlnu_all_pdgid", "pdgID (N)");
  overlay_one("vlnu_all_dau_pdgid", "daughters of N");
  overlay_one("prod_modes", "VLL production mode");
  overlay_one("decay_modes_EE", "decays from EE production");
  overlay_one("decay_modes_EN", "decays from EN production");
  overlay_one("decay_modes_NN", "decays from NN production");
  overlay_one("deltaphi_LL", "#Delta#phi(E/N,E/N)");
  overlay_one("deltaR_LL", "#Delta R(E/N,E/N)");

  //Other gen-level plots
  overlay_one("ngenmu", "N_{gen #mu}");
  overlay_one("genmu_pt", "p_{T}(gen #mu)", 10);
  overlay_one("genmu_eta", "#eta(gen #mu)", 10);
  overlay_one("genmu_phi", "#phi(gen #mu)", 10);
  overlay_one("ngenele", "N_{gen e}");
  overlay_one("genele_pt", "p_{T}(gen e)", 10);
  overlay_one("genele_eta", "#eta(gen e)", 10);
  overlay_one("genele_phi", "#phi(gen e)", 10);
  overlay_one("ngenlep", "N_{gen e/#mu}");
  overlay_one("genlep_pt", "p_{T}(gen e/#mu)", 10);
  overlay_one("genlep_eta", "#eta(gen e/#mu)", 10);
  overlay_one("genlep_phi", "#phi(gen e/#mu)", 10);
  overlay_one("ngenjet", "N_{gen jet}");
  overlay_one("genjet_pt", "p_{T}(gen jet)", 10);
  overlay_one("genjet_eta", "#eta(gen jet)", 10);
  overlay_one("genjet_phi", "#phi(gen jet)", 10);
  overlay_one("genHT", "H_{T} (jets)", 10);
  overlay_one("genLT", "L_{T} (leptons)", 10);
  overlay_one("genST", "S_{T}=H_{T}+L_{T}+MET", 10);
  overlay_one("genMET_pt", "p_{T}^{miss}", 10);
  overlay_one("genMET_phi", "#phi^{miss}", 10);
  
  //Other reco-level plots
  overlay_one("nrecomu", "N_{reco #mu}");
  overlay_one("recomu_pt", "p_{T}(reco #mu)", 10);
  overlay_one("recomu_eta", "#eta(reco #mu)", 10);
  overlay_one("recomu_phi", "#phi(reco #mu)", 10);
  overlay_one("nrecoele", "N_{reco e}");
  overlay_one("recoele_pt", "p_{T}(reco e)", 10);
  overlay_one("recoele_eta", "#eta(reco e)", 10);
  overlay_one("recoele_phi", "#phi(reco e)", 10);
  overlay_one("nrecolep", "N_{reco e/#mu}");
  overlay_one("recolep_pt", "p_{T}(reco e/#mu)", 10);
  overlay_one("recolep_eta", "#eta(reco e/#mu)", 10);
  overlay_one("recolep_phi", "#phi(reco e/#mu)", 10);
  overlay_one("nrecojet", "N_{reco jet}");
  overlay_one("recojet_pt", "p_{T}(reco jet)", 10);
  overlay_one("recojet_eta", "#eta(reco jet)", 10);
  overlay_one("recojet_phi", "#phi(reco jet)", 10);
  overlay_one("nbjet", "N_{b jet}");
  overlay_one("bjet_pt", "p_{T}(b jet)", 10);
  overlay_one("bjet_eta", "#eta(b jet)", 10);
  overlay_one("bjet_phi", "#phi(b jet)", 10);
  overlay_one("recoHT", "H_{T} (jets)", 10);
  overlay_one("recoLT", "L_{T} (leptons)", 10);
  overlay_one("recoST", "S_{T}=H_{T}+L_{T}+MET", 10);
  overlay_one("recoMET_pt", "p_{T}^{miss}", 10);
  overlay_one("recoMET_phi", "#phi^{miss}", 10);
}

void overlay_one(TString var, TString name, int rebin){
  cout<<"\033[0;33mProcessing: "<<var<<"\033[0m"<<endl;
  bool toNorm = true;

  vector<plotdata> hists = {
    {"hst_VLLD_ele_M150.root",  "VLLD-e_{150}",  kAzure-4, 3},
    {"hst_VLLD_ele_M500.root",  "VLLD-e_{500}",  kAzure+2, 2},
    {"hst_VLLD_ele_M1000.root", "VLLD-e_{1000}", kBlue,    1},
    /*
    {"hst_VLLD_mu_M150.root",    "VLLD-#mu_{150}", kRed-4,   3},
    {"hst_VLLD_mu_M500.root",    "VLLD-#mu_{500}", kRed+1,   2},
    {"hst_VLLD_mu_M1000.root",   "VLLD-#mu_{1000}",kRed,     1},

    {"hst_VLLD_tau_M150.root",   "VLLD-#tau_{150}",kGreen-6, 3},
    {"hst_VLLD_tau_M500.root",   "VLLD-#tau_{500}",kGreen+2, 2},
    {"hst_VLLD_tau_M1000.root",  "VLLD-#tau_{1000}",kGreen+1,1},*/
  };

  vector<TH1*> histObjs;
  histObjs.reserve(hists.size());

  float maxVal = -1;
  int maxIndex = -1;

  for (size_t i = 0; i < hists.size(); i++) {
    TString filepath = "test_outputs/" + hists[i].file;
    TFile *f = TFile::Open(filepath);
    if (!f || f->IsZombie()){ 
      cout<<"\033[31m[Warning] File not found: "<<filepath<<"\033[0m"<<endl; 
      histObjs.push_back(nullptr); 
      continue;
    }
    TH1 *h = (TH1*) f->Get(var);
    if (!h){ 
      cout<<"\033[31m[Warning] Hist not found: "<<var<<" in "<<filepath<<"\033[0m"<<endl; 
      histObjs.push_back(nullptr); 
      f->Close();
      continue;
    }

    h->Rebin(rebin);
    if (toNorm && h->Integral() > 0) h->Scale(1.0 / h->Integral());
    h->SetDirectory(0);
    histObjs.push_back(h);
    f->Close();

    float thisMax = h->GetMaximum();
    if (thisMax > maxVal) { maxVal = thisMax; maxIndex = i; }
  }

  float ymin = 0; 
  float ymax = maxVal * 1.2;

  TCanvas *c1 = CreateCanvas(var);
  TLegend *leg = CreateLegend();
  
  bool first_drawn = false;
  for (size_t i = 0; i < hists.size(); i++) {
    if (i == (size_t)maxIndex) continue;
    plothist(histObjs[i], hists[i], ymin, ymax, !first_drawn, name);
    first_drawn = true;
  }

  plothist(histObjs[maxIndex], hists[maxIndex], ymin, ymax, !first_drawn, name);

  //Legends and labels:
  for (size_t i = 0; i < hists.size(); ++i){if (histObjs[i]) leg->AddEntry(histObjs[i], hists[i].name, "l");}
  leg->Draw();
  AddCMSLabel();

  //Done.
  c1->SaveAs("signal_plots/overlay_"+var+".png");
}

void plothist(TH1* h, plotdata pd, float ymin, float ymax, bool first, TString xtitle) {
  if (!h) {cout << "\033[31m[Warning] Histogram pointer is NULL for: " << pd.file << "\033[0m" << endl; return;}

  h->SetLineColor(pd.color);
  h->SetLineWidth(3);
  h->SetLineStyle(pd.ls);
  
  if (first) {
    gPad->SetTicks(1,1);
    h->SetStats(0);
    h->SetTitle("");
    h->GetXaxis()->SetTitle(xtitle);
    h->GetYaxis()->SetTitle("Normalized");
    h->GetXaxis()->SetTitleSize(0.04);
    h->GetYaxis()->SetTitleSize(0.04);
    h->GetXaxis()->SetTitleOffset(1.0);
    h->GetYaxis()->SetTitleOffset(1.2);
    h->GetYaxis()->SetRangeUser(ymin, ymax);
    h->Draw("HIST");
  }
  else h->Draw("HIST SAME");
}

TCanvas* CreateCanvas(TString name) {
  TCanvas* c1 = new TCanvas(name, name, 100, 100, 600, 500);
  c1->SetFillColor(0);
  c1->SetBorderMode(0);
  c1->SetFrameFillStyle(0);
  c1->SetFrameBorderMode(0);
  c1->SetLeftMargin(0.10);
  c1->SetRightMargin(0.20);
  c1->SetTopMargin(0.08);
  c1->SetBottomMargin(0.10);
  c1->SetTickx(1);
  c1->SetTicky(1);
  c1->SetGrid(0, 0);
  return c1;
}

TLegend* CreateLegend() {
  TLegend* leg = new TLegend(0.81, 0.70, 1.0, 0.95);
  leg->SetTextSize(0.03);
  leg->SetTextFont(42);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetMargin(0.2);
  return leg;
}

void AddCMSLabel(bool bottomleft) {
  TLatex latex;
  latex.SetNDC();
  float xpos = 0.10;
  float ypos = bottomleft ? 0.26 : 0.94;

  latex.SetTextFont(61);
  latex.SetTextSize(0.06);
  latex.DrawLatex(xpos, ypos, "CMS");

  latex.SetTextFont(52);
  latex.SetTextSize(0.05);
  latex.DrawLatex(xpos + 0.12, ypos, "Preliminary");

  latex.SetTextFont(42);
  latex.SetTextSize(0.04);
  latex.SetTextAlign(31);
  latex.DrawLatex(0.80, 0.94, "Signal study");
}
