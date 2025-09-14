/*
This script is borrowed from StackedPlotMaker
 */

#include "../StackedPlotMaker/include_headers.h"
#include "../StackedPlotMaker/utilities/decorations.h"
#include "../StackedPlotMaker/utilities/functions.h"
#include "../StackedPlotMaker/utilities/read_hists.h"

#include <sstream>
string fmt_val_err(double val, double err, int prec=3) {
    ostringstream ss;
    ss << fixed << setprecision(prec) << val << " ± " << fixed << setprecision(prec) << err;
    return ss.str();
}
string fmt_val(double val, int prec=3) {
    ostringstream ss;
    ss << fixed << setprecision(prec) << val;
    return ss.str();
}

void writeYieldsOneCampaign(TString _var, TString _name, TString _jobname,
			    TString _campaign, TString _channel, TString _tag, TString _displaytext,
			    bool _data, bool _save);

//Global parameters:
float globalSbyB, globalSbyBErr, globalObsbyExp, globalObsbyExpErr;

void writeYields(
		 TString _var = "LTplusMET",
		 TString _name = "LT+MET",
		 TString _jobname = "2025-09-10_sr_ttbar-systdown",
		 TString _tag = "sr_ttbar-systdown",
		 TString _displaytext = "val",
		 bool _data = false, //careful!
		 bool _save = true
		 ){
  
  cout << "Processing the following jobs:" << endl;
  vector<TString> campaigns = {
    "2016preVFP_UL", "2016postVFP_UL", "2017_UL",      "2018_UL",
    "Run3Summer22",  "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix",
    "Run2", "Run3", "FullDataset"
  };
  vector<TString> channels = {"mm", "me", "em", "ee", "combined"};
  vector<tuple<TString, TString, TString>> metadata;
  for (auto &campaign : campaigns) {
    for (auto &channel : channels) {
      if((campaign == "Run2" || campaign=="Run3") && channel != "combined") continue;
      TString path = _jobname + "/hist_" + _tag + "_" + campaign + "_" + channel;
      if (!gSystem->AccessPathName("../ROOT_FILES/hists/"+path)) {
	metadata.push_back(make_tuple(path, campaign, channel));
	cout << " - " << path << endl;
      }
      else cout << " Not found: " << path << endl;
    }
  }
 
  for(auto &entry : metadata){
    TString jobname  = get<0>(entry);
    TString campaign = get<1>(entry);
    TString channel  = get<2>(entry);

    //Exceptions:
    if((campaign == "Run2" || campaign == "Run3") && channel != "combined") continue;
    
    writeYieldsOneCampaign(_var, _name, jobname, campaign, channel, _tag, _displaytext, _data, _save);
    
  }
  
}

void writeYieldsOneCampaign(TString _var, TString _name, TString _jobname,
			    TString _campaign, TString _channel, TString _tag, TString _displaytext,
			    bool _data, bool _save){
  TString date_stamp  = todays_date();
  bool toOverlayData=_data;
  bool toSave=_save;

  TString dump_folder ="yields/"+date_stamp+"/"+_tag;
  TString input_path = "../ROOT_FILES/hists/"+_jobname;
  
  //---------------------------------------------------------------------------------------------------
  vector<TH1D *> hist_collection = return_hist_collection(_var, input_path, _campaign);
  combine_hists(hist_collection, {"WWW", "WWZ", "WZZ", "ZZZ"},   "VVV", kGreen+3);
  combine_hists(hist_collection, {"WW", "WZ", "ZZ"},             "VV", kGreen+1);
  combine_hists(hist_collection, {"QCD (#mu)", "QCD (e#gamma)"}, "QCD", kYellow);
  combine_hists(hist_collection, {"t#bar{t}", "t#bar{t}V", "t#bar{t}W", "t#bar{t}Z"}, "t#bar{t}+x", kAzure+1);
  combine_hists(hist_collection, {"tX", "tW"},                   "Single t", kCyan-7);
  combine_hists(hist_collection, {"W+jets", "W#gamma"},          "W+jets/#gamma", kGray+2);
  cout<<"Histogram collection read."<<endl;

  //----------------------------------------------------------
  //    Preparing backgrounds: Sorting, addding and stacking
  //----------------------------------------------------------
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

  //-------------------------------
  //       Preparing data
  //-------------------------------

  TH1D* hst_data  = nullptr;
  TH1D* hst_smuon = nullptr;
  TH1D* hst_egamma = nullptr;
  vector<TH1D*> data_collection; data_collection.clear();

  if(_data){
    if(_campaign == "2016preVFP_UL"){
      vector<string> eras = {"B2", "C", "D", "E", "FHIPM"};
      for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "EGamma", era));
      for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "Muon", era));
    }
    if(_campaign == "2016postVFP_UL"){
      vector<string> eras = {"F", "G", "H"};
      for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "EGamma", era));
      for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "Muon", era));
    }
    if(_campaign == "2017_UL"){
      vector<string> eras = {"B", "C", "D", "E", "F"};
      for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "EGamma", era));
      for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "Muon", era));
    }
    if(_campaign == "2018_UL"){
      vector<string> eras = {"A", "B", "C", "D"};
      for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "EGamma", era));
      for(auto& era : eras) data_collection.push_back(get_hist(_var, input_path, "Muon", era));
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
    if (_campaign == "Run2") {
      vector<string> eras = {"A","B","B2","C","D","E","F","FHIPM","G","H"};
      for (auto& era : eras) {
	data_collection.push_back(get_hist(_var, input_path, "EGamma", era));
	data_collection.push_back(get_hist(_var, input_path, "Muon",   era));
      }
    }
    if (_campaign == "Run3") {
      vector<string> eras_2022 = {"C","D","E","F","G"};
      vector<string> eras_2023 = {"C1","C2","C3","C4","D1","D2"};
      for (auto& era : eras_2022) {
	data_collection.push_back(get_hist(_var, input_path, "EGamma",  era));
	data_collection.push_back(get_hist(_var, input_path, "Muon",    era));
      }
      for (auto& era : eras_2023){
	data_collection.push_back(get_hist(_var, input_path, "EGamma0", era));
	data_collection.push_back(get_hist(_var, input_path, "EGamma1", era));
	data_collection.push_back(get_hist(_var, input_path, "Muon0",   era));
	data_collection.push_back(get_hist(_var, input_path, "Muon1",   era));
      }
    }
    if (_campaign == "FullDataset") {
      vector<string> eras_Run2and2022 = {"A","B","B2","C","D","E","F","FHIPM","G","H"};
      vector<string> eras_2023  = {"C1","C2","C3","C4","D1","D2"};
      for (auto& era : eras_Run2and2022) {
	data_collection.push_back(get_hist(_var, input_path, "EGamma", era));
	data_collection.push_back(get_hist(_var, input_path, "Muon",   era));
      }
      for (auto& era : eras_2023) {
	data_collection.push_back(get_hist(_var, input_path, "EGamma0", era));
	data_collection.push_back(get_hist(_var, input_path, "EGamma1", era));
	data_collection.push_back(get_hist(_var, input_path, "Muon0",   era));
	data_collection.push_back(get_hist(_var, input_path, "Muon1",   era));
      }
    }
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
  }
  else{
    //For expected limits.
    cout<<"Setting data = (int)bkg"<<endl;
    hst_data = hst_bkg;
  }
  cout << "Data ready!" << endl;

  //--------------------
  // Preparing signal
  //--------------------
  
  vector<pair<TString, TString>> vlld = {
    {"VLLD-mu", "100"},
    {"VLLD-mu", "200"},
    {"VLLD-mu", "300"},
    {"VLLD-mu", "400"},
    {"VLLD-mu", "600"},
    {"VLLD-mu", "800"},
    {"VLLD-mu", "1000"},
    {"VLLD-ele", "100"},
    {"VLLD-ele", "200"},
    {"VLLD-ele", "300"},
    {"VLLD-ele", "400"},
    {"VLLD-ele", "600"},
    {"VLLD-ele", "800"},
    {"VLLD-ele", "1000"}
  };
  vector<TH1D*> sig_hists;
  for (auto& entry : vlld) {
    TH1D* h = get_hist(_var, input_path, entry.first, entry.second);
    if(!h) continue;
    h->SetName(entry.first+"_"+entry.second);
    if (h) sig_hists.push_back(h);
  }
  cout << "Signal ready!" << endl;
  
  createFolder(dump_folder);
  cout<<"\nFolder created: "+dump_folder<<endl;

  //-------------------------
  //  Writing yields as TSV
  //-------------------------
  TString csvfile = dump_folder + "/yields_"+_tag+"_"+_campaign+"_"+_channel+".csv";
  ofstream out(csvfile.Data());

  // header
  out << "nbin,nData,TotalBkg";
  for (auto it = bkg.rbegin(); it != bkg.rend(); ++it) out << "," << (*it)->GetName();
  for (size_t i=0; i<sig_hists.size(); i++) out << "," << sig_hists[i]->GetName();
  out << "\n";

  // bins
  int nbins = hst_bkg->GetNbinsX();
  for (int bin=1; bin<=nbins; ++bin) {
    out << bin;
    out << "," << (int)round(hst_data->GetBinContent(bin));
    out << "," << fmt_val_err(hst_bkg->GetBinContent(bin), hst_bkg->GetBinError(bin), 3);
    for (auto it = bkg.rbegin(); it != bkg.rend(); ++it) {
      TH1* h = *it;
      out << "," << fmt_val_err(h->GetBinContent(bin), h->GetBinError(bin), 3);
    }
    for (auto* s : sig_hists) {
      out << "," << fmt_val_err(s->GetBinContent(bin), s->GetBinError(bin), 3);
    }
    out << "\n";
  }

  // totals
  out << "Total";
  out << "," << (int)round(hst_data->Integral(1, hst_data->GetNbinsX()));
  {
    double err = 0;
    double val = hst_bkg->IntegralAndError(1, hst_bkg->GetNbinsX(), err);
    out << "," << fmt_val_err(val, err, 3);
  }
  for (auto it = bkg.rbegin(); it != bkg.rend(); ++it) {
    double err = 0;
    double val = (*it)->IntegralAndError(1, (*it)->GetNbinsX(), err);
    out << "," << fmt_val_err(val, err, 3);
  }
  for (auto* s : sig_hists) {
    double err = 0;
    double val = s->IntegralAndError(1, s->GetNbinsX(), err);
    out << "," << fmt_val_err(val, 3);
  }
  out << "\n";

  cout<<"\nDone! File created: \033[1;33m"<<csvfile<<"\033[0m"<<endl;
 
}
