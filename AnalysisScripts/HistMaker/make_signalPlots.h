void AnaScript::MakeSignalPlots(){

  //----------------------------------
  // Investigating GenPart collection
  //----------------------------------
  int test_event = 1;
  if(nEvtTotal == test_event){
    cout<<"\n------------------Event display-------------------"<<endl;
    cout<<"index\tpdgId\tname\tmomind\tmomid\tname\tstat"<<endl;
    for(unsigned int i=0; i< (unsigned int)(*nGenPart); i++){
      TString mom = "--";
      int momid = GenPart_genPartIdxMother[GenPart_pdgId[i]];
      if(fabs(momid)<10000) mom=(TString)to_string(momid);
      cout<<i<<"\t";
      cout<<GenPart_pdgId[i]<<"\t";
      cout<<ParticleName(GenPart_pdgId[i])<<"\t";
      cout<<GenPart_genPartIdxMother[i]<<"\t";
      cout<<mom<<"\t";
      cout<<ParticleName(momid)<<"\t";
      cout<<GenPart_status[i]<<endl;
    }
    cout<<"--------------------------------------------------\n"<<endl;
  }

  //----------------------------------
  // Creating VLL and VLNu collections
  //-----------------------------------
  
  vllep.clear();
  vlnu.clear();

  unsigned int iterator = (unsigned int)*nGenPart;
  for(unsigned int i=0; i<iterator; i++){
    Particle temp;
    temp.v.SetPtEtaPhiM(GenPart_pt[i],GenPart_eta[i],GenPart_phi[i],GenPart_mass[i]);
    temp.status = GenPart_status[i];
    temp.ind = i;
    temp.pdgid = GenPart_pdgId[i];
    //temp.momid= MotherID(i,GenPart_genPartIdxMother[i]);
    temp.momid = GenPart_pdgId[GenPart_genPartIdxMother[i]]; //Not fixing the mother right now

    bool mother_is_Z = (temp.momid == 23) || (temp.momid == 22); //or gamma*
    bool mother_is_W = (fabs(temp.momid) == 24);
    bool not_its_own_mother = (temp.momid != temp.pdgid); //To avoid overcounting

    //Finding daughters:
    vector<int> daughter_ind, daughter_id;
    daughter_ind.clear(); daughter_id.clear();

    //unsigned int iterator2 = (unsigned int)*(readerConfig->nGenPart);
    unsigned int iterator2 = (unsigned int)*nGenPart;
    for(unsigned int j=0; j<iterator2; j++){
      float id = GenPart_pdgId[j];
      //float momid = GenPart_pdgId[GenPart_genPartIdxMother[i]];
      float momid = MotherID(j,GenPart_genPartIdxMother[j]);
      //remove identical daughetrs:
      bool unique_daughter = true;
      if(momid == GenPart_pdgId[i]){ //If mother of the j-th particle is the i-th particle,
	for(int k=0; k<(int)daughter_id.size(); k++){
	  if(id == daughter_id.at(k)) unique_daughter = false;
	}
	if(unique_daughter){
	  daughter_ind.push_back(j); //Then the particle at j-th index is a daughter.
	  daughter_id.push_back(id);
	}
      }
    }
    temp.dauind = daughter_ind;
    temp.dauid = daughter_id;

    //Setting decaymodes:
    //If the particle decays into W, Z or H, decaymode = 1, 2 or 3 respectively.
    temp.decaymode = 0;
    for(int j=0; j<(int)temp.dauid.size(); j++){
      if(fabs(temp.dauid[j]) == 23 || fabs(temp.dauid[j]) == 22) temp.decaymode = 2; //Z/gamma*
      else if(fabs(temp.dauid[j]) == 24) temp.decaymode = 1; //W
      else if(fabs(temp.dauid[j]) == 25) temp.decaymode = 3; //H
    }   
    
    if(fabs(temp.pdgid) == 17 && not_its_own_mother){
      if(temp.pdgid == 17) temp.charge = -1;
      else if(temp.pdgid == -17) temp.charge = +1;
      vllep.push_back(temp);
    }

    else if(fabs(temp.pdgid) == 18 && not_its_own_mother){
      if(temp.pdgid == 18) temp.charge = -1;
      else if(temp.pdgid == -18) temp.charge = +1;
      //temp.charge = 0;
      vlnu.push_back(temp);
    }
  }

  SortVLL();

  //-------------------------------
  // Per event on-screen displays:
  //-------------------------------
  
  bad_event = false;
  bool todisplay = false;
  if(todisplay) cout<<"\n----- Event no. "<<nEvtTotal<<" -----"<<endl;
  
  //1. Counting VLLS:
  int nvll = (int)vllep.size();
  int nvlnu = (int)vlnu.size();
  if(todisplay) cout<<"nVLL = "<<nvll<<" , nVLNu = "<<nvlnu<<", total = "<<nvll+nvlnu<<endl;

  //2. Daughters:
  if(todisplay) cout<<"Daughters of VLL:  ";
  for(int i=0; i<(int)vllep.size(); i++){
    for(int j=0; j<(int)vllep.at(i).dauid.size(); j++){
      if(todisplay) cout << setw(4) << vllep.at(i).dauid[j] << ", ";
      if(fabs(vllep.at(i).dauid[j]) == 24)     bad_event = true;
      //The charged lepton cannot decay to a W,nu of the corresponding flavor (ele/mu):
    }
  }
  if(todisplay) cout<<endl<<"Daughters of VLNu: ";
  for(int i=0; i<(int)vlnu.size(); i++){
    for(int j=0; j<(int)vlnu.at(i).dauid.size(); j++){
      if(todisplay) cout << setw(4) << vlnu.at(i).dauid[j] << ", ";
      if(fabs(vlnu.at(i).dauid[j]) == 25)      bad_event = true;
      else if(fabs(vlnu.at(i).dauid[j]) == 23) bad_event = true;
      //The neutral lepton  cannot decay to H,nu or Z,nu.
    }
  }
  if(todisplay) {
    cout<<endl;
    if(bad_event) cout<<"\033[31mInvalid decay!\033[0m"<<endl;
    else cout<<"\033[93mValid decay.\033[0m"<<endl;
  }
  
  if(bad_event) return;
  
  //--------------------
  // Properties of VLLs
  //--------------------

  for(int i=0; i<(int)vllep.size(); i++){
    h.vllep[0]->Fill(vllep.at(i).v.Pt());
    h.vllep[1]->Fill(vllep.at(i).v.Eta());
    h.vllep[2]->Fill(vllep.at(i).v.Phi());
    h.vllep[3]->Fill(vllep.at(i).v.M());
    h.vllep[4]->Fill(vllep.at(i).pdgid);
    for(int j=0; j<(int)vllep[i].dauid.size(); j++) h.vllep[5]->Fill(vllep[i].dauid[j]);
  }

  for(int i=0; i<(int)vlnu.size(); i++){
    h.vlnu[0]->Fill(vlnu.at(i).v.Pt());
    h.vlnu[1]->Fill(vlnu.at(i).v.Eta());
    h.vlnu[2]->Fill(vlnu.at(i).v.Phi());
    h.vlnu[3]->Fill(vlnu.at(i).v.M());
    h.vlnu[4]->Fill(vlnu.at(i).pdgid);
    for(int j=0; j<(int)vlnu[i].dauid.size(); j++) h.vlnu[5]->Fill(vlnu[i].dauid[j]);
  }

  //------------------------------------------------------------------
  // Investigating production modes and decay modes:
  // prod mode: mode[0] entries = all, 0 = LL, 1 = LN, 2 = NN, -1 = invalid
  // decay mode: mode[1], mode[2], mode[3]; 0 = all, -1 = invalid
  //------------------------------------------------------------------
  
  //Production mode: LL
  if((int)vllep.size()==2){
    h.mode[0]->Fill(0);
    //Decaymodes: ZZ, ZH, HH
    int decayL0 = vllep.at(0).decaymode;
    int decayL1 = vllep.at(1).decaymode;
    if     (decayL0 == 2 && decayL1 == 2) h.mode[1]->Fill(0); //ZZ
    else if(decayL0 == 2 && decayL1 == 3) h.mode[1]->Fill(1); //ZH
    else if(decayL0 == 3 && decayL1 == 2) h.mode[1]->Fill(1); //HZ
    else if(decayL0 == 3 && decayL1 == 3) h.mode[1]->Fill(2); //HH
    else                                  h.mode[1]->Fill(-1); //other

    float dphi = delta_phi(vllep.at(0).v.Phi(), vllep.at(1).v.Phi());
    float dr   = vllep.at(0).v.DeltaR(vllep.at(1).v);
    h.gen[0]->Fill(dphi);
    h.gen[1]->Fill(dr);
  }
  
  //Production mode: LN
  else if((int)vllep.size()==1 && (int)vlnu.size()==1){
    h.mode[0]->Fill(1);
    //Decaymodes: ZW, HW
    int decayL = vllep.at(0).decaymode;
    int decayN = vlnu.at(0).decaymode;
    if     (decayL == 2 && decayN == 1) h.mode[2]->Fill(0); //ZW
    else if(decayL == 3 && decayN == 1) h.mode[2]->Fill(1); //HW
    else                                h.mode[2]->Fill(-1); //other

    float dphi = delta_phi(vllep.at(0).v.Phi(), vlnu.at(0).v.Phi());
    float dr   = vllep.at(0).v.DeltaR(vlnu.at(0).v);
    h.gen[0]->Fill(dphi);
    h.gen[1]->Fill(dr);
  }
  
  //Production mode: NN
  else if((int)vlnu.size()==2){
    h.mode[0]->Fill(2);
    //Decaymodes: WW
    int decayN0 = vlnu.at(0).decaymode;
    int decayN1 = vlnu.at(1).decaymode;
    if(decayN0 == 1 && decayN1 == 1) h.mode[3]->Fill(0); //WW
    else                             h.mode[3]->Fill(-1); //other

    float dphi = delta_phi(vlnu.at(0).v.Phi(), vlnu.at(1).v.Phi());
    float dr   = vlnu.at(0).v.DeltaR(vlnu.at(1).v);
    h.gen[0]->Fill(dphi);
    h.gen[1]->Fill(dr);
  }

  //Correction for bad events:
  bad_event = false;
  if(_flag=="doublet"){ //for VLLD files
    bad_event = false;
    //a) The neutral particle cannot decay to H,nu or Z,nu.
    // I am flagging out the events with Higgs(25) or the Z(23) as daughetrs of N
    //cout<<"----"<<endl;
    for(int i=0; i<(int)vlnu.size(); i++){
      for(int j=0; j<(int)vlnu.at(i).dauid.size(); j++){
	if(fabs(vlnu.at(i).dauid[j]) == 25)      bad_event = true;
	else if(fabs(vlnu.at(i).dauid[j]) == 23) bad_event = true;
      }
      //cout<<""<<endl;
    }	  
    //b) The lepton cannot decay to a W,nu of the corresponding flavor (ele/mu):
    // I am flagging out the events with W(24) as daughetrs of L
    for(int i=0; i<(int)vllep.size(); i++){
      for(int j=0; j<(int)vllep.at(i).dauid.size(); j++){
	if(fabs(vllep.at(i).dauid[j]) == 24)     bad_event = true;
      }
    }
  }
  if(bad_event) nEvtBad++;
  
  //----------------------------------
  // Other gen-level plots
  //-----------------------------------
  genMuon.clear();  genElectron.clear();  genLightLepton.clear(); genJet.clear();
  createGenLightLeptons();
  createGenJets();
  SortGenObjects();
  //SortPt(genMuon);
  //SortPt(genElectron);
  //SortPt(genLightLepton);

  float genHT=0, genLT=0, genST=0;
  h.gen[2]->Fill((int)genMuon.size());
  for(auto &mu: genMuon){
    h.gen[3]->Fill(mu.v.Pt());
    h.gen[4]->Fill(mu.v.Eta());
    h.gen[5]->Fill(mu.v.Phi());
  }
  h.gen[6]->Fill((int)genElectron.size());
  for(auto &el: genElectron){
    h.gen[7]->Fill(el.v.Pt());
    h.gen[8]->Fill(el.v.Eta());
    h.gen[9]->Fill(el.v.Phi());
  }
  h.gen[10]->Fill((int)genLightLepton.size());
  for(auto &lep:genLightLepton){
    h.gen[11]->Fill(lep.v.Pt());
    h.gen[12]->Fill(lep.v.Eta());
    h.gen[13]->Fill(lep.v.Phi());
    genLT+=lep.v.Pt();
  }
  h.gen[14]->Fill((int)genJet.size());
  for(auto &jet: genJet){
    h.gen[15]->Fill(jet.v.Pt());
    h.gen[16]->Fill(jet.v.Eta());
    h.gen[17]->Fill(jet.v.Phi());
    genHT+=jet.v.Pt();
  }
  genST=genHT+genLT+*GenMET_pt;
  h.gen[18]->Fill(genHT);
  h.gen[19]->Fill(genLT);
  h.gen[20]->Fill(genST);
  h.gen[21]->Fill(*GenMET_pt);
  h.gen[22]->Fill(*GenMET_phi);

  //----------------------------------
  // Reco-level plots
  //-----------------------------------
  float recoHT = 0, recoLT = 0, recoST = 0;
  h.reco[0]->Fill((int)Muon.size());
  for (auto &mu : Muon) {
    h.reco[1]->Fill(mu.v.Pt());
    h.reco[2]->Fill(mu.v.Eta());
    h.reco[3]->Fill(mu.v.Phi());
  }
  h.reco[4]->Fill((int)Electron.size());
  for (auto &el : Electron) {
    h.reco[5]->Fill(el.v.Pt());
    h.reco[6]->Fill(el.v.Eta());
    h.reco[7]->Fill(el.v.Phi());
  }
  h.reco[8]->Fill((int)LightLepton.size());
  for (auto &lep : LightLepton) {
    h.reco[9]->Fill(lep.v.Pt());
    h.reco[10]->Fill(lep.v.Eta());
    h.reco[11]->Fill(lep.v.Phi());
    recoLT += lep.v.Pt();
  }
  h.reco[12]->Fill((int)Jet.size());
  for (auto &jet : Jet) {
    h.reco[13]->Fill(jet.v.Pt());
    h.reco[14]->Fill(jet.v.Eta());
    h.reco[15]->Fill(jet.v.Phi());
    recoHT += jet.v.Pt();
  }
  h.reco[16]->Fill((int)MediumbJet.size());
  for (auto &b : MediumbJet) {
    h.reco[17]->Fill(b.v.Pt());
    h.reco[18]->Fill(b.v.Eta());
    h.reco[19]->Fill(b.v.Phi());
    recoHT += b.v.Pt();
  }
  recoST = recoHT + recoLT + *PuppiMET_pt;
  h.reco[20]->Fill(recoHT);
  h.reco[21]->Fill(recoLT);
  h.reco[22]->Fill(recoST);
  h.reco[23]->Fill(*PuppiMET_pt);
  h.reco[24]->Fill(*PuppiMET_phi);
}
