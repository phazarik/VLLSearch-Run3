void AnaScript::InitializeBranches(TTree *tree){
  tree->Branch("nlep",     &nlep);
  tree->Branch("njet",     &njet);
  tree->Branch("nbjet",    &nbjet);
  tree->Branch("metpt",    &metpt);
  tree->Branch("metphi",   &metphi);
  tree->Branch("lep0_pt",  &lep0_pt);
  tree->Branch("lep0_eta", &lep0_eta);
  tree->Branch("lep0_phi", &lep0_phi);
  tree->Branch("lep0_mt",  &lep0_mt);
  tree->Branch("lep1_pt",  &lep1_pt);
  tree->Branch("lep1_eta", &lep1_eta);
  tree->Branch("lep1_phi", &lep1_phi);
  tree->Branch("lep1_mt",  &lep1_mt);
  tree->Branch("ll_pt",    &ll_pt);
  tree->Branch("ll_mass",  &ll_mass);
  tree->Branch("ll_deta",  &ll_deta);
  tree->Branch("ll_dphi",  &ll_dphi);
  tree->Branch("ll_dR",    &ll_dR);
  tree->Branch("HT",       &HT);
  tree->Branch("ST",       &ST);
  tree->Branch("STfrac",   &STfrac);
  tree->Branch("dphi_metlep0",      &dphi_metlep0);
  tree->Branch("dphi_metlep1",      &dphi_metlep1);
  tree->Branch("dphi_metlep_max",   &dphi_metlep_max);
  tree->Branch("dphi_metlep_min",   &dphi_metlep_min);
}

void AnaScript::FillTree(TTree *tree){
  
  // The following is a list of event-level variables for 2LSS which are being filled in the tree.
  // This script is dependant on the final state. It may be segmentation violation, if the wrong final state is chosen.

  // Making sure that it's a 2LSS (exclusive) event:
  bool right_events = false;
  if((int)LightLepton.size()==2 && (LightLepton.at(0).charge == LightLepton.at(1).charge)) right_events = true;

  if(right_events){

    nEvtPass++;
    //h.nevt->Fill(3);
    
    //Setting values to the variables that I want to my output file:
    
    //Integers:
    nlep  = (UInt_t)LightLepton.size();
    njet  = (UInt_t)Jet.size();
    nbjet = (UInt_t)bJet.size();
    
    //Object-level variables:
    lep0_pt  = (Float_t)LightLepton.at(0).v.Pt();
    lep0_eta = (Float_t)LightLepton.at(0).v.Eta();
    lep0_phi = (Float_t)LightLepton.at(0).v.Phi();
    lep0_mt  = (Float_t)transv_mass(LightLepton.at(0).v.E(), LightLepton.at(0).v.Phi(), metpt, metphi);
    lep1_pt  = (Float_t)LightLepton.at(1).v.Pt();
    lep1_eta = (Float_t)LightLepton.at(1).v.Eta();
    lep1_phi = (Float_t)LightLepton.at(1).v.Phi();
    lep1_mt  = (Float_t)transv_mass(LightLepton.at(1).v.E(), LightLepton.at(1).v.Phi(), metpt, metphi);

    //Dilepton system:
    TLorentzVector dilep = LightLepton.at(0).v + LightLepton.at(1).v;
    ll_pt   = (Float_t)dilep.Pt();
    ll_mass = (Float_t)dilep.M();
    ll_deta = (Float_t)fabs(LightLepton.at(0).v.Eta() - LightLepton.at(1).v.Eta());
    ll_dphi = (Float_t)delta_phi(LightLepton.at(0).v.Phi(), LightLepton.at(1).v.Phi());
    ll_dR   = (Float_t)LightLepton.at(0).v.DeltaR(LightLepton.at(1).v);
    ptratio = (Float_t)LightLepton.at(1).v.Pt() / LightLepton.at(0).v.Pt();

    //Event level:
    HT=0; for(Int_t i=0; i<(Int_t)Jet.size(); i++) HT = HT + Jet.at(i).v.Pt();
    ST = HT + ll_pt;
    STfrac = 0; if(ST>0) STfrac = ll_pt/ST;
    dphi_metlep0 = (Float_t)delta_phi(LightLepton.at(0).v.Phi(), metphi);
    dphi_metlep1 = (Float_t)delta_phi(LightLepton.at(1).v.Phi(), metphi);
    dphi_metlep_max = (Float_t)max(dphi_metlep0, dphi_metlep1);
    dphi_metlep_min = (Float_t)min(dphi_metlep0, dphi_metlep1);
  }
  //tree->Fill();
}

//The following function is called to apply settings to all branches, if needed.
void AnaScript::AddAndCompressBranch(TBranch *br){
  br->SetBasketSize(17920);
  br->SetCompressionAlgorithm(2);
  br->SetCompressionLevel(9);
  br->SetCompressionSettings(209);
  //br->SetAutoDelete(true);
}


