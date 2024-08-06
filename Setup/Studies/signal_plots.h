void AnaScript::MakeSignalPlots(float wt){
  
  int nvll = (int)vllep.size();
  int nvlnu = (int)vlnu.size();
  bool pair_production_L = false;
  bool pair_production_N = false;
  bool associated_production = false;
  if(!bad_event){
    if(nvll==2 && nvlnu == 0)      pair_production_L = true;
    else if(nvll==1 && nvlnu == 1) associated_production = true;
    else if(nvll==0 && nvlnu == 2) pair_production_N = true;
  }

  //Basic plots for Vector like lepton:    
  h.vll[0]->Fill((int)vllep.size());
  for(int i=0; i<(int)vllep.size(); i++){
    h.vll[1]->Fill( vllep.at(i).v.Pt(),  wt);
    h.vll[2]->Fill( vllep.at(i).v.Eta(), wt);
    h.vll[3]->Fill( vllep.at(i).v.Phi(), wt);
    h.vll[4]->Fill( vllep.at(i).v.M(),   wt);
    h.vll[5]->Fill( vllep.at(i).pdgid,  wt);
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
    h.vln[5]->Fill( vlnu.at(i).pdgid,  wt);
    for(int j=0; j<(int)vlnu.at(i).dauid.size(); j++){
      h.vln[6] -> Fill(vlnu.at(i).decaymode);
      h.vln[7] -> Fill(vlnu.at(i).dauid[j]); //j-th daugher of the i-th particle
    }
  }

  //---------------------------------------
  // Investigating the feynman diagrams
  //---------------------------------------

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
  //Counting events in the final state, irrespective of the intermediate decay modes:
  h.sig[4]                       ->Fill((int)0, wt); //All events, excluding the bad ones
  if(evt_1L0J)           h.sig[4]->Fill((int)1, wt); //1L0J events
  else if(evt_1L1J)      h.sig[4]->Fill((int)2, wt); //1L1J events
  else if(evt_1L2J_incl) h.sig[4]->Fill((int)3, wt); //1L2J+ events
  else if(evt_2LOS)      h.sig[4]->Fill((int)4, wt); //2LOS events
  else if(evt_2LSS)      h.sig[4]->Fill((int)5, wt); //2LSS events
  else if(evt_3L)        h.sig[4]->Fill((int)6, wt); //3L events
  else if(evt_4L_incl)   h.sig[4]->Fill((int)7, wt); //4L+ events
  
}
