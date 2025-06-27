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

    //h.sig[0]->Fill(GenPart_pdgId[i], 1.0);

    bool mother_is_Z = (temp.momid == 23);
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

  SortVLL();

  //-------------------------------
  // Per event on-screen displays:
  //-------------------------------
  
  bad_event = false;
  cout<<"\n----- Event no. "<<nEvtTotal<<" -----"<<endl;
  
  //1. Counting VLLS:
  int nvll = (int)vllep.size();
  int nvlnu = (int)vlnu.size();
  cout<<"nVLL = "<<nvll<<" , nVLNu = "<<nvlnu<<", total = "<<nvll+nvlnu<<endl;

  //2. Daughters:
  cout<<"Daughters of VLL:  ";
  for(int i=0; i<(int)vllep.size(); i++){
    for(int j=0; j<(int)vllep.at(i).dauid.size(); j++){
      cout << setw(4) << vllep.at(i).dauid[j] << ", ";
      if(fabs(vllep.at(i).dauid[j]) == 24)     bad_event = true;
      //The charged lepton cannot decay to a W,nu of the corresponding flavor (ele/mu):
    }
  }
  cout<<endl;
  cout<<"Daughters of VLNu: ";
  for(int i=0; i<(int)vlnu.size(); i++){
    for(int j=0; j<(int)vlnu.at(i).dauid.size(); j++){
      cout << setw(4) << vlnu.at(i).dauid[j] << ", ";
      if(fabs(vlnu.at(i).dauid[j]) == 25)      bad_event = true;
      else if(fabs(vlnu.at(i).dauid[j]) == 23) bad_event = true;
      //The neutral lepton  cannot decay to H,nu or Z,nu.
    }
  }
  cout<<endl;
  if(bad_event) cout<<"\033[31mInvalid decay!\033[0m"<<endl;
  else cout<<"\033[93mValid decay.\033[0m"<<endl;

}
