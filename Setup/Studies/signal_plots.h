void AnaScript::MakeSignalPlots(float wt){

  //Printing out the GenPart Array: 
  int test_event = 100;
  if(nEvtTotal == test_event){
    cout<<"\n------------------Event display-------------------"<<endl;
    cout<<"index\tpdgId\tname\tmomind\tmomid\tname\tstat"<<endl;
    for(unsigned int i=0; i< (*nGenPart); i++){
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
  
  //---------------------------------------------------
  //Creating an array of VLLS:
  vllep.clear();
  vlnu.clear();
  
  for(unsigned int i=0; i< (*nGenPart); i++){
    Particle temp;
    temp.v.SetPtEtaPhiM(GenPart_pt[i],GenPart_eta[i],GenPart_phi[i],GenPart_mass[i]);
    temp.status = GenPart_status[i];
    temp.ind = i;
    temp.pdgid = GenPart_pdgId[i];
    //temp.momid= MotherID(i,GenPart_genPartIdxMother[i]);
    temp.momid = GenPart_pdgId[GenPart_genPartIdxMother[i]]; //Not fixing the mother right now

    h.sig[0]->Fill(GenPart_pdgId[i], wt);

    bool mother_is_Z = (temp.momid == 23);
    bool mother_is_W = (fabs(temp.momid) == 24);
    bool not_its_own_mother = (temp.momid != temp.pdgid); //To avoid overcounting

    //Finding daughters:
    vector<int> daughter_ind, daughter_id;
    daughter_ind.clear(); daughter_id.clear();

    for(unsigned int j=0; j< (*nGenPart); j++){
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
      if(fabs(temp.dauid[j]) == 23)      temp.decaymode = 2; //Z
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

  SortPt(vllep);
  SortPt(vlnu);
  
  //---------------------------------------------------
  //Counting daughters:

  if(nEvtTotal == test_event){
    cout<<"nVLL = "<<(int)vllep.size()<<endl;
    if((int)vllep.size()>0){
      cout<<"Daughters of the leading VLL = ";
      for(int i=0; i<(int)vllep.at(0).dauid.size(); i++){
	cout<<ParticleName(vllep.at(0).dauid[i])<<", ";
      }
      cout<<endl;
    }
    cout<<"nVNu = "<<(int)vlnu.size()<<endl;
    if((int)vlnu.size()>0){
      cout<<"Daughters of the leading VLNu = ";
      for(int i=0; i<(int)vlnu.at(0).dauid.size(); i++){
	cout<<ParticleName(vlnu.at(0).dauid[i])<<", ";
      }
      cout<<endl;
    }
  }

  //###################################################
  //Flag out the invalid decay modes:
  bool bad_event = false;
  
  //In the Doublet model:
  //if(strcmp(_Flag, "doublet") == 0){
  if(_flag=="doublet"){ //for VLLD files
    //a) The neutral particle cannot decay to a H or a Z
    for(int i=0; i<(int)vlnu.size(); i++){
      for(int j=0; j<(int)vlnu.at(i).dauid.size(); j++){
	if(fabs(vlnu.at(i).dauid[j]) == 25)      bad_event = true;
	else if(fabs(vlnu.at(i).dauid[j]) == 23) bad_event = true;
      }
    }
    //b) The lepton cannot decay to a W:
    for(int i=0; i<(int)vllep.size(); i++){
      for(int j=0; j<(int)vllep.at(i).dauid.size(); j++){
	if(fabs(vllep.at(i).dauid[j]) == 24)     bad_event = true;
      }
    }
  }
  //The singlet models are fine.
  //###################################################

  //Only plot if it's a valid event:
  if(bad_event) h.hist[9]->Fill(0);//Counting invalid events.
  else {
    h.hist[9]->Fill(1);
  
    //Basic plots for Vector like lepton:
    h.vll[0]->Fill((int)vllep.size());
    for(int i=0; i<(int)vllep.size(); i++){
      h.vll[1]->Fill( vllep.at(i).v.Pt(),  wt);
      h.vll[2]->Fill( vllep.at(i).v.Eta(), wt);
      h.vll[3]->Fill( vllep.at(i).v.Phi(), wt);
      h.vll[4]->Fill( vllep.at(i).v.M(),   wt);
      h.vll[5]->Fill( vllep.at(i).charge,  wt);
      for(int j=0; j<(int)vllep.at(i).dauid.size(); j++){
	h.vll[6] -> Fill(vllep.at(i).decaymode);
	h.vll[7] -> Fill(vllep.at(i).dauid[j]); //j-th daugher of the i-th particle
      }
    }

    //Basic plots for Vector like neutrino:
    h.vln[0]->Fill((int)vlnu.size());
    for(int i=0; i<(int)vlnu.size(); i++){
      h.vln[1]->Fill( vlnu.at(i).v.Pt(),  wt);
      h.vln[2]->Fill( vlnu.at(i).v.Eta(), wt);
      h.vln[3]->Fill( vlnu.at(i).v.Phi(), wt);
      h.vln[4]->Fill( vlnu.at(i).v.M(),   wt);
      h.vln[5]->Fill( vlnu.at(i).charge,  wt);
      for(int j=0; j<(int)vlnu.at(i).dauid.size(); j++){
	h.vln[6] -> Fill(vlnu.at(i).decaymode);
	h.vln[7] -> Fill(vlnu.at(i).dauid[j]); //j-th daugher of the i-th particle
      }
    }

    //---------------------------------------
    // Investigating the feynman diagrams
    //---------------------------------------
    int nvll = (int)vllep.size();
    int nvlnu = (int)vlnu.size();
    bool pair_production_L = false;
    bool pair_production_N = false;
    bool associated_production = false;
    if(nvll==2 && nvlnu == 0)      pair_production_L = true;
    else if(nvll==1 && nvlnu == 1) associated_production = true;
    else if(nvll==0 && nvlnu == 2) pair_production_N = true;

    if(pair_production_L){
      h.sig[1]                       ->Fill((int)0, wt); //All events
      if(evt_1L0J)           h.sig[1]->Fill((int)1, wt); //1L0J events
      else if(evt_1L1J)      h.sig[1]->Fill((int)2, wt); //1L1J events
      else if(evt_1L2J_incl) h.sig[1]->Fill((int)3, wt); //1L2J+ events
      else if(evt_2LOS)      h.sig[1]->Fill((int)4, wt); //2LOS events
      else if(evt_2LSS)      h.sig[1]->Fill((int)5, wt); //2LSS events
      else if(evt_3L)        h.sig[1]->Fill((int)6, wt); //3L events
      else if(evt_4L_incl)   h.sig[1]->Fill((int)7, wt); //4L+ events
    }
    else if(associated_production){
      h.sig[2]                       ->Fill((int)0, wt); //All events
      if(evt_1L0J)           h.sig[2]->Fill((int)1, wt); //1L0J events
      else if(evt_1L1J)      h.sig[2]->Fill((int)2, wt); //1L1J events
      else if(evt_1L2J_incl) h.sig[2]->Fill((int)3, wt); //1L2J+ events
      else if(evt_2LOS)      h.sig[2]->Fill((int)4, wt); //2LOS events
      else if(evt_2LSS)      h.sig[2]->Fill((int)5, wt); //2LSS events
      else if(evt_3L)        h.sig[2]->Fill((int)6, wt); //3L events
      else if(evt_4L_incl)   h.sig[2]->Fill((int)7, wt); //4L+ events
    }
    else if(pair_production_N){
      h.sig[3]                       ->Fill((int)0, wt); //All events
      if(evt_1L0J)           h.sig[3]->Fill((int)1, wt); //1L0J events
      else if(evt_1L1J)      h.sig[3]->Fill((int)2, wt); //1L1J events
      else if(evt_1L2J_incl) h.sig[3]->Fill((int)3, wt); //1L2J+ events
      else if(evt_2LOS)      h.sig[3]->Fill((int)4, wt); //2LOS events
      else if(evt_2LSS)      h.sig[3]->Fill((int)5, wt); //2LSS events
      else if(evt_3L)        h.sig[3]->Fill((int)6, wt); //3L events
      else if(evt_4L_incl)   h.sig[3]->Fill((int)7, wt); //4L+ events
    }
  }//!bad_event
    
}