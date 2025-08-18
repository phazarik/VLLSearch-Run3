#include "include_headers.h"
#include "utilities/decorations.h"
#include "utilities/functions.h"

TH2D *gethist2D(const TString& var, const TString& input_path, const TString& sample, const TString& subsample, int rebin=1.0);
TH2D* combineHists2D(const vector<TH2D*>& hists, const TString& name="combined" , Color_t col=kWhite);
//Global parameters (not used):
float globalSbyB, globalSbyBErr, globalObsbyExp, globalObsbyExpErr;
void normalizeHist2D(TH2D* h) {if (h && h->Integral() > 0) h->Scale(1.0 / h->Integral());}

void make2Dcontour(
		   TString _jobname="2025-08-14_baseline/hist_baseline_2018_UL_mm",
		   TString _campaign = "2018_UL",
		   TString _channel = "mm",
		   TString _tag = "baseline",
		   TString _displaytext = "baseline",
		   bool _save = true
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
  bool toSave=_save;
  Double_t ymin = 0.1; Double_t ymax = 10E8;
  TString info1 = _displaytext; //event-selection
  TString info2 = channelname + "-channel";
  TString hstname = "nnscore_Run3_vlld_1v3";
  TString xtitle  = "DNN: QCD-vs-VLLD";
  TString ytitle  = "DNN: W+jets/#gamma-vs-VLLD"; 
  //--------------------------------------------------------------------------
  TString dump_folder = "2Dplots/"+date_stamp;  
  TString filename = dump_folder+"/2D_"+hstname+"_"+_campaign+"_"+_channel;
  TString input_path = "../ROOT_FILES/hists/"+_jobname;

  //-------------------------
  // Preparing TH2D objects
  //-------------------------
  
  // 1. QCD
  vector<TH2D *> hst2D_qcd = {
    gethist2D(hstname, input_path, "QCDMu", "20to30"),
    gethist2D(hstname, input_path, "QCDMu", "30to50"),
    gethist2D(hstname, input_path, "QCDMu", "50to80"),
    gethist2D(hstname, input_path, "QCDMu", "80to120"),
    gethist2D(hstname, input_path, "QCDMu", "120to170"),
    gethist2D(hstname, input_path, "QCDMu", "170to300"),
    gethist2D(hstname, input_path, "QCDMu", "300to470"),
    gethist2D(hstname, input_path, "QCDMu", "470to600"),
    gethist2D(hstname, input_path, "QCDMu", "600to800"),
    gethist2D(hstname, input_path, "QCDMu", "800to1000"),
    gethist2D(hstname, input_path, "QCDMu", "1000"),
  };
  TH2D* hst2D_qcd_comb = combineHists2D(hst2D_qcd, "QCD", kRed);

  //2. Wjets:
  vector<TH2D *> hst2D_wjets = {
    gethist2D(hstname, input_path, "WGtoLNuG", "Inclusive"),
  };
  TH2D* hst2D_wjets_comb = combineHists2D(hst2D_wjets, "W+jets/#gamma", kGray+2);

  //3.Signal:
  vector<TH2D *> hst2D_sig1 = {
    gethist2D(hstname, input_path, "VLLD-mu", "400"),
  };
  TH2D* hst2D_sig1_comb = combineHists2D(hst2D_sig1, "VLLD-mu (400 GeV)", kGreen+1);

  // Normalize histograms to unit integral for comparable scales
  normalizeHist2D(hst2D_qcd_comb);
  normalizeHist2D(hst2D_wjets_comb);
  normalizeHist2D(hst2D_sig1_comb);

  //--------------------
  // DRAWING ON CANVAS
  //--------------------
  
  TCanvas *canvas = create_canvas(hstname, filename, 600, 600);
  canvas->cd();
  canvas->SetTopMargin(0.09);
  canvas->SetRightMargin(0.05);  // Standard CMS right margin
  canvas->SetLeftMargin(0.15); // Standard CMS right margin
  canvas->SetBottomMargin(0.15);
  //canvas->SetLogz();

  hst2D_qcd_comb->Rebin2D(5,5);  // combine rebinxrebin bins
  hst2D_wjets_comb->Rebin2D(5,5);
  hst2D_sig1_comb->Rebin2D(5,5);

  const int nlevels = 7;
  double levels[nlevels] = {0.002, 0.02, 0.05, 0.1, 0.2, 0.5, 1.0};
  hst2D_qcd_comb->SetContour(nlevels, levels);
  hst2D_wjets_comb->SetContour(nlevels, levels);
  hst2D_sig1_comb->SetContour(nlevels, levels);

  //First one
  hst2D_qcd_comb->SetLineColor(kRed);
  hst2D_qcd_comb->Draw("CONT3");
  hst2D_qcd_comb->SetTitle("");
  hst2D_qcd_comb->GetXaxis()->SetTitle(xtitle);
  hst2D_qcd_comb->GetXaxis()->SetTitleSize(0.05);
  hst2D_qcd_comb->GetYaxis()->SetTitle(ytitle);
  hst2D_qcd_comb->GetYaxis()->SetTitleSize(0.05);
  hst2D_qcd_comb->GetZaxis()->SetRangeUser(0.0001, 1.0);
  //Rest of them
  hst2D_wjets_comb->Draw("CONT3 SAME");
  hst2D_sig1_comb->Draw("CONT3 SAME");
  canvas->Update();

  //Draw legend
  TLegend *lg = create_legend(0.20, 0.55, 0.45, 0.75);
  lg->SetFillColor(kWhite); lg->SetFillStyle(1001);
  lg->AddEntry(hst2D_qcd_comb, hst2D_qcd_comb->GetName(), "l");
  lg->AddEntry(hst2D_wjets_comb, hst2D_wjets_comb->GetName(), "l");
  lg->AddEntry(hst2D_sig1_comb, hst2D_sig1_comb->GetName(), "l");
  lg->Draw();
  
  float xright = 0.95;  // right edge
  float yup = 0.93;
  put_text("CMS", 0.15, yup, 62, 0.07);          // Larger, bold CMS label
  put_text("Preliminary", 0.30, yup, 52, 0.05);  // Smaller preliminary label
  if(_campaign == "2016preVFP_UL")    put_latex_text("19.7 fb^{-1} (2016-preVFP)",   xright, yup, 42, 0.04, true);
  if(_campaign == "2016postVFP_UL")   put_latex_text("16.2 fb^{-1} (2016-postVFP)",  xright, yup, 42, 0.04, true);
  if(_campaign == "2017_UL")          put_latex_text("41.5 fb^{-1} (2017)",          xright, yup, 42, 0.04, true);
  if(_campaign == "2018_UL")          put_latex_text("59.8 fb^{-1} (2018)",          xright, yup, 42, 0.04, true);
  if(_campaign == "Run2")             put_latex_text("137.2 fb^{-1} (Run-2)",        xright, yup, 42, 0.04, true);
  if(_campaign == "Run3Summer22")     put_latex_text("7.98 fb^{-1} (2022-preEE)",    xright, yup, 42, 0.04, true);
  if(_campaign == "Run3Summer22EE")   put_latex_text("26.7 fb^{-1} (2022-postEE)",   xright, yup, 42, 0.04, true);
  if(_campaign == "Run3Summer23")     put_latex_text("17.8 fb^{-1} (2023-preBPix)",  xright, yup, 42, 0.04, true);
  if(_campaign == "Run3Summer23BPix") put_latex_text("9.45 fb^{-1} (2023-postBPix)", xright, yup, 42, 0.04, true);
  if(_campaign == "Run3")             put_latex_text("61.9 fb^{-1} (2022+2023)",     xright, yup, 42, 0.04, true);
  put_latex_text(info1, 0.18, 0.86, 42, 0.04);     //Additional information
  put_latex_text(info2, 0.18, 0.82, 42, 0.04);     //Additional information

  canvas->Update();

  if(toSave){
    createFolder(dump_folder);
    canvas->SaveAs(filename+".png");
    cout<<"File created: \033[33;1m"<<filename<<".png\033[0m"<<endl;
  }
  
}

TH2D *gethist2D(
		const TString& var,
		const TString& input_path,
		const TString& sample,
		const TString& subsample,
		int rebin=1
		){
  TString filename = input_path+"/"+"hst_"+sample+"_"+subsample+".root";
  if(!file_exists(filename)){
    DisplayText("Not found: "+filename, 31);
    return nullptr;
  }
  TFile *file = TFile::Open(filename, "READ");
  TH2D *hst = (TH2D *)file->Get(var);
  if(!hst){
    DisplayText("Hist not found for "+filename, 31);
    file->Close();
    delete file;
    return nullptr;
  }
  hst->Rebin2D(rebin, rebin);
  hst->SetDirectory(0);
  file->Close();
  delete file;
  return hst;
}

TH2D* combineHists2D(const vector<TH2D*>& hists, const TString& name="combined", Color_t col) {
  TH2D* combined = nullptr;
  for (auto hst : hists) {
    if (!hst) continue;
    if (!combined) {
      combined = (TH2D*)hst->Clone(name);
      combined->SetDirectory(0);
    }
    else combined->Add(hst);  
  }
  if (!combined) {
    DisplayText("No valid histograms to combine for " + name, 31);
    return nullptr;
  }
  // Apply decorations
  combined->SetStats(0);
  combined->SetLineColor(col);
  combined->SetLineWidth(2);
  return combined;
}
