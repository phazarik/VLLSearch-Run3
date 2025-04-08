#ifndef SETBRANCHESANDHISTOGRAMS_H
#define SETBRANCHESANDHISTOGRAMS_H

#include <vector>
#include <TString.h>
#include <TTree.h>
#include <TH1F.h>
using namespace std;
/*
extern Long64_t channel, trigger, nlep, njet, nbjet;
extern Double_t lep0_pt, lep0_eta, lep0_phi, lep0_iso, lep0_sip3d, lep0_mt;
extern Double_t lep1_pt, lep1_eta, lep1_phi, lep1_iso, lep1_sip3d, lep1_mt;
extern Double_t dilep_pt, dilep_eta, dilep_phi, dilep_mass, dilep_mt, dilep_deta, dilep_dphi, dilep_dR, dilep_ptratio;
extern Double_t HT, LT, STvis, ST, HTMETllpt, STfrac, metpt, metphi;
extern Double_t dphi_metlep0, dphi_metlep1, dphi_metdilep, dphi_metlep_max, dphi_metlep_min;
extern Double_t wt_leptonSF, wt_trig, wt_pileup, wt_bjet, weight;
extern Double_t nnscore1, nnscore2, nnscore3, nnscore4, nnscore5;*/
extern Int_t channel, trigger, nlep, njet, nbjet;
extern Float_t lep0_pt, lep0_eta, lep0_phi, lep0_iso, lep0_sip3d, lep0_mt;
extern Float_t lep1_pt, lep1_eta, lep1_phi, lep1_iso, lep1_sip3d, lep1_mt;
extern Float_t dilep_pt, dilep_eta, dilep_phi, dilep_mass, dilep_mt, dilep_deta, dilep_dphi, dilep_dR, dilep_ptratio;
extern Float_t HT, LT, STvis, ST, HTMETllpt, STfrac, metpt, metphi;
extern Float_t dphi_metlep0, dphi_metlep1, dphi_metdilep, dphi_metlep_max, dphi_metlep_min;
extern Float_t nnscore1, nnscore2, nnscore3, nnscore4, nnscore5;
extern Double_t wt_leptonSF, wt_trig, wt_pileup, wt_bjet, weight;

void setBranches(TTree *tree)
{
  tree->SetBranchAddress("channel", &channel);
  tree->SetBranchAddress("trigger", &trigger);
  tree->SetBranchAddress("nlep", &nlep);
  tree->SetBranchAddress("njet", &njet);
  tree->SetBranchAddress("nbjet", &nbjet);
  tree->SetBranchAddress("lep0_pt", &lep0_pt);
  tree->SetBranchAddress("lep0_eta", &lep0_eta);
  tree->SetBranchAddress("lep0_phi", &lep0_phi);
  tree->SetBranchAddress("lep0_iso", &lep0_iso);
  tree->SetBranchAddress("lep0_sip3d", &lep0_sip3d);
  tree->SetBranchAddress("lep0_mt", &lep0_mt);
  tree->SetBranchAddress("lep1_pt", &lep1_pt);
  tree->SetBranchAddress("lep1_eta", &lep1_eta);
  tree->SetBranchAddress("lep1_phi", &lep1_phi);
  tree->SetBranchAddress("lep1_iso", &lep1_iso);
  tree->SetBranchAddress("lep1_sip3d", &lep1_sip3d);
  tree->SetBranchAddress("lep1_mt", &lep1_mt);
  tree->SetBranchAddress("dilep_pt", &dilep_pt);
  tree->SetBranchAddress("dilep_eta", &dilep_eta);
  tree->SetBranchAddress("dilep_phi", &dilep_phi);
  tree->SetBranchAddress("dilep_mass", &dilep_mass);
  tree->SetBranchAddress("dilep_mt", &dilep_mt);
  tree->SetBranchAddress("dilep_deta", &dilep_deta);
  tree->SetBranchAddress("dilep_dphi", &dilep_dphi);
  tree->SetBranchAddress("dilep_dR", &dilep_dR);
  tree->SetBranchAddress("dilep_ptratio", &dilep_ptratio);
  tree->SetBranchAddress("HT", &HT);
  tree->SetBranchAddress("LT", &LT);
  tree->SetBranchAddress("STvis", &STvis);
  tree->SetBranchAddress("ST", &ST);
  tree->SetBranchAddress("HTMETllpt", &HTMETllpt);
  tree->SetBranchAddress("STfrac", &STfrac);
  tree->SetBranchAddress("metpt", &metpt);
  tree->SetBranchAddress("metphi", &metphi);
  tree->SetBranchAddress("dphi_metlep0", &dphi_metlep0);
  tree->SetBranchAddress("dphi_metlep1", &dphi_metlep1);
  tree->SetBranchAddress("dphi_metdilep", &dphi_metdilep);
  tree->SetBranchAddress("dphi_metlep_max", &dphi_metlep_max);
  tree->SetBranchAddress("dphi_metlep_min", &dphi_metlep_min);
  tree->SetBranchAddress("wt_leptonSF", &wt_leptonSF);
  tree->SetBranchAddress("wt_trig", &wt_trig);
  tree->SetBranchAddress("wt_pileup", &wt_pileup);
  tree->SetBranchAddress("wt_bjet", &wt_bjet);
  tree->SetBranchAddress("weight", &weight);
  //tree->SetBranchAddress("nnscore_qcd_vlld_2016preVFP",  &nnscore1);
  //tree->SetBranchAddress("nnscore_qcd_vlld_2016postVFP", &nnscore2);
  //tree->SetBranchAddress("nnscore_qcd_vlld_2017",        &nnscore3);
  //tree->SetBranchAddress("nnscore_qcd_vlld_2018",        &nnscore4);
  //tree->SetBranchAddress("nnscore_ttbar_vlld",           &nnscore5);
}

void SetLastBinAsOverflow(TH1D *hst){
  
  //Handle overlow:
  int lastBin = hst->GetNbinsX();
  double content  = hst->GetBinContent(lastBin);
  double error    = hst->GetBinError(lastBin);
  double overflow = hst->GetBinContent(lastBin + 1);
  double overflow_err = hst->GetBinError(lastBin + 1);
  double updated_content = content+overflow;
  double updated_error = std::sqrt(error * error + overflow_err * overflow_err);
  hst->SetBinContent(lastBin, updated_content);
  hst->SetBinError(lastBin, updated_error);

  //Handle underflow:
  double content_first  = hst->GetBinContent(1);
  double error_first    = hst->GetBinError(1);
  double underflow      = hst->GetBinContent(0);
  double underflow_err  = hst->GetBinError(0);
  double updated_content_first = content_first + underflow;
  double updated_error_first = std::sqrt(error_first*error_first + underflow_err*underflow_err);
  hst->SetBinContent(1, updated_content_first);
  hst->SetBinError(1, updated_error_first);

  // Resetting underflow and overflow bins to avoid multiple calling of this function.
  hst->SetBinContent(lastBin + 1, 0);
  hst->SetBinError(lastBin + 1, 0);
  hst->SetBinContent(0, 0);
  hst->SetBinError(0, 0);
  
}

#endif //SETBRANCHESANDHISTOGRAMS_H
