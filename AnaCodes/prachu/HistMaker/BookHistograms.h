void AnaScript::BookHistograms(){

  h.nevt = new TH1F("nEvents", "0-nEvtTotal, 1-nEvtRan, 2-nEvtTrigger, 3-nEvtPass, 4-nEvtPass(wt)",5,0,5); //h.nevt->Sumw2();
  h.evtweight[0] = new TH1F("wt_SF","Reco-ID-Iso Scale Factor",200,0,2);
  h.evtweight[1] = new TH1F("wt_trigger","Trigger Scale Factor",200,0,2);
  h.evtweight[2] = new TH1F("wt_pileup","pileup weight",200,0,2);
  h.evtweight[3] = new TH1F("wt_bjet","bJet weight",200,0,2);
  h.evtweight[4] = new TH1F("wt_evt","Event weight",200,0,2);
  //h.evtweight[4] = new TH1F("wt_lumi","LumiScale",3000,0,3000);
  h.evtweight[5] = new TH1F("wt_jec","Jet Energy Correction for each Jet",200,0,2);
  h.evtweight[6] = new TH1F("wt_jer","Jet Energy Resolution Correction for each Jet",200,0,2);
  h.evtweight[7] = new TH1F("wt_jecjer","JEC*JER for each Jet",200,0,2);
  //More of these below.

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

  //Trigger overlap counts:
  h.count[0] = new TH1F("overlap_before_trigger", "0-mupass, 1-mufail, 2-epass, 3-efail, 4-overlap", 10, 0, 10);
  h.count[1] = new TH1F("overlap_after_trigger", "0-mupass, 1-mufail, 2-epass, 3-efail, 4-overlap", 10, 0, 10);
  h.count[2] = new TH1F("overlap_ofIsoEle27", "0-27pass, 1-27fail, 2-32pass, 3-32fail, 4-bothpass, 5-bothfail", 10, 0, 10);

  //Charge-flip measurement:
  //h.chargeflip[0] = new TH1F("chargeflip_rate_lowpt",  "0=All, 1=onZ, 2=sideband (low pt)",  5, 0, 5);
  //h.chargeflip[1] = new TH1F("chargeflip_rate_medpt",  "0=All, 1=onZ, 2=sideband (med pt)",  5, 0, 5);
  //h.chargeflip[2] = new TH1F("chargeflip_rate_highpt", "0=All, 1=onZ, 2=sideband (high pt)", 5, 0, 5);
  //for(int i=0; i<3; i++) h.chargeflip[i]->Sumw2();
  
  //############################################
  //Object level plots: (before event selection)
  //############################################
  //Electrons:
  
  h.ele[0] = new TH1F("nElectrons",      "nElectrons",10,0,10);
  h.ele[1] = new TH1F("allele_pt",       "pT (all electrons)",500,0,500);
  h.ele[2] = new TH1F("allele_eta",      "eta (all electrons)",200,-4,4);
  h.ele[3] = new TH1F("allele_phi",      "phi (all electrons)",200,-4,4);
  h.ele[4] = new TH1F("allele_reliso03", "reliso03 (all electrons)",100,0,0.2);
  for(int i=0; i<5; i++)h.ele[i]->Sumw2();
  //Muons:
  h.mu[0] = new TH1F("nMuons",         "nMuons",10,0,10);
  h.mu[1] = new TH1F("allmu_pt",       "pT (all muons)",500,0,500);
  h.mu[2] = new TH1F("allmu_eta",      "eta (all muons)",200,-4,4);
  h.mu[3] = new TH1F("allmu_phi",      "phi (all muons)",200,-4,4);
  h.mu[4] = new TH1F("allmu_reliso03", "reliso03 (all muons)",100,0,0.2);
  for(int i=0; i<5; i++)h.mu[i]->Sumw2();
  /*
  //Photons:
  h.pho[0] = new TH1F("nPhotons","nPhotons",10,0,10);
  h.pho[1] = new TH1F("allpho_pt","pt",500,0,500);
  h.pho[2] = new TH1F("allpho_eta","eta",200,-4,4);
  h.pho[3] = new TH1F("allpho_phi","phi",200,-4,4);
  h.pho[4] = new TH1F("allpho_reliso03","reliso03",200,0,1);
  for(int i=0; i<5; i++)h.pho[i]->Sumw2();
  //Taus:
  h.tau[0] = new TH1F("nTaus","nTaus",10,0,10);
  h.tau[1] = new TH1F("alltau_pt","alltau_pt",500,0,500);
  h.tau[2] = new TH1F("alltau_eta","alltau_eta",200,-4,4);
  h.tau[3] = new TH1F("alltau_phi","alltau_phi",200,-4,4);
  for(int i=0; i<4; i++)h.tau[i]->Sumw2();*/
  //Jets:
  h.jet[0] = new TH1F("nJets",      "nJets",10,0,10);
  h.jet[1] = new TH1F("alljet_pt",  "pT (all Jets)",500,0,500);
  h.jet[2] = new TH1F("alljet_eta", "eta (all Jets)",200,-4,4);
  h.jet[3] = new TH1F("alljet_phi", "phi (all Jets)",200,-4,4);
  for(int i=0; i<4; i++)h.jet[i]->Sumw2();
  //bJets:
  h.bjet[0] = new TH1F("nbJets",      "nbJets",10,0,10);
  h.bjet[1] = new TH1F("allbjet_pt",  "pT (all bJets)",500,0,500);
  h.bjet[2] = new TH1F("allbjet_eta", "eta (all bJets)",200,-4,4);
  h.bjet[3] = new TH1F("allbjet_phi", "phi (all bJets)",200,-4,4);
  for(int i=0; i<4; i++)h.bjet[i]->Sumw2();

  //--------------------------------------------------------------------------------
  //Basic kinematics study:
  h.basic[0] = new TH1F("ele0_pt", "pT (ele0)", 500, 0, 1000);
  h.basic[1] = new TH1F("ele1_pt", "pT (ele1)", 500, 0, 1000);
  h.basic[2] = new TH1F("ele2_pt", "pT (ele2)", 500, 0, 1000);
  h.basic[3] = new TH1F("mu0_pt", "pT (mu0)",   500, 0, 1000);
  h.basic[4] = new TH1F("mu1_pt", "pT (mu1)",   500, 0, 1000);
  h.basic[5] = new TH1F("mu2_pt", "pT (mu2)",   500, 0, 1000);
  h.basic[6] = new TH1F("jet0_pt", "pT (jet0)", 500, 0, 1000);
  h.basic[7] = new TH1F("jet1_pt", "pT (jet1)", 500, 0, 1000);
  h.basic[8] = new TH1F("jet2_pt", "pT (jet2)", 500, 0, 1000);
  h.basic[9] = new TH1F("bjet0_pt", "pT (bjet0)",  500, 0, 1000);
  h.basic[10] = new TH1F("bjet1_pt", "pT (bjet1)", 500, 0, 1000);
  h.basic[11] = new TH1F("bjet2_pt", "pT (bjet2)", 500, 0, 1000);

  h.basic[12] = new TH1F("ele0_mt", "mT (ele0)", 500, 0, 1000);
  h.basic[13] = new TH1F("ele1_mt", "mT (ele1)", 500, 0, 1000);
  h.basic[14] = new TH1F("ele2_mt", "mT (ele2)", 500, 0, 1000);
  h.basic[15] = new TH1F("mu0_mt", "mT (mu0)",   500, 0, 1000);
  h.basic[16] = new TH1F("mu1_mt", "mT (mu1)",   500, 0, 1000);
  h.basic[17] = new TH1F("mu2_mt", "mT (mu2)",   500, 0, 1000);
  h.basic[18] = new TH1F("jet0_mt", "mT (jet0)", 500, 0, 1000);
  h.basic[19] = new TH1F("jet1_mt", "mT (jet1)", 500, 0, 1000);
  h.basic[20] = new TH1F("jet2_mt", "mT (jet2)", 500, 0, 1000);

  h.basic[21] = new TH1F("diele_pt",  "pT (diele)",  500, 0, 1000);
  h.basic[22] = new TH1F("diele_eta", "eta (diele)", 200, -4, 4);
  h.basic[23] = new TH1F("diele_phi", "phi (diele)", 200, -4, 4);
  h.basic[24] = new TH1F("diele_m",   "M (diele)",   600, 0, 1200);
  h.basic[25] = new TH1F("diele_mt",  "mT (diele)",  500, 0, 1000);

  h.basic[26] = new TH1F("dimu_pt",  "pT (dimu)",  500, 0, 1000);
  h.basic[27] = new TH1F("dimu_eta", "eta (dimu)", 200, -4, 4);
  h.basic[28] = new TH1F("dimu_phi", "phi (dimu)", 200, -4, 4);
  h.basic[29] = new TH1F("dimu_m",   "M (dimu)",   600, 0, 1200);
  h.basic[30] = new TH1F("dimu_mt",  "mT (dimu)",  500, 0, 1000);

  h.basic[31] = new TH1F("dijet_pt",  "pT (dijet)",  500, 0, 1000);
  h.basic[32] = new TH1F("dijet_eta", "eta (dijet)", 200, -4, 4);
  h.basic[33] = new TH1F("dijet_phi", "phi (dijet)", 200, -4, 4);
  h.basic[34] = new TH1F("dijet_m",   "M (dijet)",   600, 0, 1200);
  h.basic[35] = new TH1F("dijet_mt",  "mT (dijet)",  500, 0, 1000);

  h.basic[36] = new TH1F("dphi_ele0_ele1", "dphi(ele0, ele1)", 200, 0, 6);
  h.basic[37] = new TH1F("dphi_mu0_mu1",   "dphi(mu0, mu1)",   200, 0, 6);
  h.basic[38] = new TH1F("dphi_jet0_jet1", "dphi(jet0, jet1)", 200, 0, 6);

  h.basic[39] = new TH1F("dr_ele0_ele1", "dR(ele0, ele1)", 200, 0, 6);
  h.basic[40] = new TH1F("dr_mu0_mu1",   "dR(mu0, mu1)",   200, 0, 6);
  h.basic[41] = new TH1F("dr_jet0_jet1", "dR(jet0, jet1)", 200, 0, 6);

  h.basic[42] = new TH1F("met_pt",  "pTmis",     500, 0, 1000);
  h.basic[43] = new TH1F("met_phi", "phi (MET)", 200, -4, 4);
  
  for(int i=0; i<44; i++)h.basic[i]->Sumw2();

  //--------------------------------------------------------------------------------
  //SignalStudy:
  h.sig[0] = new TH1F("genPart_pdgId_all", "genPart_pdgId_all", 2000, -1000, 1000);
  h.vll[0] = new TH1F("nvll",         "nVLL",          10, 0, 10);
  h.vll[1] = new TH1F("allvll_pt",    "pT (VLL)",   1000,0,1000);
  h.vll[2] = new TH1F("allvll_eta",   "eta (VLL)",     200,-4,4);
  h.vll[3] = new TH1F("allvll_phi",   "phi (VLL)",     200,-4,4);
  h.vll[4] = new TH1F("allvll_mass",  "mass (VLL)", 2500,0,2500);
  h.vll[5] = new TH1F("allvll_pdgid", "pdgid (VLL)",    40,-20,20);
  h.vll[6] = new TH1F("allvll_decay", "decay modes (VLL)",    5, 0, 5);
  h.vll[7] = new TH1F("allvll_daughers", "daughers (VLL)", 100, -50, 50);
  
  h.vln[0] = new TH1F("nvlnu",         "nVLNu",         10, 0, 10);
  h.vln[1] = new TH1F("allvlnu_pt",    "pT (VLNu)",   1000,0,1000);
  h.vln[2] = new TH1F("allvlnu_eta",   "eta (VLNu)",     200,-4,4);
  h.vln[3] = new TH1F("allvlnu_phi",   "phi (VLNu)",     200,-4,4);
  h.vln[4] = new TH1F("allvlnu_mass",  "mass (VLNu)", 2500,0,2500);
  h.vln[5] = new TH1F("allvlnu_pdgid", "pdgid (VLNu)",    20,-20,20);
  h.vln[6] = new TH1F("allvlnu_decay", "decaymodes (VLNu)",    5, 0, 5);
  h.vln[7] = new TH1F("allvlnu_daughers", "daughers (VLNu)", 100, -50, 50);

  // VLL pair production final states:
  h.sig[1] = new TH1F("finalstates_for_LL", "finalstates for LL", 10, 0, 10);
  h.sig[2] = new TH1F("finalstates_for_LN", "finalstates for LN", 10, 0, 10);
  h.sig[3] = new TH1F("finalstates_for_NN", "finalstates for NN", 10, 0, 10);
  h.sig[4] = new TH1F("finalstates_incldecay", "finalstates for inclusive decaymode", 10, 0, 10);
  h.sig[5] = new TH1F("acceptance_study", "acceptance: all, 1L2J, 2LOS, 2LSS, 3L, 4L, mujj, SR ", 10, 0, 10);
  h.sig[6] = new TH1F("bjet_acceptance_study", "acceptance: all, 1L2J, 2LOS, 2LSS, 3L, 4L, mujj, SR ", 10, 0, 10);

  //##############
  // Final states:
  //##############
  
  h.evt2LSS[0]  = new TH1F("nlep", "nlep", 10, 0, 10);
  h.evt2LSS[1]  = new TH1F("njet", "njet", 10, 0, 10);
  h.evt2LSS[2]  = new TH1F("nbjet", "nbjet", 10, 0, 10);

  h.evt2LSS[3]  = new TH1F("lep0_pt",  "lep0_pt",  50, 0, 500);
  h.evt2LSS[4]  = new TH1F("lep0_eta", "lep0_eta", 100, -4, 4);
  h.evt2LSS[5]  = new TH1F("lep0_phi", "lep0_phi", 100, -4, 4);
  h.evt2LSS[6]  = new TH1F("lep0_iso", "lep0_iso", 100, 0, 0.2);
  h.evt2LSS[7]  = new TH1F("lep0_sip3d", "lep0_sip3d", 500, 0, 50);
  h.evt2LSS[8]  = new TH1F("lep0_mt",  "lep0_mt",  50, 0, 500);
  h.evt2LSS[9] = new TH1F("lep0_deepjet", "lep0_deepjet", 300, -1, 2);
  
  h.evt2LSS[10]  = new TH1F("lep1_pt",  "lep1_pt",  50, 0, 500);
  h.evt2LSS[11]  = new TH1F("lep1_eta", "lep1_eta", 100, -4, 4);
  h.evt2LSS[12] = new TH1F("lep1_phi", "lep1_phi", 100, -4, 4);
  h.evt2LSS[13] = new TH1F("lep1_iso", "lep1_iso", 100, 0, 0.2);
  h.evt2LSS[14]  = new TH1F("lep1_sip3d", "lep1_sip3d", 500, 0, 50);
  h.evt2LSS[15] = new TH1F("lep1_mt",  "lep1_mt",  50, 0, 500);
  h.evt2LSS[16] = new TH1F("lep1_deepjet", "lep1_deepjet", 300, -1, 2);
  
  h.evt2LSS[17] = new TH1F("dilep_pt",  "dilep_pt",  50, 0, 500);
  h.evt2LSS[18] = new TH1F("dilep_eta", "dilep_eta", 100, -10, 10);
  h.evt2LSS[19] = new TH1F("dilep_phi", "dilep_phi", 100, -4, 4);
  h.evt2LSS[20] = new TH1F("dilep_mass","dilep_mass",50, 0, 500);
  h.evt2LSS[21] = new TH1F("dilep_mt",  "dilep_mt",  50, 0, 500);
  h.evt2LSS[22] = new TH1F("dilep_deta", "dilep_deta", 100, 0, 6);
  h.evt2LSS[23] = new TH1F("dilep_dphi", "dilep_dphi", 100, 0, 6);
  h.evt2LSS[24] = new TH1F("dilep_dR",   "dilep_dR",   100, 0, 6);
  h.evt2LSS[25] = new TH1F("dilep_ptratio","dilep_ptratio",100, 0, 1);

  h.evt2LSS[26] = new TH1F("HT", "HT", 20, 0, 500);
  h.evt2LSS[27] = new TH1F("LT", "LT", 20, 0, 500);
  h.evt2LSS[28] = new TH1F("STvis", "STvis", 50, 0, 500);
  h.evt2LSS[29] = new TH1F("ST", "ST", 50, 0, 500);
  h.evt2LSS[30] = new TH1F("STfrac", "STfrac", 100, 0, 1.1); //For nJet-==0, STfrac=1
  h.evt2LSS[31] = new TH1F("HTMETllpt", "HTMETllpt", 50, 0, 500);
  h.evt2LSS[32] = new TH1F("metpt", "MET", 50, 0, 500);
  h.evt2LSS[33] = new TH1F("metphi", "MET_phi", 100, -4, 4);
  
  h.evt2LSS[34] = new TH1F("dphi_metlep0",    "dilep_metlep0",    100, 0, 4);
  h.evt2LSS[35] = new TH1F("dphi_metlep1",    "dilep_metlep1",    100, 0, 4);
  h.evt2LSS[36] = new TH1F("dphi_metdilep",   "dilep_metdilep",   100, 0, 4);
  h.evt2LSS[37] = new TH1F("dphi_metlep_max", "dilep_metlep_max", 100, 0, 4);
  h.evt2LSS[38] = new TH1F("dphi_metlep_min", "dilep_metlep_min", 100, 0, 4);

  h.evt2LSS[39] = new TH1F("2LSS_wt_leptonSF",  "Reco-ID-Iso Scale Factor",200,0,2);
  h.evt2LSS[40] = new TH1F("2LSS_wt_trig","Trigger Scale Factor",200,0,2);
  h.evt2LSS[41] = new TH1F("2LSS_wt_bjet","bJet weight",200,0,2);
  h.evt2LSS[42] = new TH1F("2LSS_wt_evt", "Event weight",200,0,2);
  
  for(int i=0; i<43; i++) h.evt2LSS[i]->Sumw2();

  h.scalef[0] = new TH2F("2LSS_lep0sfvPt", "lep0 SF v pT", 100, 0, 2, 500, 0, 500);
  h.scalef[1] = new TH2F("2LSS_lep1sfvPt", "lep1 SF v pT", 100, 0, 2, 500, 0, 500);
  h.scalef[2] = new TH2F("2LSS_lep0effvPt", "lep0 TrigEff v pT", 100, 0, 2, 500, 0, 500);
  h.scalef[3] = new TH2F("2LSS_lep1effvPt", "lep1 TrigEff v pT", 100, 0, 2, 500, 0, 500);
  for(int i=0; i<4; i++) h.scalef[i]->Sumw2();

  //Lepton flavor check
  h.flav[0] = new TH1F("all_lep_flav_abs", "abs(lep flav) (all)", 20, 0, 20);
  h.flav[1] = new TH1F("lep0_flav_abs", "abs(lep0) flav", 20, 0, 20);
  h.flav[2] = new TH1F("lep1_flav_abs", "abs(lep1) flav", 20, 0, 20);
  h.flav[3] = new TH1F("all_lep_flav_raw", "lep flav (all)", 40, -20, 20);
  h.flav[4] = new TH1F("lep0_flav_raw", "lep0 flav", 40, -20, 20);
  h.flav[5] = new TH1F("lep1_flav_raw", "lep1 flav", 40, -20, 20);
  for(int i=0; i<6; i++) h.flav[i]->Sumw2();

  //Weights check:
  h.evtweight[8]  = new TH1F("SS_jec",     "Jet Energy Correction for each Jet",200,0,2);
  h.evtweight[9]  = new TH1F("SS_jer",     "Jet Energy Resolution Correction for each Jet",200,0,2);
  h.evtweight[10] = new TH1F("SS_jecjer",  "JEC*JER for each Jet",200,0,2);
  h.evtweight[11] = new TH1F("SS_SFwt",    "Reco-ID-Iso Scale Factor",200,0,2);
  h.evtweight[12] = new TH1F("SS_trigwt",  "Trigger Scale Factor",200,0,2);
  h.evtweight[13] = new TH1F("SS_pileupwt","pileup weight",200,0,2);
  h.evtweight[14] = new TH1F("SS_bjetwt",  "bJet weight",200,0,2);
  h.evtweight[15] = new TH1F("SS_evtwt",   "Event weight",200,0,2);

  //Trigger check:
  h.flag[0] = new TH1F("SS_HLT_IsoMu24",           "HLT_IsoMu24", 5, 0, 5);
  h.flag[1] = new TH1F("SS_HLT_IsoMu27",           "HLT_IsoMu27", 5, 0, 5);
  h.flag[2] = new TH1F("SS_HLT_Ele27_WPTight_Gsf", "HLT_Ele27_WPTight_Gsf", 5, 0, 5);
  h.flag[3] = new TH1F("SS_HLT_Ele32_WPTight_Gsf", "HLT_Ele32_WPTight_Gsf", 5, 0, 5);
  h.flag[4] = new TH1F("SS_overlap",               "0:all, 1:mu, 2:ele, 3:overlap", 5, 0, 5);
  for(int i=0; i<5; i++) h.flag[i]->Sumw2();

  //Charge misID calculation:
  vector<float> ZpTbins = {0, 10, 20, 30, 50, 75, 100, 200, 350, 500, 1000};
  h.chargeflip[0] = new TH1F("chargeflip_den", "chargeflip_den", ZpTbins.size()-1, &ZpTbins[0]);
  h.chargeflip[1] = new TH1F("chargeflip_num", "chargeflip_num", ZpTbins.size()-1, &ZpTbins[0]);
  for(int i=0; i<2; i++) h.chargeflip[i]->Sumw2();
  
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
  
  for(int i=0; i<19; i++) h.gen2LSS[i]->Sumw2();*/

  /*
  h.btagsf[0] = new TH1F("JetHadronFlavor_all", "JetHadronFlavor_all", 10, -2, 8);
  //2D plots for bTagSF calculation:
  float x_bin_pT[10] = {20,30,50,70,100,140,200,300,600,1000};
  float y_bin_eta[4] = {0.0,0.8,1.6,3.0};
  h.bJets[0] = new TH2F("bJet_PtEta","bJet_PtEta",(sizeof(x_bin_pT)/ sizeof(x_bin_pT[0])-1),x_bin_pT,(sizeof(y_bin_eta)/sizeof(y_bin_eta[0])-1),y_bin_eta);
  h.bJets[1] = new TH2F("bJet_MedWP_PtEta","bJet_MedWP_PtEta",(sizeof(x_bin_pT)/ sizeof(x_bin_pT[0])-1),x_bin_pT,(sizeof(y_bin_eta)/sizeof(y_bin_eta[0])-1),y_bin_eta);
  h.cJets[0] = new TH2F("cJet_PtEta","cJet_PtEta",(sizeof(x_bin_pT)/ sizeof(x_bin_pT[0])-1),x_bin_pT,(sizeof(y_bin_eta)/sizeof(y_bin_eta[0])-1),y_bin_eta);
  h.cJets[1] = new TH2F("cJet_Mis_PtEta","cJet_Mis_PtEta",(sizeof(x_bin_pT)/ sizeof(x_bin_pT[0])-1),x_bin_pT,(sizeof(y_bin_eta)/sizeof(y_bin_eta[0])-1),y_bin_eta);
  h.lJets[0] = new TH2F("LightJet_PtEta","LightJet_PtEta",(sizeof(x_bin_pT)/ sizeof(x_bin_pT[0])-1),x_bin_pT,(sizeof(y_bin_eta)/sizeof(y_bin_eta[0])-1),y_bin_eta);
  h.lJets[1] = new TH2F("LightJet_Mis_PtEta","LightJet_Mis_PtEta",(sizeof(x_bin_pT)/ sizeof(x_bin_pT[0])-1),x_bin_pT,(sizeof(y_bin_eta)/sizeof(y_bin_eta[0])-1),y_bin_eta);
  */
  
  //Investigating bjets:
  //1D plots: (btv_deepjet.h)
  h.evtweight[15] = new TH1F("sfPOG_bcjet", "bcJet sf from POG",200,0,2);
  h.evtweight[16] = new TH1F("sfPOG_ljet",  "lJet sf from POG", 200,0,2);
  h.evtweight[17] = new TH1F("jeteff_btag", "bJet tagging efficiency",200,0,2);
  h.evtweight[18] = new TH1F("jeteff_cmis", "cJet mistagging efficiency",200,0,2);
  h.evtweight[19] = new TH1F("jeteff_lmis", "lJet mistagging efficiency",200,0,2);
  h.evtweight[20] = new TH1F("jeteff_all",  "Jet efficiency (all)",200,0,2);
  for(int i=0; i<21; i++) h.evtweight[i]->Sumw2();
  
}
