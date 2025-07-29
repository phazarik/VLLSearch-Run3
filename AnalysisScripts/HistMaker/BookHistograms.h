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
  h.vllep[0] = new TH1F("vllep_all_pt",        "p_T(E)",  500, 0, 500);
  h.vllep[1] = new TH1F("vllep_all_eta",       "#eta(E)", 100, -4, 4);
  h.vllep[2] = new TH1F("vllep_all_phi",       "#phi(E)", 100, -4, 4);
  h.vllep[3] = new TH1F("vllep_all_M",         "M_E (GeV)", 1500, 0, 1500);
  h.vllep[4] = new TH1F("vllep_all_pdgid",     "pdgId(E)", 60, -30, 30);
  h.vllep[5] = new TH1F("vllep_all_dau_pdgid", "pdgId(daughter(E))", 60, -30, 30);

  h.vlnu.resize(6);
  h.vlnu[0] = new TH1F("vlnu_all_pt",        "p_T(N)",  500, 0, 500);
  h.vlnu[1] = new TH1F("vlnu_all_eta",       "#eta(N)", 100, -4, 4);
  h.vlnu[2] = new TH1F("vlnu_all_phi",       "#phi(N)", 100, -4, 4);
  h.vlnu[3] = new TH1F("vlnu_all_M",         "M_N (GeV)", 1500, 0, 1500);
  h.vlnu[4] = new TH1F("vlnu_all_pdgid",     "pdgId(N)", 60, -30, 30);
  h.vlnu[5] = new TH1F("vlnu_all_dau_pdgid", "pdgId(daughter(N))", 60, -30, 30);

  //Production modes and decay modes:
  h.mode.resize(4);
  h.mode[0] = new TH1F("prod_modes", "prod modes (0=EE, 1=EN, 2=NN, -1=invalid)", 7, -2, 5);
  h.mode[1] = new TH1F("decay_modes_EE", "decay of EE (0=ZZ, 1=ZH, 2=HH, -1=invalid)", 7, -2, 5);
  h.mode[2] = new TH1F("decay_modes_EN", "decay of EN (0=ZW, 1=HW, -1=invalid)", 7, -2, 5);
  h.mode[3] = new TH1F("decay_modes_NN", "decay of NN (0=WW, -1=invalid)", 7, -2, 5);
  h.mode[0]->GetXaxis()->SetBinLabel(2, "invalid");
  h.mode[0]->GetXaxis()->SetBinLabel(3, "EE");
  h.mode[0]->GetXaxis()->SetBinLabel(4, "EN");
  h.mode[0]->GetXaxis()->SetBinLabel(5, "NN");
  h.mode[1]->GetXaxis()->SetBinLabel(2, "invalid");
  h.mode[1]->GetXaxis()->SetBinLabel(3, "ZZ");
  h.mode[1]->GetXaxis()->SetBinLabel(4, "ZH");
  h.mode[1]->GetXaxis()->SetBinLabel(5, "HH");
  h.mode[2]->GetXaxis()->SetBinLabel(2, "invalid");
  h.mode[2]->GetXaxis()->SetBinLabel(3, "ZW");
  h.mode[2]->GetXaxis()->SetBinLabel(4, "HW");
  h.mode[3]->GetXaxis()->SetBinLabel(2, "invalid");
  h.mode[3]->GetXaxis()->SetBinLabel(3, "WW");

  h.gen.resize(23);
  // di-VLL system
  h.gen[0] = new TH1F("deltaphi_LL", "#Delta#phi(E/N, E/N)", 100, 0, 6);
  h.gen[1] = new TH1F("deltaR_LL", "#Delta R(E/N, E/N)", 100, 0, 6);
  // Muons
  h.gen[2] = new TH1F("ngenmu", "N_{gen #mu}", 10, 0, 10);
  h.gen[3] = new TH1F("genmu_pt", "p_{T}(gen #mu)", 500, 0, 500);
  h.gen[4] = new TH1F("genmu_eta", "#eta(gen #mu)", 100, -4, 4);
  h.gen[5] = new TH1F("genmu_phi", "#phi(gen #mu)", 100, -4, 4);
  // Electrons
  h.gen[6] = new TH1F("ngenele", "N_{gen e}", 10, 0, 10);
  h.gen[7] = new TH1F("genele_pt", "p_{T}(gen e)", 500, 0, 500);
  h.gen[8] = new TH1F("genele_eta", "#eta(gen e)", 100, -4, 4);
  h.gen[9] = new TH1F("genele_phi", "#phi(gen e)", 100, -4, 4);
  // Light leptons
  h.gen[10] = new TH1F("ngenlep", "N_{gen e/#mu}", 10, 0, 10);
  h.gen[11] = new TH1F("genlep_pt", "p_{T}(gen e/#mu)", 500, 0, 500);
  h.gen[12] = new TH1F("genlep_eta", "#eta(gen e/#mu)", 100, -4, 4);
  h.gen[13] = new TH1F("genlep_phi", "#phi(gen e/#mu)", 100, -4, 4);
  // Jets
  h.gen[14] = new TH1F("ngenjet", "N_{gen jet}", 15, 0, 15);
  h.gen[15] = new TH1F("genjet_pt", "p_{T}(gen jet)", 500, 0, 500);
  h.gen[16] = new TH1F("genjet_eta", "#eta(gen jet)", 100, -4, 4);
  h.gen[17] = new TH1F("genjet_phi", "#phi(gen jet)", 100, -4, 4);
  // Event-level
  h.gen[18] = new TH1F("genHT", "gen-H_{T}", 500, 0, 1500);
  h.gen[19] = new TH1F("genLT", "gen-L_{T}", 500, 0, 1500);
  h.gen[20] = new TH1F("genST", "gen-S_{T}", 500, 0, 1500);
  h.gen[21] = new TH1F("genMET_pt", "gen-p_{T}^{\text{ miss}}", 500, 0, 500);
  h.gen[22] = new TH1F("genMET_phi", "gen#phi^{\text{ miss}}", 100, -4, 4);

  h.reco.resize(25);
  // Muons
  h.reco[0]  = new TH1F("nrecomu",   "N_{reco #mu}", 10, 0, 10);
  h.reco[1]  = new TH1F("recomu_pt", "p_{T}(reco #mu)", 500, 0, 500);
  h.reco[2]  = new TH1F("recomu_eta","#eta(reco #mu)", 100, -4, 4);
  h.reco[3]  = new TH1F("recomu_phi","#phi(reco #mu)", 100, -4, 4);
  // Electrons
  h.reco[4]  = new TH1F("nrecoele",   "N_{reco e}", 10, 0, 10);
  h.reco[5]  = new TH1F("recoele_pt", "p_{T}(reco e)", 500, 0, 500);
  h.reco[6]  = new TH1F("recoele_eta","#eta(reco e)", 100, -4, 4);
  h.reco[7]  = new TH1F("recoele_phi","#phi(reco e)", 100, -4, 4);
  // Light leptons (e/μ)
  h.reco[8]  = new TH1F("nrecolep",   "N_{reco e/#mu}", 10, 0, 10);
  h.reco[9]  = new TH1F("recolep_pt", "p_{T}(reco e/#mu)", 500, 0, 500);
  h.reco[10] = new TH1F("recolep_eta","#eta(reco e/#mu)", 100, -4, 4);
  h.reco[11] = new TH1F("recolep_phi","#phi(reco e/#mu)", 100, -4, 4);
  // Jets
  h.reco[12] = new TH1F("nrecojet",   "N_{reco jet}", 15, 0, 15);
  h.reco[13] = new TH1F("recojet_pt", "p_{T}(reco jet)", 500, 0, 500);
  h.reco[14] = new TH1F("recojet_eta","#eta(reco jet)", 100, -4, 4);
  h.reco[15] = new TH1F("recojet_phi","#phi(reco jet)", 100, -4, 4);
  // bJets
  h.reco[16] = new TH1F("nbjet",   "N_{b jet}", 10, 0, 10);
  h.reco[17] = new TH1F("bjet_pt", "p_{T}(b jet)", 500, 0, 500);
  h.reco[18] = new TH1F("bjet_eta","#eta(b jet)", 100, -4, 4);
  h.reco[19] = new TH1F("bjet_phi","#phi(b jet)", 100, -4, 4);
  // Event-level
  h.reco[20] = new TH1F("recoHT",  "reco-H_{T}", 500, 0, 1500);
  h.reco[21] = new TH1F("recoLT",  "reco-L_{T}", 500, 0, 1500);
  h.reco[22] = new TH1F("recoST",  "reco-S_{T}", 500, 0, 1500);
  h.reco[23] = new TH1F("recoMET_pt",  "reco-p_{T}^{miss}", 500, 0, 500);
  h.reco[24] = new TH1F("recoMET_phi", "reco#phi^{miss}", 100, -4, 4);
  
}
