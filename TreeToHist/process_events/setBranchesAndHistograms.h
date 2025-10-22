#ifndef SETBRANCHESANDHISTOGRAMS_H
#define SETBRANCHESANDHISTOGRAMS_H

#include <vector>
#include <TString.h>
#include <TTree.h>
#include <TH1F.h>
using namespace std;
#include <nlohmann/json.hpp>
using json = nlohmann::json;

extern Int_t channel, trigger, nlep, njet, nbjet, nfatjet;
extern Float_t lep0_pt, lep0_eta, lep0_phi, lep0_iso, lep0_sip3d, lep0_mt;
extern Float_t lep1_pt, lep1_eta, lep1_phi, lep1_iso, lep1_sip3d, lep1_mt;
extern Float_t dilep_pt, dilep_eta, dilep_phi, dilep_mass, dilep_mt, dilep_deta, dilep_dphi, dilep_dR, dilep_ptratio;
extern Float_t HT, LT, STvis, ST, HTMETllpt, STfrac, metpt, metphi;
extern Float_t HTfat, STvisfat, STfat, HTfatMETllpt;
extern Float_t LTplusMET, HTplusMET, HTfatplusMET;
extern Float_t dphi_metlep0, dphi_metlep1, dphi_metdilep, dphi_metlep_max, dphi_metlep_min;
extern Float_t nnscore11, nnscore12, nnscore13, nnscore14, nnscore15, nnscore16, nnscore17, nnscore18;
extern Float_t nnscore21, nnscore22, nnscore23, nnscore24;
extern Double_t gen_weight_evt, lumi_weight_evt;
extern Double_t wt_leptonSF, wt_trig, wt_pileup, wt_bjet, wt_pdf, wt_qcdscale;
extern Double_t wt_leptonSF_up, wt_trig_up, wt_pileup_up, wt_bjet_up, wt_pdf_up, wt_qcdscale_up;
extern Double_t wt_leptonSF_down, wt_trig_down, wt_pileup_down, wt_bjet_down, wt_pdf_down, wt_qcdscale_down;

void setBranches(TTree *tree)
{
  tree->SetBranchAddress("channel", &channel);
  tree->SetBranchAddress("trigger", &trigger);
  tree->SetBranchAddress("nlep", &nlep);
  tree->SetBranchAddress("njet", &njet);
  tree->SetBranchAddress("nfatjet", &nfatjet);
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
  tree->SetBranchAddress("LT", &LT);
  tree->SetBranchAddress("LTplusMET", &LTplusMET);
  tree->SetBranchAddress("HT", &HT);
  tree->SetBranchAddress("HTfat", &HTfat);
  tree->SetBranchAddress("HTplusMET", &HTplusMET);
  tree->SetBranchAddress("HTfatplusMET", &HTfatplusMET);
  tree->SetBranchAddress("STvis", &STvis);
  tree->SetBranchAddress("STvisfat", &STvisfat);
  tree->SetBranchAddress("ST", &ST);
  tree->SetBranchAddress("STfat", &STfat);
  tree->SetBranchAddress("HTMETllpt", &HTMETllpt);
  tree->SetBranchAddress("HTfatMETllpt", &HTfatMETllpt);
  tree->SetBranchAddress("STfrac", &STfrac);
  tree->SetBranchAddress("metpt", &metpt);
  tree->SetBranchAddress("metphi", &metphi);
  tree->SetBranchAddress("dphi_metlep0", &dphi_metlep0);
  tree->SetBranchAddress("dphi_metlep1", &dphi_metlep1);
  tree->SetBranchAddress("dphi_metdilep", &dphi_metdilep);
  tree->SetBranchAddress("dphi_metlep_max", &dphi_metlep_max);
  tree->SetBranchAddress("dphi_metlep_min", &dphi_metlep_min);
  // Event weights
  tree->SetBranchAddress("gen_weight_evt",   &gen_weight_evt);
  tree->SetBranchAddress("lumi_weight_evt",  &lumi_weight_evt);
  // Corrections: Nominal
  tree->SetBranchAddress("wt_leptonSF",   &wt_leptonSF);
  tree->SetBranchAddress("wt_trig",       &wt_trig);
  tree->SetBranchAddress("wt_pileup",     &wt_pileup);
  tree->SetBranchAddress("wt_bjet",       &wt_bjet);
  tree->SetBranchAddress("wt_pdf",        &wt_pdf);
  tree->SetBranchAddress("wt_qcdscale",   &wt_qcdscale);
  // Up variations
  tree->SetBranchAddress("wt_leptonSF_up",   &wt_leptonSF_up);
  tree->SetBranchAddress("wt_trig_up",       &wt_trig_up);
  tree->SetBranchAddress("wt_pileup_up",     &wt_pileup_up);
  tree->SetBranchAddress("wt_bjet_up",       &wt_bjet_up);
  tree->SetBranchAddress("wt_pdf_up",        &wt_pdf_up);
  tree->SetBranchAddress("wt_qcdscale_up",   &wt_qcdscale_up);
  // Down variations
  tree->SetBranchAddress("wt_leptonSF_down", &wt_leptonSF_down);
  tree->SetBranchAddress("wt_trig_down",     &wt_trig_down);
  tree->SetBranchAddress("wt_pileup_down",   &wt_pileup_down);
  tree->SetBranchAddress("wt_bjet_down",     &wt_bjet_down);
  tree->SetBranchAddress("wt_pdf_down",      &wt_pdf_down);
  tree->SetBranchAddress("wt_qcdscale_down", &wt_qcdscale_down);
  //nnscores
  tree->SetBranchAddress("nnscore_2LSS_Run2_vlld_qcd",   &nnscore11);
  tree->SetBranchAddress("nnscore_2LSS_Run2_vlld_ttbar", &nnscore12);
  tree->SetBranchAddress("nnscore_2LSS_Run2_vlld_wjets", &nnscore13);
  tree->SetBranchAddress("nnscore_2LSS_Run2_vlld_dy",    &nnscore14);
  tree->SetBranchAddress("nnscore_2LSS_Run3_vlld_qcd",   &nnscore15);
  tree->SetBranchAddress("nnscore_2LSS_Run3_vlld_ttbar", &nnscore16);
  tree->SetBranchAddress("nnscore_2LSS_Run3_vlld_wjets", &nnscore17);
  tree->SetBranchAddress("nnscore_2LSS_Run3_vlld_dy",    &nnscore18);
  tree->SetBranchAddress("nnscore_2LOS_Run2_vlld_ttbar", &nnscore21);
  tree->SetBranchAddress("nnscore_2LOS_Run2_vlld_dy",    &nnscore22);
  tree->SetBranchAddress("nnscore_2LOS_Run3_vlld_ttbar", &nnscore23);
  tree->SetBranchAddress("nnscore_2LOS_Run3_vlld_dy",    &nnscore24);
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

//--------------------------------------------------------------------------------------------
// Additional functions:
json loadJson(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "\033[31mError: could not open file " << filename << "\033[0m" << endl;
        return nullptr;  // return null JSON
    }
    json j;
    try { file >> j; }
    catch (const std::exception &e) {
        cerr << "\033[31mError parsing JSON: " << e.what() << "\033[0m" << endl;
        return nullptr;
    }
    cout<<"Loaded json file: "<<filename<<endl;
    return j;
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

#endif //SETBRANCHESANDHISTOGRAMS_H
