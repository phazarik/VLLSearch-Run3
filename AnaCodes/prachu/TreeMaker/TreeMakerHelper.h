void AnaScript::InitializeBranches(TTree *tree){
  tree->Branch("nlep",      &nlep,      "nlep/I",      32000);
  tree->Branch("njet",      &njet,      "njet/I",      32000);
  tree->Branch("nbjet",     &nbjet,     "nbjet/I",     32000);
  tree->Branch("metpt",     &metpt,     "metpt/F",     32000);
  tree->Branch("metphi",    &metphi,    "metphi/F",    32000);
  tree->Branch("lep0_pt",   &lep0_pt,   "lep0_pt/F",   32000);
  tree->Branch("lep0_eta",  &lep0_eta,  "lep0_eta/F",  32000);
  tree->Branch("lep0_phi",  &lep0_phi,  "lep0_phi/F",  32000);
  tree->Branch("lep0_mt",   &lep0_mt,   "lep0_mt/F",   32000);
  tree->Branch("lep1_pt",   &lep1_pt,   "lep1_pt/F",   32000);
  tree->Branch("lep1_eta",  &lep1_eta,  "lep1_eta/F",  32000);
  tree->Branch("lep1_phi",  &lep1_phi,  "lep1_phi/F",  32000);
  tree->Branch("lep1_mt",   &lep1_mt,   "lep1_mt/F",   32000);
  tree->Branch("dilep_pt",  &dilep_pt,  "dilep_pt/F",  32000);
  tree->Branch("dilep_eta", &dilep_eta, "dilep_eta/F", 32000);
  tree->Branch("dilep_phi", &dilep_phi, "dilep_phi/F", 32000);
  tree->Branch("dilep_mass",&dilep_mass,"dilep_mass/F",32000);
  tree->Branch("dilep_mt",  &dilep_mt,  "dilep_mt/F",  32000);
  tree->Branch("ll_deta",   &ll_deta,   "ll_deta/F",   32000);
  tree->Branch("ll_dphi",   &ll_dphi,   "ll_dphi/F",   32000);
  tree->Branch("ll_dR",     &ll_dR,     "ll_dR/F",     32000);
  tree->Branch("ll_ptratio",&ll_ptratio,"ll_ptratio/F",32000);
  tree->Branch("HT",        &HT,        "HT/F",        32000);
  tree->Branch("ST",        &ST,        "ST/F",        32000);
  tree->Branch("STfrac",    &STfrac,    "STfrac/F",    32000);
  tree->Branch("dphi_metlep0",   &dphi_metlep0,   "dphi_metlep0/F",   32000);
  tree->Branch("dphi_metlep1",   &dphi_metlep1,   "dphi_metlep1/F",   32000);
  tree->Branch("dphi_metlep_max",&dphi_metlep_max,"dphi_metlep_max/F",32000);
  tree->Branch("dphi_metlep_min",&dphi_metlep_min,"dphi_metlep_min/F",32000);
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
    dilep_pt   = (Float_t)dilep.Pt();
    dilep_eta  = (Float_t)dilep.Eta();
    dilep_phi  = (Float_t)dilep.Phi();
    dilep_mass = (Float_t)dilep.M();
    dilep_mt   = (Float_t)transv_mass(dilep.E(), dilep.Phi(), metpt, metphi);
    ll_deta    = (Float_t)fabs(LightLepton.at(0).v.Eta() - LightLepton.at(1).v.Eta());
    ll_dphi    = (Float_t)delta_phi(LightLepton.at(0).v.Phi(), LightLepton.at(1).v.Phi());
    ll_dR      = (Float_t)LightLepton.at(0).v.DeltaR(LightLepton.at(1).v);
    ll_ptratio = (Float_t)lep1_pt/lep0_pt;


    if(lep0_pt < lep1_pt) cout<<"Sorting error!"<<endl;

    //Event level:
    HT=0; for(Int_t i=0; i<(Int_t)Jet.size(); i++) HT = HT + Jet.at(i).v.Pt();
    ST = HT + dilep_pt;
    STfrac = 0; if(ST>0) STfrac = dilep_pt/ST;
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


