void AnaScript::FillTree(TTree *tree){
  
  // The following is a list of event-level variables for 2LSS which are being filled in the tree.
 // This script is dependant on the final state. It may be segmentation violation, if the wrong final state is chosen.
  
  int nlep = (int)LightLepton.size();
  float lep0_pt = LightLepton.at(0).v.Pt();

  //Filling up the braches:
  tree->Branch("nlep",    &nlep);
  tree->Branch("lep0_pt", &lep0_pt);

  tree->Fill();
}
