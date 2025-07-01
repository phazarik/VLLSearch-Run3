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

  //---------------
  // Signal study:
  //---------------
  
  //Properties of VLLs:
  h.vllep.resize(6);
  h.vllep[0] = new TH1F("vllep_all_pt",        "p_T(L)",  500, 0, 500);
  h.vllep[1] = new TH1F("vllep_all_eta",       "#eta(L)", 100, -4, 4);
  h.vllep[2] = new TH1F("vllep_all_phi",       "#phi(L)", 100, -4, 4);
  h.vllep[3] = new TH1F("vllep_all_M",         "M(L)", 1000, 0, 1000);
  h.vllep[4] = new TH1F("vllep_all_pdgid",     "pdgId(L)", 60, -30, 30);
  h.vllep[5] = new TH1F("vllep_all_dau_pdgid", "pdgId(daughter(L))", 60, -30, 30);

  h.vlnu.resize(6);
  h.vlnu[0] = new TH1F("vlnu_all_pt",        "p_T(L)",  500, 0, 500);
  h.vlnu[1] = new TH1F("vlnu_all_eta",       "#eta(N)", 100, -4, 4);
  h.vlnu[2] = new TH1F("vlnu_all_phi",       "#phi(N)", 100, -4, 4);
  h.vlnu[3] = new TH1F("vlnu_all_M",         "M(N)", 1000, 0, 1000);
  h.vlnu[4] = new TH1F("vlnu_all_pdgid",     "pdgId(N)", 60, -30, 30);
  h.vlnu[5] = new TH1F("vlnu_all_dau_pdgid", "pdgId(daughter(N))", 60, -30, 30);

  //Production modes:
  h.mode.resize(3);
  h.mode[0] = new TH1F("LL_dphi_vlls", "d#phi(L, L)", 100, 0, 4);
  h.mode[1] = new TH1F("LN_dphi_vlls", "d#phi(L, N)", 100, 0, 4);
  h.mode[2] = new TH1F("NN_dphi_vlls", "d#phi(N, N)", 100, 0, 4);
  
}
