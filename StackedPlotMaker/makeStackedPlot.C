#include "include_headers.h"
#include "utilities/decorations.h"
#include "utilities/functions.h"
#include "utilities/read_hists.h"

//Global parameters:
float globalSbyB, globalSbyBErr, globalObsbyExp, globalObsbyExpErr;

void makeStackedPlot(
		     TString _var = "HT",
		     TString _name = "HT (GeV)",
		     TString _jobname = "2025-05-15_sr/hist_Run3Summer22EE_sr_mm",
		     TString _campaign = "Run3Summer22EE",
		     TString _channel = "mm",
		     TString _tag = "sr",
		     TString _displaytext = "Signal region"
		     )
{
  TString date_stamp  = todays_date();
  TString channelname = "";
  if (_channel == "mm") channelname = "#mu#mu";
  if (_channel == "me") channelname = "#mue";
  if (_channel == "em") channelname = "e#mu";
  if (_channel == "ee") channelname = "ee";

  //--------------------------------------------------------------------------
  // SET GLOBAL SETTINGS 
  bool toOverlayData=false;
  bool toSave=false;
  Double_t ymin = 0.1; Double_t ymax = 10E6;
  TString output_tag = _tag;
  TString info1 = _displaytext; //event-selection
  TString info2 = channelname + "-channel";
  //TString info2 = "corrected";
  
  //--------------------------------------------------------------------------
  TString dump_folder = "plots/"+date_stamp+"/"+output_tag+"_"+_campaign+"_"+_channel;  
  TString filename = dump_folder+"/"+_var;
  TString input_path = "../ROOT_FILES/hists/"+_jobname;;
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
  combine_hists(hist_collection, {"t#bar{t}W", "t#bar{t}Z"},     "t#bar{t}V", kAzure+2);
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
  if(_channel == "ee"){
    sig1 = get_hist(_var, input_path, "VLLD_ele", "M400");
    if(sig1) {SetHistoStyle(sig1, kRed+0); sig1->SetName("VLLDe_{400}");}
    sig2 = get_hist(_var, input_path, "VLLD_ele", "M200");
    if(sig2) {SetHistoStyle(sig2, kRed+2); sig2->SetName("VLLDe_{200}");}
  }
  else if (_channel == "em"){
    sig1 = get_hist(_var, input_path, "VLLD_ele", "M400");
    if(sig1) {SetHistoStyle(sig1, kRed+0); sig1->SetName("VLLDe_{400}");}
    sig2 = get_hist(_var, input_path, "VLLD_mu", "M400");
    if(sig2) {SetHistoStyle(sig2, kRed+2); sig2->SetName("VLLD#mu_{400}");}
  }
  else if (_channel == "me"){
    sig1 = get_hist(_var, input_path, "VLLD_mu", "M400");
    if(sig1) {SetHistoStyle(sig1, kRed+0); sig1->SetName("VLLD#mu_{400}");}
    sig2 = get_hist(_var, input_path, "VLLD_ele", "M400");
    if(sig2) {SetHistoStyle(sig2, kRed+2); sig2->SetName("VLLDe_{400}");}
  }
  else if (_channel == "mm"){
    sig1 = get_hist(_var, input_path, "VLLD_mu", "M400");
    if(sig1) {SetHistoStyle(sig1, kRed+0); sig1->SetName("VLLD#mu_{400}");}
    sig2 = get_hist(_var, input_path, "VLLD_mu", "M200");
    if(sig2) {SetHistoStyle(sig2, kRed+2); sig2->SetName("VLLD#mu_{200}");}
  }
  sig3 = nullptr;
  vector<TH1D*> sigvec = {sig1, sig2, sig3};
  cout<<"Signal ready!"<<endl;

  //______________________________________________________________
  
  //                     ON-SCREEN DISPLAYS
  //______________________________________________________________

  GetBinwiseSF(_var, "HT", hst_data, bkg, "t#bar{t}");
  //GetBinwiseSF(_var, "HT", hst_data, bkg, "QCD");
  //GetBinwiseSF(_var, "dilep_pt", hst_data, bkg, "DY");
  //GetBinwiseSF(_var, "HT", hst_data, bkg, "W+jets");
  //DisplayBinwiseSF(_var, "HT", hst_data, bkg, "t#bar{t}");
  //DisplayBinwiseSF(_var, "HT", hst_data, bkg, "W+#gamma");
  
  //______________________________________________________________
  
  //                     PLOTTING ON CANVAS
  //______________________________________________________________
  
  TCanvas *canvas = create_canvas(_var, filename, 600, 600);
  TPad *mainPad   = create_mainPad(0.0, 0.3, 1.0, 1.0);
  TPad *ratioPad  = create_ratioPad(0.0, 0.0, 1.0, 0.3);
  mainPad->Draw(); ratioPad->Draw();

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

    ratiohist->Draw("ep"); //Inheriting the settings from the ratio hist.
    err->Draw("SAME P E2");
    line->Draw("same");
    ratiohist->Draw("ep same"); //I want the ratio to be on top.
    canvas->Update();
  }

  // 3) Drawing veto region for Drell-Yan:
  //if(_channel=="ee" && _var=="dilep_mass") draw_veto_region(ratioPad, 76, 106);

  //-------------------------------------------------------------
  //                    Texts and legend
  //-------------------------------------------------------------

  mainPad->cd();

  put_text("CMS", 0.17, 0.83, 62, 0.07);          // Larger, bold CMS label
  put_text("Preliminary", 0.27, 0.83, 52, 0.05);  // Smaller preliminary label
  if(_campaign == "2016preVFP_UL")  put_latex_text("19.7 fb^{-1} (2016-preVFP)", 0.62, 0.94, 42, 0.05);
  if(_campaign == "2016postVFP_UL") put_latex_text("16.2 fb^{-1} (2016-postVFP)", 0.60, 0.94, 42, 0.05);
  if(_campaign == "2017_UL")        put_latex_text("41.5 fb^{-1} (2017)", 0.74, 0.94, 42, 0.05);
  if(_campaign == "2018_UL")        put_latex_text("59.8 fb^{-1} (2018)", 0.74, 0.94, 42, 0.05);
  if(_campaign == "Run3Summer22")   put_latex_text("7.98 fb^{-1} (2022)", 0.74, 0.94, 42, 0.05);
  if(_campaign == "Run3Summer22EE") put_latex_text("26.67 fb^{-1} (2022-postEE)", 0.61, 0.94, 42, 0.05);
  put_latex_text(info1, 0.17, 0.78, 42, 0.04);     //Additional information
  put_latex_text(info2, 0.17, 0.73, 42, 0.04);     //Additional information

  TLegend *lg = create_legend(0.48, 0.52, 0.94, 0.89);
  //TLegend *lg = create_legend(0.52, 0.52, 0.96, 0.89);
  lg->SetNColumns(2);
  
  if(toOverlayData){
    TString data_yield = Form("%d", (int)hst_data->Integral());
    TString text = "Observed ["+data_yield+"]";
    lg->AddEntry(hst_data, text, "f");
    if(hst_smuon)  SetLegendEntry(lg, hst_smuon);
    if(hst_egamma) SetLegendEntry(lg, hst_egamma);
  }
  for(int i=(int)bkg.size()-1; i>=0; i--){
    bkg[i]->SetLineColor(kBlack);
    bkg[i]->SetLineWidth(1);
    SetLegendEntry(lg, bkg[i]);
  }
  if(sig1) SetLegendEntry(lg, sig1);
  if(sig2) SetLegendEntry(lg, sig2);
  if(sig3) SetLegendEntry(lg, sig3);
  TString legendheader = Form("Global S/#sqrt{B} = %.3f ± %.3f", globalSbyB, globalSbyBErr);
  if(toOverlayData){
    TString val = Form("Global obs/exp = %.3f", globalObsbyExp);
    TString err = Form("%.3f", globalObsbyExpErr);
    legendheader = val+" #pm "+err;
    cout << fixed << setprecision(3);
    cout << "Obs/Exp = " << globalObsbyExp << " ± " << globalObsbyExpErr << endl;
    cout << "S/sqrt(B) = " << globalSbyB << " ± " << globalSbyBErr << "\n";
    cout << defaultfloat << endl;
  }
  else cout<<legendheader<<"\n"<<endl;
					 
  lg->SetHeader(legendheader);
  lg->Draw();
  canvas->Update();

  //-------------------------------------------------------------
  //                    Output management
  //-------------------------------------------------------------
  if(toSave){
    createFolder(dump_folder);
    canvas->SaveAs(filename+".png");
    cout<<"File created: "<<filename<<".png"<<endl;
  }
  
  cout<<"Done!"<<endl;
}

