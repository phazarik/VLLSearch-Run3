void AnaScript::BookHistograms(){

  h.nevt = new TH1F("nEvents", "0-nEvtTotal, 1-nEvtRan, 2-nEvtTrigger, 3-nEvtPass",5,0,5); //h.nevt->Sumw2();
  h.evtweight[0] = new TH1F("wt_SF","Reco-ID-Iso Scale Factor",200,0,2);
  h.evtweight[1] = new TH1F("wt_trigger","Trigger Scale Factor",200,0,2);
  h.evtweight[2] = new TH1F("wt_evt","Event weight",200,0,2);
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
  
  //############################################
  //Object level plots: (before event selection)
  //############################################
  //Electrons:
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
  for(int i=0; i<4; i++)h.bjet[i]->Sumw2();

  //SignalStudy:
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
  h.vln[7] = new TH1F("allvlnu_daughers", "allvll_daughers", 100, -50, 50);

  // VLL pair production final states:
  h.sig[1] = new TH1F("finalstates_for_LL", "finalstates for LL", 10, 0, 10);
  h.sig[2] = new TH1F("finalstates_for_LN", "finalstates for LN", 10, 0, 10);
  h.sig[3] = new TH1F("finalstates_for_NN", "finalstates for NN", 10, 0, 10);

  //###############
  // Final states:
  //###############

}
