void AnaScript::InitializeBranches(TTree *tree){

  //Counts:
  tree->Branch("nlep",      &nlep,      "nlep/I",      32000);
  tree->Branch("njet",      &njet,      "njet/I",      32000);
  tree->Branch("nbjet",     &nbjet,     "nbjet/I",     32000);

  //Object-level (first lepton)
  tree->Branch("lep0_pt",   &lep0_pt,   "lep0_pt/F",   32000);
  tree->Branch("lep0_eta",  &lep0_eta,  "lep0_eta/F",  32000);
  tree->Branch("lep0_phi",  &lep0_phi,  "lep0_phi/F",  32000);
  tree->Branch("lep0_iso",  &lep0_iso,  "lep0_iso/F",  32000);
  tree->Branch("lep0_sip3d",&lep0_sip3d,"lep0_sip3d/F",32000);
  tree->Branch("lep0_mt",   &lep0_mt,   "lep0_mt/F",   32000);
  
  //Object-level (second lepton)
  tree->Branch("lep1_pt",   &lep1_pt,   "lep1_pt/F",   32000);
  tree->Branch("lep1_eta",  &lep1_eta,  "lep1_eta/F",  32000);
  tree->Branch("lep1_phi",  &lep1_phi,  "lep1_phi/F",  32000);
  tree->Branch("lep1_iso",  &lep1_iso,  "lep1_iso/F",  32000);
  tree->Branch("lep1_mt",   &lep1_mt,   "lep1_mt/F",   32000);

  //Dilepton system:
  tree->Branch("dilep_pt",  &dilep_pt,  "dilep_pt/F",  32000);
  tree->Branch("dilep_eta", &dilep_eta, "dilep_eta/F", 32000);
  tree->Branch("dilep_phi", &dilep_phi, "dilep_phi/F", 32000);
  tree->Branch("dilep_mass",&dilep_mass,"dilep_mass/F",32000);
  tree->Branch("dilep_mt",  &dilep_mt,  "dilep_mt/F",  32000);
  tree->Branch("dilep_deta",   &dilep_deta,   "dilep_deta/F",   32000);
  tree->Branch("dilep_dphi",   &dilep_dphi,   "dilep_dphi/F",   32000);
  tree->Branch("dilep_dR",     &dilep_dR,     "dilep_dR/F",     32000);
  tree->Branch("dilep_ptratio",&dilep_ptratio,"dilep_ptratio/F",32000);

  //Event level:
  tree->Branch("HT",        &HT,        "HT/F",        32000);
  tree->Branch("LT",        &LT,        "LT/F",        32000);
  tree->Branch("STvis",     &STvis,     "STvis/F",     32000);
  tree->Branch("ST",        &ST,        "ST/F",        32000);
  tree->Branch("HTMETllpt", &HTMETllpt, "HTMETllpt/F", 32000);
  tree->Branch("STfrac",    &STfrac,    "STfrac/F",    32000);
  tree->Branch("metpt",     &metpt_tree,     "metpt/F",     32000);
  tree->Branch("metphi",    &metphi_tree,    "metphi/F",    32000);

  tree->Branch("dphi_metlep0",   &dphi_metlep0,   "dphi_metlep0/F",   32000);
  tree->Branch("dphi_metlep1",   &dphi_metlep1,   "dphi_metlep1/F",   32000);
  tree->Branch("dphi_metdilep",  &dphi_metdilep,  "dphi_metdilep/F",  32000);
  tree->Branch("dphi_metlep_max",&dphi_metlep_max,"dphi_metlep_max/F",32000);
  tree->Branch("dphi_metlep_min",&dphi_metlep_min,"dphi_metlep_min/F",32000);

  tree->Branch("wt",    &wt,    "wt/D",    32000);
}

void AnaScript::FillTree(TTree *tree){
  
  // This function sets values to the variables that goes into the tree.
  // Event selection must be done before calling this function.  
    
  //Integers:
  nlep  = (UInt_t)Muon.size();
  njet  = (UInt_t)Jet.size();
  nbjet = (UInt_t)MediumbJet.size();
    
  //Object-level variables:
  lep0_pt  = (Float_t)Muon.at(0).v.Pt();
  lep0_eta = (Float_t)Muon.at(0).v.Eta();
  lep0_phi = (Float_t)Muon.at(0).v.Phi();
  lep0_iso = (Float_t)Muon.at(0).reliso03;
  lep0_mt  = (Float_t)transv_mass(Muon.at(0).v.E(), Muon.at(0).v.Phi(), metpt, metphi);
  lep1_pt  = (Float_t)Muon.at(1).v.Pt();
  lep1_eta = (Float_t)Muon.at(1).v.Eta();
  lep1_phi = (Float_t)Muon.at(1).v.Phi();
  lep1_iso = (Float_t)Muon.at(1).reliso03;
  lep1_mt  = (Float_t)transv_mass(Muon.at(1).v.E(), Muon.at(1).v.Phi(), metpt, metphi);

  //Dilepton system:
  TLorentzVector dilep = Muon.at(0).v + Muon.at(1).v;
  dilep_pt   = (Float_t)dilep.Pt();
  dilep_eta  = (Float_t)dilep.Eta();
  dilep_phi  = (Float_t)dilep.Phi();
  dilep_mass = (Float_t)dilep.M();
  dilep_mt   = (Float_t)transv_mass(dilep.E(), dilep.Phi(), metpt, metphi);
  dilep_deta    = (Float_t)fabs(Muon.at(0).v.Eta() - Muon.at(1).v.Eta());
  dilep_dphi    = (Float_t)delta_phi(Muon.at(0).v.Phi(), Muon.at(1).v.Phi());
  dilep_dR      = (Float_t)Muon.at(0).v.DeltaR(Muon.at(1).v);
  dilep_ptratio = (Float_t)lep1_pt/lep0_pt;

  //Event level:
  HT=0; for(Int_t i=0; i<(Int_t)Jet.size();  i++) HT = HT +  Jet.at(i).v.Pt();
  LT=0; for(Int_t i=0; i<(Int_t)Muon.size(); i++) LT = LT + Muon.at(i).v.Pt();
  STvis = HT + LT;
  ST = HT + LT + metpt;
  HTMETllpt = HT + metpt + dilep_pt;
  STfrac = 0; if(ST>0) STfrac = dilep_pt/ST;
  dphi_metlep0 = (Float_t)delta_phi(Muon.at(0).v.Phi(), metphi);
  dphi_metlep1 = (Float_t)delta_phi(Muon.at(1).v.Phi(), metphi);
  dphi_metdilep = (Float_t)delta_phi(dilep.Phi(), metphi);
  dphi_metlep_max = (Float_t)max(dphi_metlep0, dphi_metlep1);
  dphi_metlep_min = (Float_t)min(dphi_metlep0, dphi_metlep1);
  metpt_tree = metpt;
  metphi_tree = metphi;

}

//The following function is called to apply settings to all branches, if needed.
void AnaScript::AddAndCompressBranch(TBranch *br){
  br->SetBasketSize(17920);
  br->SetCompressionAlgorithm(2);
  br->SetCompressionLevel(9);
  br->SetCompressionSettings(209);
  //br->SetAutoDelete(true);
}


