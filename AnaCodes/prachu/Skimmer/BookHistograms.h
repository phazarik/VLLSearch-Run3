void AnaScript::BookHistograms(){

  h.nevt = new TH1F("nEvents", "0-nEvtTotal, 1-nEvtRan, 2-nEvtTrigger, 3-nEvtPass, 4-nEvtPass(wt)",5,0,5); //h.nevt->Sumw2();
  h.evtweight[0] = new TH1F("wt_SF","Reco-ID-Iso Scale Factor",200,0,2);
  h.evtweight[1] = new TH1F("wt_trigger","Trigger Scale Factor",200,0,2);
  h.evtweight[2] = new TH1F("wt_bjet","bJet weight",200,0,2);
  h.evtweight[3] = new TH1F("wt_evt","Event weight",200,0,2);
  h.evtweight[4] = new TH1F("wt_lumi","LumiScale",3000,0,3000);
  //for(int i=0; i<3; i++)h.evtweight[i]->Sumw2();

  //Checking the triggers:
  h.hist[0] = new TH1F("Flag_goodVertices", "Flag_goodVertices", 5, 0, 5);
  h.hist[1] = new TH1F("Flag_globalSuperTightHalo2016Filter", "Flag_globalSuperTightHalo2016Filter", 5, 0, 5);
  h.hist[2] = new TH1F("Flag_HBHENoiseFilter", "Flag_HBHENoiseFilter", 5, 0, 5);
  h.hist[3] = new TH1F("Flag_EcalDeadCellTriggerPrimitiveFilter", "Flag_EcalDeadCellTriggerPrimitiveFilter", 5, 0, 5);
  h.hist[4] = new TH1F("Flag_BadPFMuonFilter", "Flag_BadPFMuonFilter", 5, 0, 5);
  h.hist[5] = new TH1F("HLT_IsoMu24", "HLT_IsoMu24", 5, 0, 5);
  h.hist[6] = new TH1F("HLT_IsoMu27", "HLT_IsoMu27", 5, 0, 5);
  h.hist[7] = new TH1F("HLT_Ele27_WPTight_Gsf", "HLT_Ele27_WPTight_Gsf", 5, 0, 5);
  h.hist[8] = new TH1F("HLT_Ele32_WPTight_Gsf", "HLT_Ele32_WPTight_Gsf", 5, 0, 5);
  h.hist[9] = new TH1F("Flag_ValidDecayMode", "Flag_ValidDecayMode (0=bad, 1=good)", 5, 0, 5);
  h.hist[10] = new TH1F("Flag_Zwindow", "Flag_Zwindow (0=onZ, 1=offZ, 2=sideband)", 5, 0, 5);
  h.hist[11] = new TH1F("onZ_ptbins", "onZ_ptbins", 5, 0, 5);
  h.hist[12] = new TH1F("sideZ_ptbins", "sideZ_ptbins", 5, 0, 5);
  h.hist[13] = new TH1F("TTbarAcceptanceStudy", "0=all, 1=basic, 2=cuts, 3=basic2, 4=cuts2", 10, 0, 10);
  for(int i=0; i<14; i++) h.hist[i]->Sumw2();
  
  //############################################
  //Object level plots: (before event selection)
  //############################################
  //Electrons:
  /*
  h.ele[0] = new TH1F("nElectrons","nElectrons",10,0,10);
  h.ele[1] = new TH1F("allele_pt","allele_pt",500,0,500);
  h.ele[2] = new TH1F("allele_eta","allele_eta",200,-4,4);
  h.ele[3] = new TH1F("allele_phi","allele_phi",200,-4,4);
  h.ele[4] = new TH1F("allele_reliso03","allele_reliso03",200,0,1);
  for(int i=0; i<5; i++)h.ele[i]->Sumw2();
  //Muons:
  h.mu[0] = new TH1F("nMuons","nMuons",10,0,10);
  h.mu[1] = new TH1F("allmu_pt","allmu_pt",500,0,500);
  h.mu[2] = new TH1F("allmu_eta","allmu_eta",200,-4,4);
  h.mu[3] = new TH1F("allmu_phi","allmu_phi",200,-4,4);
  h.mu[4] = new TH1F("allmu_reliso03","allmu_reliso03",200,0,1);
  for(int i=0; i<5; i++)h.mu[i]->Sumw2();
  //Photons:
  h.pho[0] = new TH1F("nPhotons","nPhotons",10,0,10);
  h.pho[1] = new TH1F("allpho_pt","allpho_pt",500,0,500);
  h.pho[2] = new TH1F("allpho_eta","allpho_eta",200,-4,4);
  h.pho[3] = new TH1F("allpho_phi","allpho_phi",200,-4,4);
  h.pho[4] = new TH1F("allpho_reliso03","allpho_reliso03",200,0,1);
  for(int i=0; i<5; i++)h.pho[i]->Sumw2();
  //Taus:
  h.tau[0] = new TH1F("nTaus","nTaus",10,0,10);
  h.tau[1] = new TH1F("alltau_pt","alltau_pt",500,0,500);
  h.tau[2] = new TH1F("alltau_eta","alltau_eta",200,-4,4);
  h.tau[3] = new TH1F("alltau_phi","alltau_phi",200,-4,4);
  for(int i=0; i<4; i++)h.tau[i]->Sumw2();
  //Jets:
  h.jet[0] = new TH1F("nJets","nJets",10,0,10);
  h.jet[1] = new TH1F("alljet_pt","alljet_pt",500,0,500);
  h.jet[2] = new TH1F("alljet_eta","alljet_eta",200,-4,4);
  h.jet[3] = new TH1F("alljet_phi","alljet_phi",200,-4,4);
  for(int i=0; i<4; i++)h.jet[i]->Sumw2();
  //bJets:
  h.bjet[0] = new TH1F("nbJets","nbJets",10,0,10);
  h.bjet[1] = new TH1F("allbjet_pt","allbjet_pt",500,0,500);
  h.bjet[2] = new TH1F("allbjet_eta","allbjet_eta",200,-4,4);
  h.bjet[3] = new TH1F("allbjet_phi","allbjet_phi",200,-4,4);
  for(int i=0; i<4; i++)h.bjet[i]->Sumw2();*/

  //SignalStudy:
  /*
  h.sig[0] = new TH1F("genPart_pdgId_all", "genPart_pdgId_all", 2000, -1000, 1000);
  h.vll[0] = new TH1F("nvll",         "nvll",          10, 0, 10);
  h.vll[1] = new TH1F("allvll_pt",    "allvll_pt",   1000,0,1000);
  h.vll[2] = new TH1F("allvll_eta",   "allvll_eta",     200,-4,4);
  h.vll[3] = new TH1F("allvll_phi",   "allvll_phi",     200,-4,4);
  h.vll[4] = new TH1F("allvll_mass",  "allvll_mass", 2500,0,2500);
  h.vll[5] = new TH1F("allvll_charge","allvll_charge",    6,-3,3);
  h.vll[6] = new TH1F("allvll_decay", "allvll_decay",    5, 0, 5);
  h.vll[7] = new TH1F("allvll_daughers", "allvll_daughers", 100, -50, 50);
  
  h.vln[0] = new TH1F("nvlnu",         "nvllnu",         10, 0, 10);
  h.vln[1] = new TH1F("allvlnu_pt",    "allvlnu_pt",   1000,0,1000);
  h.vln[2] = new TH1F("allvlnu_eta",   "allvlnu_eta",     200,-4,4);
  h.vln[3] = new TH1F("allvlnu_phi",   "allvlnu_phi",     200,-4,4);
  h.vln[4] = new TH1F("allvlnu_mass",  "allvlnu_mass", 2500,0,2500);
  h.vln[5] = new TH1F("allvlnu_charge","allvlnu_charge",    6,-3,3);
  h.vln[6] = new TH1F("allvlnu_decay", "allvlnu_decay",    5, 0, 5);
  h.vln[7] = new TH1F("allvlnu_daughers", "allvll_daughers", 100, -50, 50);*/

  // VLL pair production final states:
  h.sig[1] = new TH1F("finalstates_for_LL", "finalstates for LL", 10, 0, 10);
  h.sig[2] = new TH1F("finalstates_for_LN", "finalstates for LN", 10, 0, 10);
  h.sig[3] = new TH1F("finalstates_for_NN", "finalstates for NN", 10, 0, 10);
  h.sig[4] = new TH1F("finalstates_incldecay", "finalstates for inclusive decaymode", 10, 0, 10);

  //##############
  // Final states:
  //##############

  h.evt2LSS[0]  = new TH1F("nlep", "nlep", 10, 0, 10);
  h.evt2LSS[1]  = new TH1F("njet", "njet", 10, 0, 10);
  h.evt2LSS[2]  = new TH1F("nbjet", "nbjet", 10, 0, 10);

  h.evt2LSS[3]  = new TH1F("lep0_pt",  "lep0_pt",  50, 0, 500);
  h.evt2LSS[4]  = new TH1F("lep0_eta", "lep0_eta", 100, -4, 4);
  h.evt2LSS[5]  = new TH1F("lep0_phi", "lep0_phi", 100, -4, 4);
  h.evt2LSS[6]  = new TH1F("lep0_iso", "lep0_iso", 200, 0, 1);
  h.evt2LSS[7]  = new TH1F("lep0_mt",  "lep0_mt",  50, 0, 500);

  h.evt2LSS[8]  = new TH1F("lep1_pt",  "lep1_pt",  50, 0, 500);
  h.evt2LSS[9]  = new TH1F("lep1_eta", "lep1_eta", 100, -4, 4);
  h.evt2LSS[10] = new TH1F("lep1_phi", "lep1_phi", 100, -4, 4);
  h.evt2LSS[11] = new TH1F("lep1_iso", "lep1_iso", 200, 0, 1);
  h.evt2LSS[12] = new TH1F("lep1_mt",  "lep1_mt",  50, 0, 500);

  h.evt2LSS[13] = new TH1F("dilep_pt",  "dilep_pt",  50, 0, 500);
  h.evt2LSS[14] = new TH1F("dilep_eta", "dilep_eta", 100, -10, 10);
  h.evt2LSS[15] = new TH1F("dilep_phi", "dilep_phi", 100, -4, 4);
  h.evt2LSS[16] = new TH1F("dilep_mass","dilep_mass",50, 0, 500);
  h.evt2LSS[17] = new TH1F("dilep_mt",  "dilep_mt",  50, 0, 500);
  h.evt2LSS[18] = new TH1F("dilep_deta", "dilep_deta", 100, 0, 6);
  h.evt2LSS[19] = new TH1F("dilep_dphi", "dilep_dphi", 100, 0, 6);
  h.evt2LSS[20] = new TH1F("dilep_dR",   "dilep_dR",   100, 0, 6);
  h.evt2LSS[21] = new TH1F("dilep_ptratio","dilep_ptratio",100, 0, 1);

  h.evt2LSS[22] = new TH1F("HT", "HT", 20, 0, 500);
  h.evt2LSS[23] = new TH1F("STvis", "STvis", 50, 0, 500);
  h.evt2LSS[24] = new TH1F("STfrac", "STfrac", 100, 0, 1.1); //For nJet-==0, STfrac=1
  
  h.evt2LSS[25] = new TH1F("dphi_metlep0",    "dilep_metlep0",    100, 0, 4);
  h.evt2LSS[26] = new TH1F("dphi_metlep1",    "dilep_metlep1",    100, 0, 4);
  h.evt2LSS[27] = new TH1F("dphi_metdilep",   "dilep_metdilep",   100, 0, 4);
  h.evt2LSS[28] = new TH1F("dphi_metlep_max", "dilep_metlep_max", 100, 0, 4);
  h.evt2LSS[29] = new TH1F("dphi_metlep_min", "dilep_metlep_min", 100, 0, 4);

  h.evt2LSS[30] = new TH1F("MET", "MET", 50, 0, 500);
  h.evt2LSS[31] = new TH1F("MET_phi", "MET_phi", 100, -4, 4);
  h.evt2LSS[32] = new TH1F("ST", "ST", 50, 0, 500);

  /*
  //Charge and ID check:
  h.evt2LSS[32] = new TH1F("charge_lep0", "charge_lep0", 10, -5, 5);
  h.evt2LSS[33] = new TH1F("charge_lep1", "charge_lep1", 10, -5, 5);
  h.evt2LSS[34] = new TH1F("charge_dilep", "charge_dilep", 10, -5, 5);
  h.evt2LSS[35] = new TH1F("id_lep0", "id_lep0", 5, 10, 15);
  h.evt2LSS[36] = new TH1F("id_lep1", "id_lep1", 5, 10, 15);*/
  
  for(int i=0; i<33; i++) h.evt2LSS[i]->Sumw2();

  /*
  //Gen-Level study:
  h.gen2LSS[0]  = new TH1F("2LSS_count",         "0=all, 1=ee, 2=em, 3=mm", 5, 0, 5);

  h.gen2LSS[1]  = new TH1F("ee_dRmin_lep0",      "ee dRmin lep0", 50, 0, 1);
  h.gen2LSS[2]  = new TH1F("ee_dRmin_lep1",      "ee dRmin lep1", 50, 0, 1);
  h.gen2LSS[3]  = new TH1F("ee_diffcharge_lep0", "ee reco_lep0 charge-gen_lep0 charge",  10, -5, 5);
  h.gen2LSS[4]  = new TH1F("ee_diffcharge_lep1", "ee reco_lep1 charge-gen_lep1 charge",  10, -5, 5);
  h.gen2LSS[5]  = new TH1F("ee_genmatch_rate",   "ee 0=all, 1=lep0match, 2=lep1match, 3=bothmatch",  5, 0, 5);
  h.gen2LSS[6]  = new TH1F("ee_chargematch_rate","ee 0=all, 1=lep0match, 2=lep1match, 3=bothmatch",  5, 0, 5);

  h.gen2LSS[7]  = new TH1F("em_dRmin_lep0",      "em dRmin lep0", 50, 0, 1);
  h.gen2LSS[8]  = new TH1F("em_dRmin_lep1",      "em dRmin lep1", 50, 0, 1);
  h.gen2LSS[9]  = new TH1F("em_diffcharge_lep0", "em reco_lep0 charge-gen_lep0 charge",  10, -5, 5);
  h.gen2LSS[10] = new TH1F("em_diffcharge_lep1", "em reco_lep1 charge-gen_lep1 charge",  10, -5, 5);
  h.gen2LSS[11] = new TH1F("em_genmatch_rate",   "em 0=all, 1=lep0match, 2=lep1match, 3=bothmatch",  5, 0, 5);
  h.gen2LSS[12] = new TH1F("em_chargematch_rate","em 0=all, 1=lep0match, 2=lep1match, 3=bothmatch",  5, 0, 5);

  h.gen2LSS[13]  = new TH1F("mm_dRmin_lep0",      "mm dRmin lep0", 50, 0, 1);
  h.gen2LSS[14]  = new TH1F("mm_dRmin_lep1",      "mm dRmin lep1", 50, 0, 1);
  h.gen2LSS[15]  = new TH1F("mm_diffcharge_lep0", "mm reco_lep0 charge-gen_lep0 charge",  10, -5, 5);
  h.gen2LSS[16]  = new TH1F("mm_diffcharge_lep1", "mm reco_lep1 charge-gen_lep1 charge",  10, -5, 5);
  h.gen2LSS[17]  = new TH1F("mm_genmatch_rate",   "mm 0=all, 1=lep0match, 2=lep1match, 3=bothmatch",  5, 0, 5);
  h.gen2LSS[18]  = new TH1F("mm_chargematch_rate","mm 0=all, 1=lep0match, 2=lep1match, 3=bothmatch",  5, 0, 5);
  
  for(int i=0; i<19; i++) h.gen2LSS[i]->Sumw2();

  h.btagsf[0] = new TH1F("JetHadronFlavor_all", "JetHadronFlavor_all", 10, -2, 8);
  //2D plots for bTagSF calculation:
  float x_bin_pT[10] = {20,30,50,70,100,140,200,300,600,1000};
  float y_bin_eta[4] = {0.0,0.8,1.6,3.0};
  h.bJets[0] = new TH2F("bJet_PtEta","bJet_PtEta",(sizeof(x_bin_pT)/ sizeof(x_bin_pT[0])-1),x_bin_pT,(sizeof(y_bin_eta)/sizeof(y_bin_eta[0])-1),y_bin_eta);
  h.bJets[1] = new TH2F("bJet_MedWP_PtEta","bJet_MedWP_PtEta",(sizeof(x_bin_pT)/ sizeof(x_bin_pT[0])-1),x_bin_pT,(sizeof(y_bin_eta)/sizeof(y_bin_eta[0])-1),y_bin_eta);
  h.cJets[0] = new TH2F("cJet_PtEta","cJet_PtEta",(sizeof(x_bin_pT)/ sizeof(x_bin_pT[0])-1),x_bin_pT,(sizeof(y_bin_eta)/sizeof(y_bin_eta[0])-1),y_bin_eta);
  h.cJets[1] = new TH2F("cJet_Mis_PtEta","cJet_Mis_PtEta",(sizeof(x_bin_pT)/ sizeof(x_bin_pT[0])-1),x_bin_pT,(sizeof(y_bin_eta)/sizeof(y_bin_eta[0])-1),y_bin_eta);
  h.lJets[0] = new TH2F("LightJet_PtEta","LightJet_PtEta",(sizeof(x_bin_pT)/ sizeof(x_bin_pT[0])-1),x_bin_pT,(sizeof(y_bin_eta)/sizeof(y_bin_eta[0])-1),y_bin_eta);
  h.lJets[1] = new TH2F("LightJet_Mis_PtEta","LightJet_Mis_PtEta",(sizeof(x_bin_pT)/ sizeof(x_bin_pT[0])-1),x_bin_pT,(sizeof(y_bin_eta)/sizeof(y_bin_eta[0])-1),y_bin_eta);*/
  
}
