//------------------------------------------------------------------------------------------------------------
// fit.C
// Purpose: This script fits trigger efficiency data from a JSON file using an error function model and 
//          generates plots with nominal fits and systematic variations. It processes data for different 
//          triggers and detector regions (barrel and endcap)
//
// Requirements:
//  - nlohmann/json library for JSON parsing
//  - Input JSON file containing trigger efficiency data (e.g., efficiencies/<campaign>.json) with structure:
//    {
//      "trigger_name": {
//        "region": {
//          "points": [{"ptrange": [ptlow, pthigh], "eff": efficiency, "err": error}, ...],
//          "offline_cut": threshold_value
//        }
//      }
//    }
//
// Notes:
//
//  - The fit model is eff = f(x) = 0.5 * A * (1 + Erf((x - mu) / (2 * sigma)))
//    where:
//      - A     = par[0] → plateau efficiency (maximum value the curve reaches)
//      - mu    = par[1] → threshold (value of x where the turn-on is centered)
//      - sigma = par[2] → width of the turn-on (related to how sharp the rise is)
//    This function models the S-shaped turn-on behavior seen in trigger efficiency curves
//
//  - Systematic variations are computed using 1-sigma parameter shifts from the covariance matrix.
//  - Nominal fit is drawn as a solid line, while SystUp and SystDn are drawn as dotted lines.
//------------------------------------------------------------------------------------------------------------

#include <TFile.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TLegend.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <TLine.h>
#include <TMatrixDSym.h>

using namespace std;
using json = nlohmann::json;

// ANSI colors for terminal output
TString RESET = "\033[0m";
TString RED = "\033[0;31m";
TString YELLOW = "\033[0;33m";
TString YELLOW_BOLD = "\033[1;33m";
TString BLUE = "\033[0;34m";
TString BLUE_BOLD = "\033[1;34m";

//Fit function:
double model(double* x, double* par) {
  return 0.5 * par[0] * (1.0 + TMath::Erf((x[0] - par[1]) / (2.0 * par[2])));
}

// Utility function declarations
void printDataPoints(const TString& trigger, const TString& region, const vector<double>& x, const vector<double>& y, const vector<double>& ey);
void printFitFunction(const TString& label, double p0, double p1, double p2);
void writeFitParameters(ofstream& out, const TString& campaign, const TString& trigger, const TString& region, TF1* func, TFitResultPtr fitResult, double* up, double* down);
TCanvas* createCanvas(TString name);
TLegend* createLegend();
void addCMSLabel(TCanvas* canvas, TString campaign);

//----------------
// Main function
//----------------
void fit(const TString campaign = "2017_UL", bool test = true, bool debug = true) {

  double fitRangeMin = 5.0; // Minimum for log x-axis
  double fitRangeMax = 400.0; // Maximum fit range

  // Reading JSON file
  TString jsonFile = TString::Format("efficiencies/%s.json", campaign.Data());
  ifstream f(jsonFile.Data());
  if (!f.is_open()) {
    cerr << RED << "ERROR: Could not open JSON file: " << jsonFile << RESET << endl;
    return;
  }
  json j;
  try {f >> j;}
  catch (const json::parse_error& e) {
    cerr << RED << "ERROR: JSON parse error: " << e.what() << RESET << endl;
    return;
  }
  if (debug) {
    cout << YELLOW << "JSON content for " << jsonFile << ":" << RESET << endl;
    cout << j.dump(2) << endl;
  }
  // Define triggers for different campaigns
  vector<pair<TString, TString>> triggers;
  if     (campaign=="2016preVFP_UL")    {triggers= {{"HLT_IsoMu24", "muon"}, {"HLT_Ele27_WPTight_Gsf", "electron"}};}
  else if(campaign=="2016postVFP_UL")   {triggers= {{"HLT_IsoMu24", "muon"}, {"HLT_Ele27_WPTight_Gsf", "electron"}};}
  else if(campaign=="2017_UL")          {triggers= {{"HLT_IsoMu27", "muon"}, {"HLT_Ele35_WPTight_Gsf", "electron"}};}
  else if(campaign=="2018_UL")          {triggers= {{"HLT_IsoMu24", "muon"}, {"HLT_Ele32_WPTight_Gsf", "electron"}};}
  else if(campaign=="Run3Summer22")     {triggers= {{"HLT_IsoMu24", "muon"}, {"HLT_Ele32_WPTight_Gsf", "electron"}};}
  else if(campaign=="Run3Summer22EE")   {triggers= {{"HLT_IsoMu24", "muon"}, {"HLT_Ele32_WPTight_Gsf", "electron"}};}
  else if(campaign=="Run3Summer23")     {triggers= {{"HLT_IsoMu24", "muon"}, {"HLT_Ele32_WPTight_Gsf", "electron"}};}
  else if(campaign=="Run3Summer23BPix") {triggers= {{"HLT_IsoMu24", "muon"}, {"HLT_Ele32_WPTight_Gsf", "electron"}};}
  else{
    cout<<RED<<"[ERROR] Pick the right campaign"<<RESET<<endl;
    return;
  }

  // Prepare output file
  system("mkdir -p functions");
  string outfile = Form("functions/%s_trigeff.txt", campaign.Data());
  ofstream out(outfile);
  out << fixed << setprecision(6);

  // Looping over triggers
  for (const auto& [trig, label] : triggers) {
    vector<TString> regions = {"barrel", "endcap"};
    map<TString, TGraphErrors*> graphs;
    map<TString, TF1*> fits;
    map<TString, TF1*> syst_up_fits;
    map<TString, TF1*> syst_down_fits;
    map<TString, TLine*> cut_lines;
    map<TString, TLatex*> threshold_labels;
    map<TString, TGraph*> syst_bands;

    out << endl;
    out << string(80, '=') << endl;
    out << "Trigger: " << trig << endl;
    out << string(80, '=') << endl;

    // Looping over barrel/endcap regions
    for (const auto& reg : regions) {
      cout << YELLOW_BOLD << "\nProcessing " << trig << " (" << reg << ")" << RESET << endl;
      vector<double> x, y, ex, ey;
      double offlineCut = 26.0; //replaced by value from JSON

      // Checking JSON structure
      if (!j.contains(trig.Data()) || !j[trig.Data()].contains(reg.Data())) {
        cerr << RED << "ERROR: Missing trigger " << trig << " or region " << reg << " in JSON" << RESET << endl;
        continue;
      }
      json regionData = j[trig.Data()][reg.Data()];
      if (!regionData.is_object() || !regionData.contains("points") || !regionData["points"].is_array()) {
        cerr << RED << "ERROR: Invalid data structure for " << trig << " " << reg << RESET << endl;
        continue;
      }

      //-------------------------------------------------------------------------------------------------
      // Extracting data points
      for (const auto& entry : regionData["points"]) {
        if (!entry.contains("ptrange") || !entry["ptrange"].is_array() || entry["ptrange"].size() != 2) {
          cerr << RED << "ERROR: Invalid ptrange for " << trig << " " << reg << RESET << endl;
          continue;
        }
        double ptlow = entry["ptrange"][0].get<double>();
        double pthigh = entry["ptrange"][1].get<double>();
        x.push_back(0.5 * (ptlow + pthigh));
        ex.push_back(0.0);
        y.push_back(entry["eff"].get<double>());
        ey.push_back(entry["err"].get<double>());
      }
      offlineCut = regionData.contains("offline_cut") ? regionData["offline_cut"].get<double>() : 26.0;
      //-------------------------------------------------------------------------------------------------
      
      if (debug) printDataPoints(trig, reg, x, y, ey);
      cout << "Data points loaded: " << x.size() << endl;

      //--------
      //POINTS:
      //--------
      TGraphErrors* gr = new TGraphErrors(x.size(), x.data(), y.data(), ex.data(), ey.data());
      gr->SetTitle(trig);
      gr->SetMarkerStyle(20);
      gr->SetMarkerColor(reg == "barrel" ? kBlue+1 : kRed+1);
      gr->SetLineColor(reg == "barrel" ? kBlue+1 : kRed+1);
      gr->SetLineWidth(0); // No connecting lines
      gr->SetMarkerSize(1.0);
      gr->SetLineWidth(2);
      gr->SetMarkerStyle(reg == "barrel" ? kFullSquare : kCircle);
      graphs[reg] = gr;

      //-------
      // FITS:
      //-------
      //par[0] = 0.97: starting guess for plateau efficiency.
      //par[1] = offlineCut: center of the turn-on (threshold).
      //par[2] = 0.5: width of the turn-on.
      TVirtualFitter::SetDefaultFitter("Minuit2");
      TF1* func = new TF1(TString::Format("fit_%s", reg.Data()), model, fitRangeMin, fitRangeMax, 3);
      func->SetParNames("plateau", "threshold", "width");
      func->SetParameters(0.97, offlineCut, 0.5);
      func->SetLineColor(reg == "barrel" ? kBlue : kRed);
      func->SetLineStyle(1); func->SetLineWidth(2);
      TFitResultPtr fitResult = gr->Fit(func, "S M"); // enable Minos errors
      fits[reg] = func;

      //Extracting systematic variations by shifting each fit parameter up and down by its 1-sigma uncertainty.
      //The covariance matrix provides the variances (diagonal elements), i.e., squared uncertainties.
      //These variations can be used to compute "up" and "down" fits representing ±1-sigma systematic shifts.
      double params[3] = {func->GetParameter(0), func->GetParameter(1), func->GetParameter(2)};
      TMatrixDSym cov = fitResult->GetCovarianceMatrix();
      double width_uncertainty = sqrt(cov(2,2));
      double up[3] = {
	params[0] + sqrt(cov(0,0)), // Plateau up
	params[1] + sqrt(cov(1,1)), // Threshold up
	std::max(0.01, params[2] - width_uncertainty) // Ensure width is positive
      };
      double down[3] = {
	params[0] - sqrt(cov(0,0)), // Plateau down
	params[1] - sqrt(cov(1,1)), // Threshold down
	params[2] + width_uncertainty // Larger width for SystDn
      };

      // Creating additional systematic up/down functions
      TF1* upFunc = new TF1(TString::Format("fit_%s_up", reg.Data()), model, fitRangeMin, fitRangeMax, 3);
      upFunc->SetParameters(up[0], up[1], up[2]);
      upFunc->SetLineColor(reg == "barrel" ? kBlue : kRed);
      upFunc->SetLineStyle(1); upFunc->SetLineWidth(1);
      syst_up_fits[reg] = upFunc;

      TF1* downFunc = new TF1(TString::Format("fit_%s_down", reg.Data()), model, fitRangeMin, fitRangeMax, 3);
      downFunc->SetParameters(down[0], down[1], down[2]);
      downFunc->SetLineColor(reg == "barrel" ? kBlue : kRed);
      downFunc->SetLineStyle(1); downFunc->SetLineWidth(1);
      syst_down_fits[reg] = downFunc;

      // Printing and writing fit parameters
      printFitFunction(reg, params[0], params[1], params[2]);
      printFitFunction(TString(reg + "_SystUp"), up[0], up[1], up[2]);
      printFitFunction(TString(reg + "_SystDn"), down[0], down[1], down[2]);
      writeFitParameters(out, campaign, trig, reg, func, fitResult, up, down);

      // Creating offline-cut line and label
      TLine* cutLine = new TLine(offlineCut, -0.05, offlineCut, 1.05);
      cutLine->SetLineColor(kGray+2);
      cutLine->SetLineStyle(kDashed);
      cutLine->SetLineWidth(3);
      cut_lines[reg] = cutLine;
      TString labelText = TString::Format("%.0f GeV", offlineCut);
      double x_plus_offset = offlineCut * pow(10, 0.05);
      TLatex* thresholdLabel = new TLatex(x_plus_offset, 0.5, labelText);
      thresholdLabel->SetTextFont(62);
      thresholdLabel->SetTextSize(0.03);
      thresholdLabel->SetTextAngle(90);
      thresholdLabel->SetTextAlign(22);
      thresholdLabel->SetTextColor(kGray+2);
      threshold_labels[reg] = thresholdLabel;

      //-------
      // BAND:
      //-------
      const int nPoints = 100;
      double xBand[nPoints], yUp[nPoints], yDown[nPoints];
      for (int i = 0; i < nPoints; ++i) {
        xBand[i] = fitRangeMin * pow(fitRangeMax / fitRangeMin, i / (double)(nPoints - 1));
        yUp[i] = syst_up_fits[reg]->Eval(xBand[i]);
        yDown[i] = syst_down_fits[reg]->Eval(xBand[i]);
      }
      TGraph* band = new TGraph(2 * nPoints);
      for (int i = 0; i < nPoints; ++i) {
        band->SetPoint(i, xBand[i], yUp[i]);
        band->SetPoint(nPoints + i, xBand[nPoints - 1 - i], yDown[nPoints - 1 - i]);
      }
      band->SetFillColor(reg == "barrel" ? kBlue : kRed);
      band->SetFillStyle(3002);
      band->SetLineColor(0);
      syst_bands[reg] = band;
      
    }//region loop
    
    cout << "\nFitting completed for " << trig << endl;

    // Create and draw plot
    TCanvas* c = createCanvas(TString::Format("c_%s", label.Data()));
    c->SetFillColor(0);
    c->SetFrameFillColor(0);

    TH1F* dummy = new TH1F("dummy", TString::Format("%s Trigger Efficiency", trig.Data()), 50, 10, 250);
    dummy->SetMinimum(-0.05);
    dummy->SetMaximum(1.05);
    dummy->GetXaxis()->SetTitle("p_{T} [GeV]");
    dummy->GetXaxis()->SetLabelSize(0.04);
    dummy->GetXaxis()->SetTitleSize(0.05);
    dummy->GetXaxis()->SetTitleOffset(1.08);
    dummy->GetXaxis()->SetRangeUser(10, 45);
    dummy->GetYaxis()->SetTitle("Trigger Efficiency");
    dummy->GetYaxis()->SetLabelSize(0.04);
    dummy->GetYaxis()->SetTitleSize(0.05);
    dummy->GetYaxis()->SetTitleOffset(1.00);
    dummy->SetStats(0);
    //dummy->GetXaxis()->SetMoreLogLabels(kTRUE);
    dummy->Draw("AXIS");

    for (const auto& reg : regions) {
      if (graphs[reg]) {
	syst_bands[reg]->Draw("F same"); // Draw shaded band first
	fits[reg]->Draw("same"); // Nominal fit (solid line)
        syst_up_fits[reg]->Draw("same"); // SystUp (dashed line)
        syst_down_fits[reg]->Draw("same"); // SystDn (dotted line)
        graphs[reg]->Draw("EP same"); //points
        cut_lines[reg]->Draw("same");
        threshold_labels[reg]->Draw("same");
      }
    }

    TLegend* lg = createLegend();
    lg->SetTextSize(0.03);
    lg->SetHeader(TString::Format("#font[62]{%s}", trig.Data()));
    lg->AddEntry(graphs["barrel"], "Barrel Points", "p");
    lg->AddEntry(graphs["endcap"], "Endcap Points", "p");
    lg->AddEntry(fits["endcap"], "Endcap Fit", "l");
    lg->AddEntry(fits["barrel"], "Barrel Fit", "l");
    lg->AddEntry(syst_bands["barrel"], "#pm1#sigma band", "f");
    lg->Draw();

    addCMSLabel(c, campaign);

    if (!test) {
      c->SaveAs(TString::Format("plots/trigeff_%s_%s.png", campaign.Data(), label.Data()));
    }
  }//trigger loop

  cout << YELLOW << "\nOutput file created: " << outfile << RESET << endl;
  out.close();
}

// Utility functions
void printDataPoints(const TString& trigger, const TString& region, const vector<double>& x, const vector<double>& y, const vector<double>& ey) {
  cout << YELLOW << "Data points for " << trigger << " (" << region << "):" << RESET << endl;
  cout << setw(12) << "pT [GeV]" << setw(15) << "Efficiency" << setw(15) << "Error" << endl;
  cout << string(42, '-') << endl;
  for (size_t i = 0; i < x.size(); ++i) {
    cout << fixed << setprecision(2) << setw(12) << x[i];
    cout << setprecision(6) << setw(15) << y[i] << setw(15) << ey[i] << endl;
  }
}

void printFitFunction(const TString& label, double p0, double p1, double p2) {
  cout << setw(15) << left << label << " "
       << BLUE_BOLD << "eff = 0.5 * "
       << YELLOW_BOLD << fixed << setprecision(6) << p0
       << BLUE_BOLD << " * (1 + Erf((pT - "
       << YELLOW_BOLD << p1
       << BLUE_BOLD << ") / (2 * "
       << YELLOW_BOLD << p2
       << BLUE_BOLD << ")));" << RESET << endl;
}

void writeFitParameters(ofstream& out, const TString& campaign, const TString& trigger, const TString& region, TF1* func, TFitResultPtr fitResult, double* up, double* down) {
  // Writing fit parameters and systematic variations to file

  out<<"---------------"<<endl;
  out << "Region: " << region << endl;
  out<<"---------------\n";
  
  out << fixed << setprecision(6);
  out << setw(15) << left << "Fit function:"<< " eff = 0.5 * " << func->GetParameter(0)
      << " * (1 + Erf((pT - " << func->GetParameter(1) << ") / (2 * " << func->GetParameter(2) << ")));"<<endl;
  out << setw(15) << left << "SystUp:"      << " eff = 0.5 * " << up[0]
      << " * (1 + Erf((pT - " << up[1]      << ") / (2 * " << up[2] << ")));"<<endl;
  out << setw(15) << left << "SystDn:"      << " eff = 0.5 * " << down[0]
      << " * (1 + Erf((pT - " << down[1]    << ") / (2 * " << down[2] << ")));"<<endl;

  TMatrixDSym cov = fitResult->GetCovarianceMatrix();
  out << "\nCorrelation matrix:\n";
  for (int i = 0; i < func->GetNpar(); ++i) {
    for (int j = 0; j < func->GetNpar(); ++j)
      out << setw(10) << right << fitResult->Correlation(i, j);
    out << '\n';
  }
  out << "\nCovariance matrix:\n";
  for (int i = 0; i < cov.GetNrows(); ++i) {
    for (int j = 0; j < cov.GetNcols(); ++j)
      out << setw(10) << right << cov(i, j);
    out << '\n';
  }
  out << "\nParameters  (p0, p1, p2): " << func->GetParameter(0) << ", " << func->GetParameter(1) << ", " << func->GetParameter(2) << '\n';
  out << "Uncertainty (p0, p1, p2): " << sqrt(cov(0,0)) << ", " << sqrt(cov(1,1)) << ", " << sqrt(cov(2,2)) << '\n';

}

TCanvas* createCanvas(TString name) {
  float W = 500, H = 500;
  float T = 0.08 * H, B = 0.12 * H;
  float L = 0.12 * W, R = 0.04 * W;
  TCanvas* c = new TCanvas(name, name, 100, 100, W, H);
  c->SetFillColor(0);
  c->SetBorderMode(0);
  c->SetFrameFillStyle(0);
  c->SetFrameBorderMode(0);
  c->SetLeftMargin(L / W);
  c->SetRightMargin(R / W);
  c->SetTopMargin(T / H);
  c->SetBottomMargin(B / H);
  c->SetTickx(1);
  c->SetTicky(1);
  c->SetGrid(0, 0);
  //c->SetLogx();
  return c;
}

TLegend* createLegend() {
  //TLegend* legend = new TLegend(0.60, 0.18, 0.94, 0.40); //bottom-right
  TLegend* legend = new TLegend(0.18, 0.60, 0.40, 0.88); //top-left
  legend->SetTextSize(0.035);
  legend->SetTextFont(42);
  legend->SetBorderSize(0);
  legend->SetFillColor(0);
  legend->SetFillStyle(3013);
  return legend;
}

void addCMSLabel(TCanvas* canvas, TString campaign) {
  TLatex latex;
  latex.SetNDC();
  float xpos = 0.12, ypos = 0.94, xposright = 0.96;
  latex.SetTextFont(61);
  latex.SetTextSize(0.06);
  latex.DrawLatex(xpos, ypos, "CMS");
  latex.SetTextFont(52);
  latex.SetTextSize(0.05);
  latex.DrawLatex(xpos + 0.13, ypos, "Preliminary");
  latex.SetTextFont(41);
  latex.SetTextAlign(31);
  latex.SetTextSize(0.05);
  if (campaign == "2016preVFP_UL") latex.DrawLatex(xposright, ypos, "2016-preVFP");
  else if (campaign == "2016postVFP_UL") latex.DrawLatex(xposright, ypos, "2016-postVFP");
  else if (campaign == "2017_UL") latex.DrawLatex(xposright, ypos, "2017");
  else if (campaign == "2018_UL") latex.DrawLatex(xposright, ypos, "2018");
  else if (campaign == "Run3Summer22") latex.DrawLatex(xposright, ypos, "2022-preEE");
  else if (campaign == "Run3Summer22EE") latex.DrawLatex(xposright, ypos, "2022-postEE");
  else if (campaign == "Run3Summer23") latex.DrawLatex(xposright, ypos, "2023-preBPix");
  else if (campaign == "Run3Summer23BPix") latex.DrawLatex(xposright, ypos, "2023-postBPix");
  else cerr << RED << "WARNING: Unknown campaign: " << campaign << RESET << endl;
}
