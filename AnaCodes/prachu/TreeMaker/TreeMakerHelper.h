void AnaScript::FillTree(TTree *tree){
  
  // The following is a list of event-level variables for 2LSS which are being filled in the tree.
  // This script is dependant on the final state. It may be segmentation violation, if the wrong final state is chosen.

  // Making sure that it's a 2LSS (exclusive) event:
  bool right_events = false;
  if((int)LightLepton.size()==2 && (LightLepton.at(0).charge == LightLepton.at(1).charge)) right_events = true;

  if(right_events){

    nEvtPass++;
    h.nevt->Fill(3);
    
    //Declaring the variables:
    //Integers:
    int nlep = (int)LightLepton.size();
    int njet = (int)Jet.size();
    int nbjet = (int)bJet.size();
    
    //Object-level variables:
    float lep0_pt  = LightLepton.at(0).v.Pt();
    float lep0_eta = LightLepton.at(0).v.Eta();
    float lep0_phi = LightLepton.at(0).v.Phi();
    float lep0_mt  = transv_mass(LightLepton.at(0).v.E(), LightLepton.at(0).v.Phi(), metpt, metphi);
    float lep1_pt  = LightLepton.at(1).v.Pt();
    float lep1_eta = LightLepton.at(1).v.Eta();
    float lep1_phi = LightLepton.at(1).v.Phi();
    float lep1_mt  = transv_mass(LightLepton.at(1).v.E(), LightLepton.at(1).v.Phi(), metpt, metphi);

    //Dilepton system:
    TLorentzVector dilep = LightLepton.at(0).v + LightLepton.at(1).v;
    float ll_pt   = dilep.Pt();
    float ll_mass = dilep.M();
    float ll_deta = fabs(LightLepton.at(0).v.Eta() - LightLepton.at(1).v.Eta());
    float ll_dphi = delta_phi(LightLepton.at(0).v.Phi(), LightLepton.at(1).v.Phi());
    float ll_dR   = LightLepton.at(0).v.DeltaR(LightLepton.at(1).v);
    float ptratio = LightLepton.at(1).v.Pt() / LightLepton.at(0).v.Pt();

    //Event level:
    float HT=0; for(int i=0; i<(int)Jet.size(); i++) HT = HT + Jet.at(i).v.Pt();
    float ST = HT + ll_pt;
    float STfrac = 0; if(ST>0) STfrac = ll_pt/ST;
    float dphi_metlep0 = delta_phi(LightLepton.at(0).v.Phi(), metphi);
    float dphi_metlep1 = delta_phi(LightLepton.at(1).v.Phi(), metphi);
    float dphi_metlep_max = max(dphi_metlep0, dphi_metlep1);
    float dphi_metlep_min = min(dphi_metlep0, dphi_metlep1);
  
    //Filling up the braches:
    tree->Branch("nlep",   &nlep);
    tree->Branch("njet",   &njet);
    tree->Branch("nbjet",  &nbjet);
    tree->Branch("met",    &metpt);
    tree->Branch("metphi", &metphi);
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
    tree->Branch("ll_dr",    &ll_dR);
    tree->Branch("ht",     &HT);
    tree->Branch("st",     &ST);
    tree->Branch("stfrac", &STfrac);
    tree->Branch("dphi_metlep0",    &dphi_metlep0);
    tree->Branch("dphi_metlep1",    &dphi_metlep1);
    tree->Branch("dphi_metlep_max", &dphi_metlep_max);
    tree->Branch("dphi_metlep_min", &dphi_metlep_min);

  }
  
  tree->Fill();
}
