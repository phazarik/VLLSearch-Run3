#include "include_headers.h"
#include "utilities/decorations.h"
#include "utilities/functions.h"
#include "utilities/read_hists.h"

//Global parameters:
float globalSbyB, globalSbyBErr, globalObsbyExp, globalObsbyExpErr;

void makeStackedPlot(
		     //TString _var = "njet",
		     //TString _var = "nnscore_Run2_vlld_qcd",
		     //TString _var = "metpt",
		     //TString _var = "dilep_dR",
		     //TString _var = "dilep_mt",
		     //TString _var = "HT",
		     TString _var = "LTplusMET",
		     //TString _var = "gen_weight_evt",
		     TString _name = "test",
		     TString _jobname = "2025-10-17_cleanup/hist_2LOS_cleanup_Run3Summer22EE_ee",
		     TString _campaign = "Run3Summer22EE",
		     TString _channel = "ee",
		     TString _tag = "test",
		     TString _displaytext = "test",
		     bool _data = true,
		     bool _save = false
		     )
{
  TString date_stamp  = todays_date();
  TString channelname = "";
  if (_channel == "mm") channelname = "#mu#mu";
  if (_channel == "me") channelname = "#mue";
  if (_channel == "em") channelname = "e#mu";
  if (_channel == "ee") channelname = "ee";
  if (_channel == "combined") channelname = "combined";

  //--------------------------------------------------------------------------
  // SET GLOBAL SETTINGS 
  bool toOverlayData=_data;
  bool toSave=_save;
  // ymax_base = 10e9 for baseline 2LSS
  // ymax_base = 10e11 for baseline 2OSS
  Double_t ymin = 0.1; Double_t ymax_base = 10E8; Double_t ymax = ymax_base;
  if(_channel=="combined")                   ymax = ymax_base*10;
  if(_campaign=="Run2" or _campaign=="Run3") ymax = ymax_base*100; 
  if(_campaign=="FullDataset")               ymax = ymax_base*1000; 
  TString output_tag = _tag;
  TString info1 = _displaytext; //event-selection
  TString info2 = channelname + "-channel";
  if(_channel == "combined") info2 = "combined";
  
  //--------------------------------------------------------------------------
  TString dump_folder = "plots/"+date_stamp+"/"+output_tag+"_"+_campaign+"_"+_channel;  
  TString filename = dump_folder+"/"+_var;
  TString input_path = "../ROOT_FILES/hists/"+_jobname;
  vector<TH1D *> hist_collection = return_hist_collection(_var, input_path, _campaign);
  /*
  double total;
  cout<<"\nBefore combining:"<<endl;
  total = 0;
  for (int i = 0; i < (int)hist_collection.size(); i++){ 
    cout << left << setw(3) << i+1 << "  " 
         << left << setw(15) << hist_collection[i]->GetName() << "  "
         << right << setw(8) << fixed << (int)hist_collection[i]->Integral() << endl;
    total += hist_collection[i]->Integral();
  }
  cout<<"Sum = "<<total<<"\n"<<endl;
  */
  //Use LaTeX names:
  combine_hists(hist_collection, {"WWW", "WWZ", "WZZ", "ZZZ"},   "VVV", kGreen+3);
  combine_hists(hist_collection, {"WW", "WZ", "ZZ"},             "VV", kGreen+1);
  combine_hists(hist_collection, {"QCD (#mu)", "QCD (e#gamma)"}, "QCD", kYellow);
  combine_hists(hist_collection, {"t#bar{t}", "t#bar{t}V", "t#bar{t}W", "t#bar{t}Z"}, "t#bar{t}+x", kAzure+1);
  combine_hists(hist_collection, {"tX", "tW"},                   "Single t", kCyan-7);
  combine_hists(hist_collection, {"W+jets", "W#gamma"},          "W+jets/#gamma", kGray+2);
  /*
  cout<<"\nAfter combining:"<<endl;
  total = 0;
  for (int i = 0; i < (int)hist_collection.size(); i++){ 
    cout << left << setw(3) << i+1 << "  " 
         << left << setw(15) << hist_collection[i]->GetName() << "  "
         << right << setw(8) << fixed << (int)hist_collection[i]->Integral() << endl;
    total += hist_collection[i]->Integral();
  }
  cout<<"Sum = "<<total<<"\n"<<endl;
  */
  cout<<"Histogram collection read."<<endl;

  //______________________________________________________________
  
  //                           DATA
  //______________________________________________________________
  vector<TH1D*> data_collection; data_collection.clear();
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
  
  TH1D* hst_data   = nullptr;
  TH1D* hst_smuon  = nullptr;
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
  TH1D *allbkg = (TH1D *)bkg[0]->Clone(); allbkg->Reset();
  for(int i=0; i<(int)bkg.size(); i++) allbkg->Add(bkg[i]);
  allbkg->SetFillStyle(3013);
  allbkg->SetFillColor(kGray+3);
  allbkg->SetMarkerSize(0); //No marker
  allbkg->SetName("All bkg");

  THStack *bkgstack = new THStack("Stacked",_var+";"+_var+";Events");
  for(int i=0; i<(int)bkg.size(); i++) bkgstack->Add(bkg[i]);
  SetFillColorFromLineColor(bkgstack);
  bkgstack->SetTitle("");
  cout<<"Backgrounds ready!"<<endl;

  //______________________________________________________________
  
  //                    SIGNAL COLLECTION
  //______________________________________________________________

  TH1D *sig1, *sig2, *sig3;
  TString M1 = "600"; //Reference signal
  TString M2 = "200";
  TString M3 = "1000";
  TString masses[3] = {M1, M2, M3};
  TH1D* sig[3];

  Color_t colors[3] = {kRed+2, kRed+0, kRed+3};

  // default model & name patterns
  TString models[3] = {"VLLD-ele", "VLLD-ele", "VLLD-ele"};
  TString names[3]  = {"VLLDe_{" + M1 + "}", "VLLDe_{" + M2 + "}", "VLLDe_{" + M3 + "}"};

  // channel-specific overrides
  if(_channel=="em")        { models[1]="VLLD-mu"; names[1]="VLLD#mu_{" + M2 + "}"; }
  else if(_channel=="me")   { models[0]="VLLD-mu"; models[2]="VLLD-mu"; names[0]="VLLD#mu_{" + M1 + "}"; names[2]="VLLD#mu_{" + M3 + "}"; }
  else if(_channel=="mm")   { models[0]=models[1]=models[2]="VLLD-mu"; 
    names[0]="VLLD#mu_{" + M1 + "}"; names[1]="VLLD#mu_{" + M2 + "}"; names[2]="VLLD#mu_{" + M3 + "}"; }
  else if(_channel=="combined") { models[0]=models[2]="VLLD-mu"; models[1]="VLLD-ele"; 
    names[0]="VLLD#mu_{" + M1 + "}"; names[1]="VLLDe_{" + M2 + "}"; names[2]="VLLD#mu_{" + M3 + "}"; }

  // load histograms
  for(int i=0;i<3;i++){
    sig[i] = get_hist(_var, input_path, models[i], masses[i]);
    if(sig[i]) { SetHistoStyle(sig[i], colors[i]); sig[i]->SetName(names[i]); }
  }

  sig1 = sig[0];
  sig2 = sig[1];
  sig3 = sig[2];

  if(sig1) sig1->SetLineStyle(1); // reference is solid
  if(sig2) sig2->SetLineStyle(2); // others are dotted
  if(sig3) sig3->SetLineStyle(2);
  
  //sig2 = nullptr;
  //sig3 = nullptr;
  vector<TH1D*> sigvec = {sig1, sig2, sig3};
  cout<<"Signal ready!"<<endl;

  //______________________________________________________________
  
  //                     ON-SCREEN DISPLAYS
  //______________________________________________________________

  if(toOverlayData){
    //GetBinwiseSF(_var, "dilep_pt", hst_data, bkg, "DY");
    //GetBinwiseSF(_var, "HT", hst_data, bkg, "QCD");
    GetBinwiseSF(_var, "HT", hst_data, bkg, "t#bar{t}+x");
    //GetBinwiseSF(_var, "HT", hst_data, bkg, "W+jets/#gamma");
    
    //DisplayBinwiseSF(_var, "dilep_pt", hst_data, bkg, "DY");
    DisplayBinwiseSF(_var, "HT", hst_data, bkg, "t#bar{t}+x");
    //DisplayBinwiseSF(_var, "HT", hst_data, bkg, "W+jets/#gamma");
  }
  
  //______________________________________________________________
  
  //                     PLOTTING ON CANVAS
  //______________________________________________________________
  
  TCanvas *canvas = create_canvas(_var, filename, 600, 600);
  TPad *mainPad   = create_mainPad(0.0, 0.3, 1.0, 1.0);
  TPad *ratioPad  = create_ratioPad(0.0, 0.0, 1.0, 0.3);
  mainPad->Draw(); ratioPad->Draw();
  if (_var.Contains("iso")) mainPad->SetLogx();
  if (_var.Contains("iso")) ratioPad->SetLogx();
  //-------------------------------------------------------------
  //                     Drawing on mainpad
  //-------------------------------------------------------------

  mainPad->cd();
  
  // 1) Setting up a dummy hist to enforce display style in the plot. It should always be drawn first.
  TH1D *dummy = (TH1D *)bkg[0]->Clone(); dummy->Reset();
  dummy->SetTitle("");
  dummy->GetYaxis()->SetTitle("Events");
  dummy->GetYaxis()->SetTitleSize(0.07);
  dummy->GetYaxis()->SetTitleOffset(1.00);
  dummy->GetYaxis()->SetLabelSize(0.05);
  dummy->GetYaxis()->SetTickSize(0.01);
  dummy->GetYaxis()->SetRangeUser(ymin, ymax);
  dummy->GetXaxis()->SetTickSize(0.01);
  dummy->SetStats(0);
  dummy->Draw("hist");

  // 2) Drawing the rest:
  bkgstack->Draw("hist same"); //Stacked (total) background
  allbkg->Draw("E2 same");     //Uncertainty on the total background
  TH1D *allbkg2 = (TH1D*) allbkg->Clone();
  allbkg2->SetFillColor(0);
  allbkg2->SetLineColor(kGray+3);
  allbkg2->Draw("hist same");  //Black hist over the stack (to outline the total background)
  if(sig1) sig1->Draw("HIST same");
  if(sig2) sig2->Draw("HIST same");
  if(sig3) sig3->Draw("HIST same");
  if(toOverlayData) hst_data->Draw("ep same");
  dummy->Draw("sameaxis"); //Drawing axis ticks on top

  // 3) Done. Update the canvas.
  canvas->Update();

  //-------------------------------------------------------------
  //                     Drawing on ratiopad
  //-------------------------------------------------------------
  
  ratioPad->cd();

  // 1) Calculation of S/sqrtB and plotting:
  globalSbyB = 0;
  TH1D *sbyrb = nullptr;
  vector<float> isobinslog = {0.002, 0.004, 0.008, 0.016, 0.032, 0.064, 0.128, 0.256};
  
  if     (sig1) sbyrb = GetSbyRootB(sig1, bkg);
  else if(sig2) sbyrb = GetSbyRootB(sig2, bkg);
  else if(sig3) sbyrb = GetSbyRootB(sig3, bkg);
  if(sbyrb){
    refine_hist(sbyrb);
    sbyrb->SetName("SbyRootB");
    SetRatioStyle(sbyrb, _name);
    sbyrb->GetYaxis()->SetTitle("S/\\sqrt{B}");
    sbyrb->GetYaxis()->SetTitleSize(0.15);
    sbyrb->GetYaxis()->SetTitleOffset(0.43);
    sbyrb->GetYaxis()->SetLabelSize(0.13);
    if(_var.Contains("iso")) showMoreLabels(sbyrb);
    if(!toOverlayData) sbyrb->Draw("ep");
    canvas->Update();
    //cout<<"S/sqrtB bin contents:"<<endl;
    //DisplayYieldsInBins(sbyrb);
  }
  else DisplayText("Warning: SbyRootB is null!");

  // 2) Calculation of Obs/Exp and plotting (in case data is overlaid):
  TH1D *ratiohist = nullptr;
  globalObsbyExp =0;
  globalObsbyExpErr =0;
    
  if(toOverlayData){
    ratiohist = GetRatio(hst_data, bkg);
    
    //Setting up a horizontal line on the ratiopad:
    float xlow  = ratiohist->GetXaxis()->GetBinLowEdge(1);
    float xhigh = ratiohist->GetXaxis()->GetBinUpEdge(ratiohist->GetNbinsX());
    TLine *line = new TLine(xlow, 1, xhigh, 1);
    line->SetLineColor(kRed);
    line->SetLineWidth(2);
    
    //Calculating the uncertainty on the background in each bin:
    TGraphErrors *err = GetUncertainty(allbkg);
    err->GetYaxis()->SetNdivisions(5, kTRUE);
    err->SetStats(0);

    //-----------------------------
    // Special corrections in bins:
    //-----------------------------

    /*
    if(_var == "LT"){
      TH1D* data_minus_bkg = (TH1D *)hst_data->Clone();
      for(int i=0; i<(int)bkg.size(); i++){
	if(TString (bkg[i]->GetName()) != "t#bar{t}"){
	  data_minus_bkg->Add(bkg[i], -1);
	}
      }
      ratiohist->Reset();
      for(int i=0; i<(int)bkg.size(); i++){
	if(TString (bkg[i]->GetName()) == "t#bar{t}"){
	  data_minus_bkg->Divide(bkg[i]);
	  ratiohist = data_minus_bkg;
	}
      }
    }//Special
    */
    
    //Drawing everything in the proper order:
    refine_hist(ratiohist);
    SetRatioStyle(ratiohist, _name);
    ratiohist->GetYaxis()->SetTitle("obs/exp");
    //if(_var == "LT") ratiohist->GetYaxis()->SetTitle("SF");
    ratiohist->GetYaxis()->SetTitleSize(0.15);
    ratiohist->GetYaxis()->SetTitleOffset(0.43);
    ratiohist->GetYaxis()->SetLabelSize(0.13);
    ratiohist->GetYaxis()->SetRangeUser(0, 2.2);
    //if(toZoom) ratiohist->GetXaxis()->SetRangeUser(xmin, xmax);
    if(_var.Contains("iso")) showMoreLabels(ratiohist);
    
    ratiohist->Draw("ep"); //Inheriting the settings from the ratio hist.
    err->Draw("SAME P E2");
    line->Draw("same");
    ratiohist->Draw("ep same"); //I want the ratio to be on top.
    canvas->Update();
  }

  // 3) Drawing veto region for Drell-Yan:
  if(_channel=="ee" && _var=="dilep_mass") draw_veto_region(ratioPad, 76, 106);

  //-------------------------------------------------------------
  //                    Texts and legend
  //-------------------------------------------------------------

  mainPad->cd();
  
  float xright = 0.95;  // right edge
  float yup = 0.94;
  put_text("CMS", 0.17, 0.83, 62, 0.07);          // Larger, bold CMS label
  put_text("Preliminary", 0.27, 0.83, 52, 0.05);  // Smaller preliminary label
  if(_campaign == "2016preVFP_UL")    put_latex_text("19.7 fb^{-1} (2016-preVFP)",   xright, yup, 42, 0.05, true);
  if(_campaign == "2016postVFP_UL")   put_latex_text("16.2 fb^{-1} (2016-postVFP)",  xright, yup, 42, 0.05, true);
  if(_campaign == "2017_UL")          put_latex_text("41.5 fb^{-1} (2017)",          xright, yup, 42, 0.05, true);
  if(_campaign == "2018_UL")          put_latex_text("59.8 fb^{-1} (2018)",          xright, yup, 42, 0.05, true);
  if(_campaign == "Run2")             put_latex_text("137.2 fb^{-1} (Run-2)",        xright, yup, 42, 0.05, true);
  if(_campaign == "Run3Summer22")     put_latex_text("7.98 fb^{-1} (2022-preEE)",    xright, yup, 42, 0.05, true);
  if(_campaign == "Run3Summer22EE")   put_latex_text("26.7 fb^{-1} (2022-postEE)",   xright, yup, 42, 0.05, true);
  if(_campaign == "Run3Summer23")     put_latex_text("17.8 fb^{-1} (2023-preBPix)",  xright, yup, 42, 0.05, true);
  if(_campaign == "Run3Summer23BPix") put_latex_text("9.45 fb^{-1} (2023-postBPix)", xright, yup, 42, 0.05, true);
  if(_campaign == "Run3")             put_latex_text("61.9 fb^{-1} (2022+2023)",     xright, yup, 42, 0.05, true);
  if(_campaign == "FullDataset")      put_latex_text("199.1 fb^{-1} (Run-2+2022+2023)", xright, yup, 42, 0.05, true);
  put_latex_text(info1, 0.17, 0.78, 42, 0.04);     //Additional information
  put_latex_text(info2, 0.17, 0.73, 42, 0.04);     //Additional information

  TLegend *lg = create_legend(0.48, 0.52, 0.94, 0.89);
  //TLegend *lg = create_legend(0.52, 0.52, 0.96, 0.89);
  lg->SetNColumns(2);
  
  if(toOverlayData){
    TString data_yield = Form("%d", (int)hst_data->Integral());
    TString text = "Observed ["+data_yield+"]";
    lg->AddEntry(hst_data, text, "ep");
    if(hst_smuon)  SetLegendEntry(lg, hst_smuon, "ep");
    if(hst_egamma) SetLegendEntry(lg, hst_egamma, "ep");
  }
  for(int i=(int)bkg.size()-1; i>=0; i--){
    bkg[i]->SetLineColor(kBlack);
    bkg[i]->SetLineWidth(1);
    SetLegendEntry(lg, bkg[i]);
  }
  if(sig1) SetLegendEntry(lg, sig1, "l");
  if(sig2) SetLegendEntry(lg, sig2, "l");
  if(sig3) SetLegendEntry(lg, sig3, "l");
  TString legendheader = Form("Global S/#sqrt{B} = %.3f #pm %.3f", globalSbyB, globalSbyBErr);
  if(toOverlayData){
    TString val = Form("Global obs/exp = %.3f", globalObsbyExp);
    TString err = Form("%.3f", globalObsbyExpErr);
    legendheader = val + " #pm " + err;
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Obs/Exp = " << globalObsbyExp << " ± " << globalObsbyExpErr << std::endl;
    std::cout << "S/sqrt(B) = " << globalSbyB << " ± " << globalSbyBErr << std::endl;
    std::cout.unsetf(std::ios::fixed); // Reset to defaultfloat
    std::cout << std::endl;
  }
  else{
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "S/sqrt(B) = " << globalSbyB << " ± " << globalSbyBErr << std::endl;
    std::cout.unsetf(std::ios::fixed); // Reset to defaultfloat
    std::cout << std::endl;
  }
					 
  lg->SetHeader(legendheader);
  lg->Draw();
  canvas->Update();

  //-------------------------------------------------------------
  //                    Output management
  //-------------------------------------------------------------
  if(toSave){
    createFolder(dump_folder);
    canvas->SaveAs(filename+".png");
    cout<<"File created: \033[33;1m"<<filename<<".png\033[0m"<<endl;
  }
  
  cout<<"Done!"<<endl;
}

