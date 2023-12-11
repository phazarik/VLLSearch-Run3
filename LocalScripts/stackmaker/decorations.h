#ifndef DECORATIONS_H
#define DECORATIONS_H

TCanvas *create_canvas(TString plotname, float width, float height){
  TCanvas *c1 = new TCanvas(plotname,plotname,width,height);
  c1->SetGrid();
  c1->SetFrameLineWidth(2);
  /*
  c1->SetTopMargin(0.05);
  c1->SetBottomMargin(0.12);
  c1->SetLeftMargin(0.12);
  c1->SetRightMargin(0.18);*/
  return c1;
}

TPad *create_mainPad(double x1, double y1, double x2, double y2){
  TPad *pad = new TPad("", "", x1, y1, x2, y2);
  pad->SetLeftMargin(0.1);
  pad->SetRightMargin(0.25);
  pad->SetTopMargin(0.09);
  pad->SetBottomMargin(0.12);
  pad->SetTickx(1);
  pad->SetTicky(1);
  return pad;
}

TPad *create_ratioPad(double x1, double y1, double x2, double y2){
  TPad *pad = new TPad("", "", x1, y1, x2, y2);
  pad->SetLeftMargin(0.1);
  pad->SetRightMargin(0.25);
  pad->SetTopMargin(0.09);
  pad->SetBottomMargin(0.12);
  pad->SetTickx(1);
  pad->SetTicky(1);
  return pad;
}

TLegend *create_legend(double x1, double y1, double x2, double y2){
  //TLegend *lg1 = new TLegend(0.87, 0.45, 0.95, 0.90);
  TLegend *lg1 = new TLegend(x1, y1, x2, y2);
  lg1->SetTextFont(62);		
  lg1->SetFillStyle(0);
  lg1->SetBorderSize(0);
  lg1->SetTextSize(0.025);
  return lg1;
}

void SetHistoStyle(TH1F *h, TString xtitle, TString plottitle, int color){
  h->SetLineColor(color);
  h->SetLineWidth(2);
  h->SetMarkerStyle(20);
  h->SetMarkerSize(1.2);
  h->SetTitle("");
  //Y-axis
  //h->GetYaxis()->SetRangeUser(0, 1); //This is overruled by the tallest hist.
  h->GetYaxis()->SetTitle("Events");
  h->GetYaxis()->SetTitleSize(0.04);
  h->GetYaxis()->SetTitleOffset(1.38);
  h->GetYaxis()->CenterTitle();
  h->GetYaxis()->SetNdivisions(606);
  h->GetYaxis()->SetLabelSize(0.04);
  //X-axis
  h->GetXaxis()->SetTitle(xtitle);
  h->GetXaxis()->SetTitleSize(0.04);
  h->GetXaxis()->SetTitleOffset(1.15);
  h->GetXaxis()->CenterTitle();
  h->GetXaxis()->SetNdivisions(606);
  //h->GetXaxis()->SetDecimals();
  h->GetXaxis()->SetLabelSize(0.04);
}

void put_text(){
  TText* cmsText = new TText(0.12, 0.93, "CMS");
  cmsText->SetTextSize(0.06);
  cmsText->SetNDC();
  cmsText->SetTextFont(62); // Bold
  cmsText->Draw();
  
  TText* preliminaryText = new TText(0.22, 0.93, "preliminary");
  preliminaryText->SetTextSize(0.05);
  preliminaryText->SetNDC();
  preliminaryText->SetTextFont(52); // Italics
  preliminaryText->Draw();
  
  TLatex* latex = new TLatex(); 
  latex->SetTextFont(42);
  latex->SetTextSize(0.04);
  latex->SetNDC();
  latex->DrawLatex(0.71, 0.93, "(2018) 59.8 fb^{-1}");  
}

#endif // DECORATIONS_H
