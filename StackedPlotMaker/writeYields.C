#include "include_headers.h"
#include "utilities/decorations.h"
#include "utilities/functions.h"
#include "utilities/read_hists.h"

//Global parameters:
float globalSbyB, globalSbyBErr, globalObsbyExp, globalObsbyExpErr;

void writeYields(
		 TString _var = "dilep_pt",
		 TString _name = "p_{T}^{LL} (GeV)",
		 TString _jobname = "2025-07-03_sr2/hist_Run3Summer22EE_sr2_mm/",
		 TString _campaign = "Run3Summer22EE",
		 TString _channel = "mm",
		 TString _tag = "sr",
		 TString _displaytext = "Signal region"
		 )
{
  TString date_stamp  = todays_date();
  bool toOverlayData=false; //Careful!

  TString dump_folder = "signalyields/"+date_stamp+"/yields_"+_campaign+"_"+_channel;
  TString input_path = "../ROOT_FILES/hists/"+_jobname;
  
  //--------------------------------------------------------------------------
  vector<TH1D *> hist_collection = return_hist_collection(_var, input_path, _campaign);
  combine_hists(hist_collection, {"WWW", "WWZ", "WZZ", "ZZZ"},   "VVV", kGreen+3);
  combine_hists(hist_collection, {"WW", "WZ", "ZZ"},             "VV", kGreen+1);
  combine_hists(hist_collection, {"QCD (#mu)", "QCD (e#gamma)"}, "QCD", kYellow);
  combine_hists(hist_collection, {"t#bar{t}", "t#bar{t}V", "t#bar{t}W", "t#bar{t}Z"}, "t#bar{t}+x", kAzure+1);
  //combine_hists(hist_collection, {"t#bar{t}", "t#bar{t}V"},      "t#bar{t}+x", kAzure+1);
  combine_hists(hist_collection, {"tX", "tW"},                   "Single t", kCyan-7);
  combine_hists(hist_collection, {"W+jets", "W#gamma"},          "W+jets/#gamma", kGray+2);
  cout<<"Histogram collection read."<<endl;

  //______________________________________________________________
  
  //                           DATA
  //______________________________________________________________
  
  vector<TH1D*> data_collection; data_collection.clear();
  if(_campaign == "2016preVFP_UL"){
    vector<string> eras = {"B", "C", "D", "E", "F"};
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "EGamma", "EGamma_" + era));
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "SingleMuon", "SingleMuon_" + era));
  }
  if(_campaign == "2016postVFP_UL"){
    vector<string> eras = {"F", "G", "H"};
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "EGamma", "EGamma_" + era));
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "SingleMuon", "SingleMuon_" + era));
  }
  if(_campaign == "2017_UL"){
    vector<string> eras = {"B", "C", "D", "E", "F"};
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "EGamma", "EGamma_" + era));
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "SingleMuon", "SingleMuon_" + era));
  }
  if(_campaign == "2018_UL"){
    vector<string> eras = {"A", "B", "C", "D"};
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "EGamma", "EGamma_" + era));
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "SingleMuon", "SingleMuon_" + era));
  }
  if(_campaign == "Run3Summer22"){
    vector<string> eras = {"C", "D"};
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "EGamma", era));
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "Muon", era));
  }
  if(_campaign == "Run3Summer22EE"){
    vector<string> eras = {"E", "F", "G"};
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "EGamma", era));
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "Muon", era));
  }
  if(_campaign == "Run3Summer23"){
    vector<string> eras = {"C1", "C2", "C3", "C4"};
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "EGamma0", era));
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "EGamma1", era));
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "Muon0", era));
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "Muon1", era));
  }
  if(_campaign == "Run3Summer23BPix"){
    vector<string> eras = {"D1", "D2"};
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "EGamma0", era));
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "EGamma1", era));
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "Muon0", era));
    for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "Muon1", era));
  }
  
  TH1D* hst_data  = nullptr;
  TH1D* hst_smuon = nullptr;
  TH1D* hst_egamma = nullptr;
  for (auto* h : data_collection) {
    if (!h) continue;
    SetLastBinAsOverflow(h);
    SetHistoStyle(h, kBlack);
    TString name = h->GetName();
    if (name.Contains("EGamma")) {
      if (!hst_egamma) hst_egamma = (TH1D*)h->Clone("EGamma");
      else hst_egamma->Add(h);
    }
    else if (name.Contains("Muon")) {
      if (!hst_smuon) hst_smuon = (TH1D*)h->Clone("Muon");
      else hst_smuon->Add(h);
    }
    if (!hst_data) hst_data = (TH1D*)h->Clone("Data");
    else hst_data->Add(h);
  }
  cout << "Data ready!" << endl;

  //______________________________________________________________

  //        BACKGROUNDS: Sorting, addding and stacking
  //______________________________________________________________
  //Extract backgrounds from the collection:
  vector<TH1D*> bkg; bkg.clear();
  for (auto* h : hist_collection) {
    if (!h) continue;
    TString name = h->GetName();
    if (!name.BeginsWith("Data")) bkg.push_back(h);
  }
  std::sort(bkg.begin(), bkg.end(), [](TH1D* a, TH1D* b) {
    return a->Integral() < b->Integral();
  });

  //Summing over the backgrounds:
  TH1D *hst_bkg = (TH1D *)bkg[0]->Clone(); hst_bkg->Reset();
  for(int i=0; i<(int)bkg.size(); i++) hst_bkg->Add(bkg[i]);
  hst_bkg->SetFillStyle(3013);
  hst_bkg->SetFillColor(kGray+3);
  hst_bkg->SetMarkerSize(0); //No marker
  hst_bkg->SetName("bkg");
  cout<<"Backgrounds ready!"<<endl;

  createFolder(dump_folder);
  cout<<"\nFolder created: "+dump_folder<<endl;

  //______________________________________________________________
  
  //                    Iterating over signal
  //______________________________________________________________
  
  vector<pair<TString, TString>> vlld = {
    {"VLLD_mu", "M100"},
    {"VLLD_mu", "M200"},
    {"VLLD_mu", "M300"},
    {"VLLD_mu", "M400"},
    {"VLLD_mu", "M600"},
    {"VLLD_mu", "M800"},
    {"VLLD_ele", "M100"},
    {"VLLD_ele", "M200"},
    {"VLLD_ele", "M300"},
    {"VLLD_ele", "M400"},
    {"VLLD_ele", "M600"},
    {"VLLD_ele", "M800"},
    {"VLLD_ele", "M1000"}
  };

  int count = 0;
  for (const auto& entry : vlld){
    TString sample = entry.first;
    TString subsample = entry.second;
    TString filename = dump_folder+"/yield_"+sample+"_"+subsample+".txt";

    TH1D* hist = get_hist(_var, input_path, sample, subsample);
    if (!hist) {cerr<<"Failed to retrieve histogram for "<<sample<< ", " <<subsample<<endl; continue;}

    count = count+1;
    int nbins = hist->GetNbinsX();
    cout <<"Yields for "<<sample<<" "<<subsample<<" : \033[93m"<<filename<<"\033[0m"<<endl;

    //Test output:
    if(count == 1){
      cout <<"\n"<< string(80, '-') << endl;
      cout << setw(8) << "Bin" 
	   << setw(12) << "Nsig" 
	   << setw(12) << "Ndata" 
	   << setw(12) << "Nbkg" 
	   << setw(12) << "NbkgErr" 
	   << setw(12) << "S/B"
	   << setw(12) << "dbkg"
	   << endl;
      cout << string(80, '-') << endl;
    }

    //Create a text file for each sample
    ofstream outfile(filename.Data());
    if (!outfile.is_open()) {cerr << "Failed to open file: " << filename << endl; continue;}

    //Iterating over bins:
    for (int bin = 1; bin <= nbins; ++bin) {
      float nsig = hist->GetBinContent(bin);
      float nsigerr = hist->GetBinError(bin);
      float nbkg = hst_bkg->GetBinContent(bin);
      float nbkgerr = hst_bkg->GetBinError(bin);
      int ndata = 0;
      if(toOverlayData) ndata = (int)hst_data->GetBinContent(bin); //Careful!
      else              ndata = (int)hst_bkg->GetBinContent(bin);
      float sbyb = 0;
      float relative_bkgerr = 0;
      float dbkg = 0;
      if(nbkg!=0){
	sbyb = nsig / sqrt(nbkg);
	relative_bkgerr = nbkgerr/nbkg;
	dbkg = 1+relative_bkgerr;
      }
      //Test output:
      if(count == 1){
	cout << setw(8) << bin
	     << setw(12) << nsig
	     << setw(12) << ndata
	     << setw(12) << nbkg
	     << setw(12) << nbkgerr
	     << setw(12) << sbyb
	     << setw(12) << dbkg
	     << endl;
      }
      // Write to file:
      outfile << setw(8) << bin
	      << setw(12) << nsig
	      << setw(12) << ndata
	      << setw(12) << nbkg
	      << setw(12) << nbkgerr
	      << setw(12) << sbyb
	      << setw(12) << dbkg
	      << endl;
    }//bin loop

    if(count == 1) cout<<""<<endl;
    outfile.close();
    //break; //sample
    
  }//signal loop

  cout<<"\nDone! Files saved to: "<<dump_folder<<endl;
}
