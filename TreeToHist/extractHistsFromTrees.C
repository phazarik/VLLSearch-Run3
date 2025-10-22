#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <filesystem>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <nlohmann/json.hpp>
#include "process_events/setBranchesAndHistograms.h"
//#include "process_events/eventProcessor_2LSS.h"
#include "process_events/eventProcessor_2LOS.h"
using namespace std;
namespace fs = std::filesystem;
using json = nlohmann::json;

Int_t channel, trigger, nlep, njet, nbjet, nfatjet;
Float_t lep0_pt, lep0_eta, lep0_phi, lep0_iso, lep0_sip3d, lep0_mt;
Float_t lep1_pt, lep1_eta, lep1_phi, lep1_iso, lep1_sip3d, lep1_mt;
Float_t dilep_pt, dilep_eta, dilep_phi, dilep_mass, dilep_mt, dilep_deta, dilep_dphi, dilep_dR, dilep_ptratio;
Float_t HT, LT, STvis, ST, HTMETllpt, STfrac, metpt, metphi;
Float_t HTfat, STvisfat, STfat, HTfatMETllpt;
Float_t LTplusMET, HTplusMET, HTfatplusMET;
Float_t dphi_metlep0, dphi_metlep1, dphi_metdilep, dphi_metlep_max, dphi_metlep_min;
Float_t nnscore11, nnscore12, nnscore13, nnscore14, nnscore15, nnscore16, nnscore17, nnscore18;
Float_t nnscore21, nnscore22, nnscore23, nnscore24;
Double_t gen_weight_evt, lumi_weight_evt;
Double_t wt_leptonSF, wt_trig, wt_pileup, wt_bjet, wt_pdf, wt_qcdscale;
Double_t wt_leptonSF_up, wt_trig_up, wt_pileup_up, wt_bjet_up, wt_pdf_up, wt_qcdscale_up;
Double_t wt_leptonSF_down, wt_trig_down, wt_pileup_down, wt_bjet_down, wt_pdf_down, wt_qcdscale_down;

//json loadJson(const string &filename);
//string todays_date();

//External functions:
void setBranches(TTree *tree);
void SetLastBinAsOverflow(TH1D *hst);
void processTree(
		 const char* inputFilename,
		 const char* outputFilename,
		 const char* campaign,
		 int channelval,
		 float lumisf);

//________________________________________________________________________________________________________________
//
// Process all files for a  given jobname:
//________________________________________________________________________________________________________________

void extractHistsFromTrees(
			   const std::string& jobname  = "baseline/tree_baseline_2016preVFP_UL",
			   const std::string& dump     = "hist_baseline_2016preVFP_UL",
			   const std::string& campaign = "2016preVFP_UL",
			   const std::string& channel  = "mm",
			   bool test   = false,
			   bool dryrun = false)
{
  if (channel == "em" or channel == "me") return; //For 2LOS
  
  // Input and output directories
  string date_stamp  = todays_date();
  string indir = "../ROOT_FILES/treesWithNN/" + jobname;
  string outdir = "../ROOT_FILES/hists/"+date_stamp+"/"+dump;
  fs::create_directories(outdir);
  auto start_time = chrono::high_resolution_clock::now();
  
  //Load luminosity file:
  string lumifile;
  float datalumi = 0.0;
  lumifile = "../sample_weights_JSON/lumidata_" + campaign + ".json";
  if      (campaign == "2016preVFP_UL")    datalumi = 19692.0;
  else if (campaign == "2016postVFP_UL")   datalumi = 16227.0;
  else if (campaign == "2017_UL")          datalumi = 41480.0;
  else if (campaign == "2018_UL")          datalumi = 59830.0;
  else if (campaign == "Run3Summer22")     datalumi =  7980.4;
  else if (campaign == "Run3Summer22EE")   datalumi = 26671.7;
  else if (campaign == "Run3Summer23")     datalumi = 17794.0;
  else if (campaign == "Run3Summer23BPix") datalumi =  9451.0;
  else cout << "Provide correct campaign name!" << endl;

  // Load luminosity data
  auto samplelist = loadJson(lumifile);
  if (samplelist.is_null()) cout<<"\033[31mFailed to load JSON: "<<lumifile<<"\033[0m"<<endl;

  // Determine channel value
  int chval = -1;
  if (channel == "mm") chval = 0;
  else if (channel == "me") chval = 1;
  else if (channel == "em") chval = 2;
  else if (channel == "ee") chval = 3;
  else {
    cerr << "Pick one: ee, em, me or mm.\n";
    return;
  }

  //---------------------------------------
  // Loop over files in the input directory
  //---------------------------------------
  auto prev_time = start_time;
  vector<string> list_processed, list_failed;

  for (const auto &entry : fs::directory_iterator(indir)) {

    string treefile = entry.path().filename().string();
    if (treefile.find(".root") == string::npos) continue;
    
    cout << "\n\033[033mProcessing " << treefile << "\033[0m" <<endl;

    string histfile = treefile;
    histfile.replace(histfile.find("tree"), 4, "hst");

    string infilepath = indir + "/" + treefile;
    string outfilepath = outdir + "/" + histfile;

    // Extract luminosity
    float lumi = 0.0;

    bool found_match = false;
    for (auto it = samplelist.begin(); it != samplelist.end(); ++it) {
      const auto& sample = it.key();
      const auto& subs = it.value();
      if (treefile.find(sample) == string::npos) continue;
      for (auto sub_it = subs.begin(); sub_it != subs.end(); ++sub_it) {
        const auto& subsample = sub_it.key();
        const auto& val = sub_it.value();
	string target = sample + "_" + subsample + ".root";
        //if (treefile.find(sample + "_" + subsample) != string::npos ||
	//    treefile.find(sample + subsample) != string::npos) {
	if (treefile.find(target) != string::npos){
	  cout<<"Target = "<<target<<", lumi = "<<val<<endl;
	  lumi = val;
	  found_match = true;
	  break;
        }
      }
      if (found_match) break; // No need to check other samples if matched
    }
    if (!found_match) {
      cerr << "\033[31m[Warning]\033[0m No matching sample+subsample found for file: " << treefile << endl;
      continue;
    }

    float lumisf = datalumi / lumi;
    if (treefile.find("Muon") != string::npos || treefile.find("EGamma") != string::npos) {
      lumisf = 1.0;
    }
    //cout << "Luminosity = " << lumi << ", scalefactor = " << fixed << setprecision(6) << lumisf << endl;

    //------------------------------------------------------------------------------------------
    // Main function
    if(dryrun) cout<<"Arguments: "<<infilepath <<"\t"<< outfilepath <<"\t"<< campaign <<"\t"<< chval <<"\t"<< lumisf <<endl;
    else  processTree(infilepath.c_str(), outfilepath.c_str(), campaign.c_str(), chval, lumisf, test);
    //------------------------------------------------------------------------------------------
    
    auto current_time = chrono::high_resolution_clock::now();
    cout<<"Time taken for this sample = "<<chrono::duration_cast<chrono::seconds>(current_time - prev_time).count()<<" second(s)"<<endl;
    prev_time = current_time;

    if(test) break;
    
  }//File loop ends.
    
  auto end_time = chrono::high_resolution_clock::now();
  auto time_taken = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();

  cout << "\n\033[93mTime taken for the job: " << time_taken << " seconds.\n\033[0m"<<endl;

  if (!list_failed.empty()) {
    cerr << "\033[031m\nWarning: The following files were skipped :\n\033[0m"<<endl;
    for (const auto &file : list_failed) cerr << file << ", ";
    cerr << endl;
  }
}

