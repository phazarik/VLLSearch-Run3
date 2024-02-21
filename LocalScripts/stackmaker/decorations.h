#ifndef DECORATIONS_H
#define DECORATIONS_H

extern float globalSbyB;
extern float globalObsbyExp;

TCanvas *create_canvas(TString plotname, TString filename, float width, float height){
  //Note: Canvas settings are overruled by Pad settings.
  TCanvas *c1 = new TCanvas(filename,plotname,width,height);
  return c1;
}

TPad *create_mainPad(double x1, double y1, double x2, double y2){
  TPad *pad = new TPad("", "", x1, y1, x2, y2);
  pad->SetLeftMargin(0.1);
  pad->SetRightMargin(0.25);
  pad->SetTopMargin(0.09);
  pad->SetBottomMargin(0.1);
  pad->SetTickx(1);
  pad->SetTicky(1);
  pad->SetGrid();
  pad->SetFrameLineWidth(2);
  pad->SetLogy(1);
  return pad;
}

TPad *create_ratioPad(double x1, double y1, double x2, double y2){
  TPad *pad = new TPad("", "", x1, y1, x2, y2);
  pad->SetLeftMargin(0.1);
  pad->SetRightMargin(0.25);
  pad->SetTopMargin(0.01);
  pad->SetBottomMargin(0.35);
  pad->SetTickx(1);
  pad->SetTicky(1);
  pad->SetGrid();
  pad->SetFrameLineWidth(2);
  return pad;
}

TLegend *create_legend(double x1, double y1, double x2, double y2){
  //TLegend *lg1 = new TLegend(0.87, 0.45, 0.95, 0.90);
  TLegend *lg1 = new TLegend(x1, y1, x2, y2);
  lg1->SetTextFont(62);		
  lg1->SetFillStyle(0);
  lg1->SetBorderSize(0);
  lg1->SetTextSize(0.03);
  //Put globalSbyB as legend header:
  //TMathText *mathText = new TMathText(0.0, 0.0, Form("Global S/#sqrt{B} = %.2f", globalSbyB));
  lg1->SetHeader(Form("Global S/#sqrt{B} = %.2f", globalSbyB));
  
  return lg1;
}

void SetLegendEntry(TLegend *lg, TH1F *hist){
  TString yield = Form("%d", (int)hist->Integral());
  TString name  = hist->GetName();
  TString text  = name + " [" + yield + "]";
  lg->AddEntry(hist, text, "f");
}

void SetHistoStyle(TH1F *h, int color){
  h->SetLineColor(color);
  h->SetLineWidth(2);
  h->SetMarkerStyle(20);
  h->SetMarkerSize(1.2);
  h->SetTitle("");
  //Y-axis
  //h->GetYaxis()->SetRangeUser(0, 1); //This is overruled by the tallest hist.
  h->GetYaxis()->SetTitle("Events");
  h->GetYaxis()->SetTitleSize(0.04);
  h->GetYaxis()->SetTitleOffset(1.10);
  h->GetYaxis()->CenterTitle();
  h->GetYaxis()->SetNdivisions(606);
  h->GetYaxis()->SetLabelSize(0.04);
  h->GetYaxis()->CenterTitle(true);
  //X-axis
  h->GetXaxis()->SetTitle("");
  h->GetXaxis()->SetTitleSize(0.04);
  h->GetXaxis()->SetTitleOffset(1.15);
  h->GetXaxis()->CenterTitle();
  //h->GetXaxis()->SetNdivisions(606);
  //h->GetXaxis()->SetDecimals();
  h->GetXaxis()->SetLabelSize(0); //intead of 0.4
  h->GetXaxis()->CenterTitle(false);
}

void SetRatioStyle(TH1F *srb, TString name){
  srb->SetStats(0);
  //Decorating srb:
  srb->SetLineColor(kBlack);
  srb->SetLineWidth(2);
  srb->SetMarkerStyle(20);
  srb->SetMarkerSize(1.2);
  srb->SetTitle("");
  //Y-axis
  srb->GetYaxis()->SetTitle("S/sqrt{B}");
  srb->GetYaxis()->SetTitleSize(0.13);
  srb->GetYaxis()->CenterTitle(true);
  srb->GetYaxis()->SetTitleOffset(0.30);
  srb->GetYaxis()->SetNdivisions(3, kTRUE);
  srb->GetYaxis()->SetLabelSize(0.13);
  srb->GetYaxis()->SetLabelOffset(0.008);
  //srb->GetYaxis()->SetRangeUser(0, 1);
  //X-axis
  srb->GetXaxis()->SetTitle(name);
  srb->GetXaxis()->SetTitleSize(0.16);
  srb->GetXaxis()->SetTitleOffset(1.00);
  srb->GetXaxis()->SetLabelSize(0.13);
  srb->GetXaxis()->SetLabelOffset(0.008);
}

void put_text(TString text, float x, float y, int style, float size){
  TText* t = new TText(x, y, text);
  t->SetTextSize(size);
  t->SetNDC();
  t->SetTextFont(style); // Bold
  t->Draw();
}

void put_latex_text(TString text, float x, float y, int style, float size){
  TLatex* latex = new TLatex(); 
  latex->SetTextFont(style);
  latex->SetTextSize(size);
  latex->SetNDC();
  latex->DrawLatex(x, y, text);  
}

#endif // DECORATIONS_H
