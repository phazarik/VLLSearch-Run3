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
#include "setBranchesAndHistograms.h"
#include "eventProcessor.h"
using namespace std;
namespace fs = std::filesystem;
using json = nlohmann::json;

Int_t channel, trigger, nlep, njet, nbjet;
Float_t lep0_pt, lep0_eta, lep0_phi, lep0_iso, lep0_sip3d, lep0_mt;
Float_t lep1_pt, lep1_eta, lep1_phi, lep1_iso, lep1_sip3d, lep1_mt;
Float_t dilep_pt, dilep_eta, dilep_phi, dilep_mass, dilep_mt, dilep_deta, dilep_dphi, dilep_dR, dilep_ptratio;
Float_t HT, LT, STvis, ST, HTMETllpt, STfrac, metpt, metphi;
Float_t dphi_metlep0, dphi_metlep1, dphi_metdilep, dphi_metlep_max, dphi_metlep_min;
Float_t nnscore1, nnscore2, nnscore3, nnscore4, nnscore5, nnscore6;
Double_t wt_leptonSF, wt_trig, wt_pileup, wt_bjet, weight;

map<string, map<string, float>> loadJson(const string &filename);
string todays_date();

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
			   const std::string& jobname  = "baseline/tree_Run3Summer22_baseline",
			   const std::string& dump     = "hist_Run3Summer22_baseline_ee",
			   const std::string& campaign = "Run3Summer22",
			   const std::string& channel  = "ee",
			   bool test   = false,
			   bool dryrun = false)
{
  // Input and output directories
  string date_stamp  = todays_date();
  string indir = "../ROOT_FILES/treesWithNN/" + jobname;
  string outdir = "../ROOT_FILES/hists/"+date_stamp+"/"+dump;
  fs::create_directories(outdir);
  auto start_time = chrono::high_resolution_clock::now();
  
  //Load luminosity file:
  string lumifile;
  float datalumi = 0.0;
  if (campaign == "2016preVFP_UL"){
    lumifile = "../LumiJsons/lumidata_2016preVFP_UL.json";
    datalumi = 19692;
  }
  else if (campaign == "2016postVFP_UL") {
    lumifile = "../LumiJsons/lumidata_2016postVFP_UL.json";
    datalumi = 16227;
  }
  else if (campaign == "2017_UL") {
    lumifile = "../LumiJsons/lumidata_2017_UL.json";
    datalumi = 41480;
  }
  else if (campaign == "2018_UL") {
    lumifile = "../LumiJsons/lumidata_2018_UL.json";
    datalumi = 59830;
  }
  else if (campaign == "Run3Summer22") {
    lumifile = "../LumiJsons/lumidata_Run3Summer22.json";
    datalumi = 7980.4;
  }
  else cout<<"Provide correct campaign name!"<<endl;

  // Load luminosity data
  auto samplelist = loadJson(lumifile);

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

    //if (treefile.find("tW_AntiTop_InclusiveDecays") == string::npos &&
    //    treefile.find("WZTo2L2Q") == string::npos) continue;
    
    cout << "\n\033[033mProcessing " << treefile << "\033[0m" <<endl;

    string histfile = treefile;
    histfile.replace(histfile.find("tree"), 4, "hst");

    string infilepath = indir + "/" + treefile;
    string outfilepath = outdir + "/" + histfile;

    // Extract luminosity
    float lumi = 0.0;
    for (const auto &[sample, subs] : samplelist) {
      if (treefile.find(sample) == string::npos) continue;

      for (const auto &[subsample, val] : subs) {
	if (treefile.find(subsample) != string::npos) {
	  lumi = val;
	  break;
	}
      }
    }

    if (lumi > 0.0) list_processed.push_back(treefile);
    else            list_failed.push_back(treefile);

    float lumisf = datalumi / lumi;
    if (treefile.find("SingleMuon") != string::npos || treefile.find("EGamma") != string::npos) {
      lumisf = 1.0;
    }
    //cout << "Luminosity = " << lumi << ", scalefactor = " << fixed << setprecision(6) << lumisf << endl;

    //------------------------------------------------------------------------------------------
    // Main function
    if(dryrun) cout<<"Arguments: "<<infilepath <<"\t"<< outfilepath <<"\t"<< campaign <<"\t"<< chval <<"\t"<< lumisf <<endl;
    else  processTree(infilepath.c_str(), outfilepath.c_str(), campaign.c_str(), chval, lumisf);
    //------------------------------------------------------------------------------------------
    
    auto current_time = chrono::high_resolution_clock::now();
    cout<<"Done! time taken = \033[033m"<<chrono::duration_cast<chrono::seconds>(current_time - prev_time).count()<<" second(s)\033[0m"<<endl;
    prev_time = current_time;

    if(test) break;
    
  }//File loop ends.
    
  auto end_time = chrono::high_resolution_clock::now();
  auto time_taken = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();

  cout << "\033[093m\nSummary:\033[0m" <<endl;
  cout << "Files processed = "<<endl;
  for (const auto &file : list_processed) cout << file << ", ";
  cout << "\033[093m\nOutput directory: " << outdir << endl;
  cout << "Total time taken: " << time_taken << " seconds.\n\033[0m"<<endl;

  if (!list_failed.empty()) {
    cerr << "\033[031m\nWarning: The following files were skipped :\n\033[0m"<<endl;
    for (const auto &file : list_failed) cerr << file << ", ";
    cerr << endl;
  }
}

//--------------------------------------------------------------------------------------------
// Additional functions:
map<string, map<string, float>> loadJson(const string &filename) {
    ifstream file(filename);
    json j;
    file >> j;
    return j.get<map<string, map<string, float>>>();
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
