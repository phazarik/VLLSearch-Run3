void AnaScript::FillTree(TTree *tree){
  
  // The following is a list of event-level variables for 2LSS which are being filled in the tree.
  // This script is dependant on the final state. It may be segmentation violation, if the wrong final state is chosen.

  // Making sure that it's a 2LSS (exclusive) event:
  bool right_events = false;
  if((int)LightLepton.size()==2 && (LightLepton.at(0).charge == LightLepton.at(1).charge)) right_events = true;

  if(right_events){

    nEvtPass++;
    //h.nevt->Fill(3);
    
    //Declaring the variables:
    UInt_t nlep;  //[0,20<,UInt5_t>]
    UInt_t njet;  //[0,20<,UInt5_t>]
    UInt_t nbjet;  //[0,20<,UInt5_t>]
    Float_t lep0_pt;  //[0,1500<,Float32_t>]
    Float_t lep0_eta;  //[-6,6<,Float32_t>]
    Float_t lep0_phi;  //[-6,6<,Float32_t>]
    Float_t lep0_mt;  //[0,1500<,Float32_t>]
    Float_t lep1_pt;  //[0,1500<,Float32_t>]
    Float_t lep1_eta;  //[-6,6<,Float32_t>]
    Float_t lep1_phi;  //[-6,6<,Float32_t>]
    Float_t lep1_mt;  //[0,1500<,Float32_t>]
    Float_t ll_pt;  //[0,1500<,Float32_t>]
    Float_t ll_mass;  //[0,1500<,Float32_t>]
    Float_t ll_deta;  //[0,6<,Float32_t>]
    Float_t ll_dphi;  //[0,6<,Float32_t>]
    Float_t ll_dR;  //[0,6<,Float32_t>]
    Float_t ptratio;  //[0,1<,Float32_t>]
    Float_t HT;  //[0,1500<,Float32_t>]
    Float_t ST;  //[0,1500<,Float32_t>]
    Float_t STfrac;  //[0,1<,Float32_t>]
    Float_t dphi_metlep0;  //[0,6<,Float32_t>]
    Float_t dphi_metlep1;  //[0,6<,Float32_t>]
    Float_t dphi_metlep_max;  //[0,6<,Float32_t>]
    Float_t dphi_metlep_min;  //[0,6<,Float32_t>]  

    //Setting values to them:
    
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

    //tree->Branch("nlep",     &nlep);

    //########################
    //Setting up the branches:
    AddAndCompressBranch( tree->Branch("nlep",     &nlep));
    AddAndCompressBranch( tree->Branch("njet",     &njet));
    AddAndCompressBranch( tree->Branch("nbjet",    &nbjet));
    AddAndCompressBranch( tree->Branch("metpt",    &metpt));
    AddAndCompressBranch( tree->Branch("metphi",   &metphi));
    AddAndCompressBranch( tree->Branch("lep0_pt",  &lep0_pt));
    AddAndCompressBranch( tree->Branch("lep0_eta", &lep0_eta));
    AddAndCompressBranch( tree->Branch("lep0_phi", &lep0_phi));
    AddAndCompressBranch( tree->Branch("lep0_mt",  &lep0_mt));
    AddAndCompressBranch( tree->Branch("lep1_pt",  &lep1_pt));
    AddAndCompressBranch( tree->Branch("lep1_eta", &lep1_eta));
    AddAndCompressBranch( tree->Branch("lep1_phi", &lep1_phi));
    AddAndCompressBranch( tree->Branch("lep1_mt",  &lep1_mt));
    AddAndCompressBranch( tree->Branch("ll_pt",    &ll_pt));
    AddAndCompressBranch( tree->Branch("ll_mass",  &ll_mass));
    AddAndCompressBranch( tree->Branch("ll_deta",  &ll_deta));
    AddAndCompressBranch( tree->Branch("ll_dphi",  &ll_dphi));
    AddAndCompressBranch( tree->Branch("ll_dR",    &ll_dR));
    AddAndCompressBranch( tree->Branch("HT",       &HT));
    AddAndCompressBranch( tree->Branch("ST",       &ST));
    AddAndCompressBranch( tree->Branch("STfrac",   &STfrac));
    AddAndCompressBranch( tree->Branch("dphi_metlep0",      &dphi_metlep0));
    AddAndCompressBranch( tree->Branch("dphi_metlep1",      &dphi_metlep1));
    AddAndCompressBranch( tree->Branch("dphi_metlep_max",   &dphi_metlep_max));
    AddAndCompressBranch( tree->Branch("dphi_metlep_min",   &dphi_metlep_min));
    
  }
  //tree->Fill();
}

//The following funtion is called to apply settings to all branches.
void AnaScript::AddAndCompressBranch(TBranch *br){
  br->SetBasketSize(17920);
  br->SetCompressionAlgorithm(2);
  br->SetCompressionLevel(9);
  br->SetCompressionSettings(209);
  //br->SetAutoDelete(true);
  
}
