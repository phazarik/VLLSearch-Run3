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


  //----------------------------------------------------
  //Specific acceptance study for Arnab [August, 2024]:
  //Define specific region here.

  bool signal_region = false;
  bool mujj = false;

  if(evt_1L2J_incl){

    //mu-jj basic event selection:
    mujj = (int)Muon.size()==1 && (int)Jet.size()>1;

    if(mujj){

      //Variables used in event selection:
      float drmuj0 = Muon.at(0).v.DeltaR(Jet.at(0).v);
      float drmuj1 = Muon.at(0).v.DeltaR(Jet.at(1).v);
      float drj0j1 = Jet.at(0).v.DeltaR(Jet.at(1).v);
      float dijet_mass = (Jet.at(0).v + Jet.at(1).v).M();
      float HT=0; for(int i=0; i<(int)Jet.size(); i++) HT = HT + Jet.at(i).v.Pt();
      float ST = Muon.at(0).v.Pt() + HT + metpt;
      
      //More selections:
      bool vetoloose = (int)LightLepton.size()==1;
      bool mjj_40    = dijet_mass > 40;
      bool dr_selections = drmuj0>0.4 && drmuj1>0.4 && drj0j1>0.4;
      bool no_bjet   = (int)MediumbJet.size()==0;

      if(vetoloose && mjj_40 && dr_selections && no_bjet){
	//SR pre-selection
	bool mjj_50_110 = 50 < dijet_mass && dijet_mass < 110;
	bool dr_2p6     = drj0j1 < 2.6;
	bool ST_250     = ST > 250;

	if(mjj_50_110 && dr_2p6 && ST_250) signal_region = true;	
      }//baseline
    }//mujj
  }//1L2J

  h.sig[5]                  ->Fill((int)0, 1.0); //All events, excluding the bad ones
  if(evt_1L2J_incl) h.sig[5]->Fill((int)1, 1.0);
  if(evt_2LOS)      h.sig[5]->Fill((int)2, 1.0);
  if(evt_2LSS)      h.sig[5]->Fill((int)3, 1.0);
  if(evt_3L)        h.sig[5]->Fill((int)4, 1.0);
  if(evt_4L_incl)   h.sig[5]->Fill((int)5, 1.0);
  if(mujj)          h.sig[5]->Fill((int)6, 1.0);
  if(signal_region) h.sig[5]->Fill((int)7, 1.0);

  //With b-jet selection:
  h.sig[6]                  ->Fill((int)0, 1.0); //All events, excluding the bad ones
  if((int)MediumbJet.size()==0){
    if(evt_1L2J_incl) h.sig[6]->Fill((int)1, 1.0);
    if(evt_2LOS)      h.sig[6]->Fill((int)2, 1.0);
    if(evt_2LSS)      h.sig[6]->Fill((int)3, 1.0);
    if(evt_3L)        h.sig[6]->Fill((int)4, 1.0);
    if(evt_4L_incl)   h.sig[6]->Fill((int)5, 1.0);
    if(mujj)          h.sig[6]->Fill((int)6, 1.0);
    if(signal_region) h.sig[6]->Fill((int)7, 1.0);
  }

}
