void AnaScript::BookHistograms(){
  h.nevt = new TH1F("nEvents","0-Total events, 1-Total events ran, 2-Total events with trigger applied",5,-1,4);

  //############################################
  //Object level plots: (before event selection)
  //############################################
  //Electrons:
  h.ele[0] = new TH1F("nElectrons","nElectrons",10,0,10);
  h.ele[1] = new TH1F("allele_pt","allele_pt",500,0,500);
  h.ele[2] = new TH1F("allele_eta","allele_eta",200,-4,4);
  h.ele[3] = new TH1F("allele_phi","allele_phi",200,-4,4);
  //Muons:
  h.mu[0] = new TH1F("nMuons","nMuons",10,0,10);
  h.mu[1] = new TH1F("allmu_pt","allmu_pt",500,0,500);
  h.mu[2] = new TH1F("allmu_eta","allmu_eta",200,-4,4);
  h.mu[3] = new TH1F("allmu_phi","allmu_phi",200,-4,4);
  h.mu[4] = new TH1F("allmu_reliso04","allmu_reliso04",200,0,1);
  //Photons:
  h.pho[0] = new TH1F("nPhotons","nPhotons",10,0,10);
  h.pho[1] = new TH1F("allpho_pt","allpho_pt",500,0,500);
  h.pho[2] = new TH1F("allpho_eta","allpho_eta",200,-4,4);
  h.pho[3] = new TH1F("allpho_phi","allpho_phi",200,-4,4);
  h.pho[4] = new TH1F("allpho_reliso04","allpho_reliso04",200,0,1);
  //Taus:
  h.tau[0] = new TH1F("nTaus","nTaus",10,0,10);
  h.tau[1] = new TH1F("alltau_pt","alltau_pt",500,0,500);
  h.tau[2] = new TH1F("alltau_eta","alltau_eta",200,-4,4);
  h.tau[3] = new TH1F("alltau_phi","alltau_phi",200,-4,4);
  //Jets:
  h.jet[0] = new TH1F("nJets","nJets",10,0,10);
  h.jet[1] = new TH1F("alljet_pt","alljet_pt",500,0,500);
  h.jet[2] = new TH1F("alljet_eta","alljet_eta",200,-4,4);
  h.jet[3] = new TH1F("alljet_phi","alljet_phi",200,-4,4);
  //bJets:
  h.bjet[0] = new TH1F("nbJets","nbJets",10,0,10);
  h.bjet[1] = new TH1F("allbjet_pt","allbjet_pt",500,0,500);
  h.bjet[2] = new TH1F("allbjet_eta","allbjet_eta",200,-4,4);
  h.bjet[3] = new TH1F("allbjet_phi","allbjet_phi",200,-4,4);

  
}
