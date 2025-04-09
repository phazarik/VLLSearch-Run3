void AnaScript::BookHistograms(){
  
  //Random histograms:
  h.hist.resize(1);
  h.hist[0] = new TH1F("JetHadronFlavor_all", "JetHadronFlavor_all", 10, -2, 8); //make_bJetSFPlots.h
  for(int i = 0; i < (int)h.hist.size(); i++) h.hist[i]->Sumw2();
  
  //MC corrections:
  h.correction.resize(7);
  h.correction[0] = new TH1F("sf_jec",  "JEC",        200, 0, 2);
  h.correction[1] = new TH1F("sf_jer",  "JER",        200, 0, 2);
  h.correction[2] = new TH1F("wt_lep",  "lep wt",     200, 0, 2);
  h.correction[3] = new TH1F("wt_trig", "teigger wt", 200, 0, 2);
  h.correction[4] = new TH1F("wt_pu",   "pileup wt",  200, 0, 2);
  h.correction[5] = new TH1F("wt_bjet", "bjet wt",    200, 0, 2);
  h.correction[6] = new TH1F("wt_evt",  "event wt",   200, 0, 2);
  for(int i = 0; i < (int)h.correction.size(); i++) h.correction[i]->Sumw2();

  //2D plots for bTagSF calculation:
  vector<float> ptBins = {20, 30, 50, 70, 100, 140, 200, 300, 600, 1000};
  vector<float> etaBins = {0.0, 0.8, 1.6, 3.0};
  
  h.bJets.resize(2); 
  h.bJets[0] = new TH2F("bJet_PtEta", "bJet_PtEta", ptBins.size()-1, &ptBins[0], etaBins.size()-1, &etaBins[0]);
  h.bJets[1] = new TH2F("bJet_MedWP_PtEta", "bJet_MedWP_PtEta", ptBins.size()-1, &ptBins[0],etaBins.size()-1,&etaBins[0]);

  h.cJets.resize(2);
  h.cJets[0] = new TH2F("cJet_PtEta", "cJet_PtEta", ptBins.size()-1, &ptBins[0], etaBins.size()-1, &etaBins[0]);
  h.cJets[1] = new TH2F("cJet_Mis_PtEta", "cJet_Mis_PtEta", ptBins.size()-1, &ptBins[0], etaBins.size()-1, &etaBins[0]);

  h.lJets.resize(2);
  h.lJets[0] = new TH2F("LightJet_PtEta", "LightJet_PtEta", ptBins.size()-1, &ptBins[0], etaBins.size()-1, &etaBins[0]);
  h.lJets[1] = new TH2F("LightJet_Mis_PtEta", "LightJet_Mis_PtEta", ptBins.size()-1, &ptBins[0], etaBins.size()-1, &etaBins[0]);

}
