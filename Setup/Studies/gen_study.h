void AnaScript::Make_gen2LSS_plots(float wt){

  //This function is called for 2LSS events.
  //QCD HT binned scaling is already done by the previous function which called right before this one.

  //Let's split 2LSS into ee, emu and mumu events.
  bool evt_ee = false; bool evt_mm = false; bool evt_em = false;
  if((int)Electron.size()==2) evt_ee = true;
  else if((int)Muon.size()==2) evt_mm = true;
  else if((int)Electron.size()==1 && (int)Muon.size()==1) evt_em = true;

  //Counting events:
  h.gen2LSS[0]                ->Fill((int)0, wt);
  if(evt_ee)      h.gen2LSS[0]->Fill((int)1, wt);
  else if(evt_em) h.gen2LSS[0]->Fill((int)2, wt);
  else if(evt_mm) h.gen2LSS[0]->Fill((int)3, wt);
  else            h.gen2LSS[0]->Fill((int)4, wt);

  //Correcting for charge mimeasurement in both leptons.
  bool gen_matching = true;
  gen_matching = (isMatchingWithGen(LightLepton.at(0), genLightLepton) &&
		  isMatchingWithGen(LightLepton.at(1), genLightLepton));
  
  //-----------
  //e-e events:
  //-----------
  if(evt_ee){
    
    //leading:
    float dRmin_ele0 = 999;
    int genind_ele0 = -1;
    bool genmatch_ele0 = false;
    bool chargematch_ele0 = false;
    for(int i=0; i<(int)genElectron.size(); i++){
      float dR_temp = Electron.at(0).v.DeltaR(genElectron.at(i).v);
      if(dR_temp < dRmin_ele0){
	dRmin_ele0 = dR_temp;
	genind_ele0 = i;
      }
    }
    if(dRmin_ele0 < 0.4) genmatch_ele0 = true;
    if(genmatch_ele0 && Electron.at(0).charge==genElectron.at(genind_ele0).charge) chargematch_ele0 = true;
    h.gen2LSS[1]->Fill(dRmin_ele0, wt);
    if(genmatch_ele0) h.gen2LSS[3]->Fill(Electron.at(0).charge - genElectron.at(genind_ele0).charge);
    else              h.gen2LSS[3]->Fill(-5);
    
    //sub-leading:
    float dRmin_ele1 = 999;
    int genind_ele1 = -1;
    bool genmatch_ele1 = false;
    bool chargematch_ele1 = false;
    for(int i=0; i<(int)genElectron.size(); i++){
      float dR_temp = Electron.at(1).v.DeltaR(genElectron.at(i).v);
      if(dR_temp < dRmin_ele1){
	dRmin_ele1 = dR_temp;
	genind_ele1 = i;
      }
    }
    if(dRmin_ele1 < 0.4) genmatch_ele1 = true;
    if(genmatch_ele1 && Electron.at(1).charge==genElectron.at(genind_ele1).charge) chargematch_ele1 = true;
    h.gen2LSS[2]->Fill(dRmin_ele1, wt);
    if(genmatch_ele1) h.gen2LSS[4]->Fill(Electron.at(1).charge - genElectron.at(genind_ele1).charge);
    else              h.gen2LSS[4]->Fill(-5);

    //Gen-matching rate:
    h.gen2LSS[5]                                   ->Fill(0);
    if(genmatch_ele0)                  h.gen2LSS[5]->Fill(1);
    if(genmatch_ele1)                  h.gen2LSS[5]->Fill(2);
    if(genmatch_ele0 && genmatch_ele1) h.gen2LSS[5]->Fill(3);

    //Charge-matching rate:
    h.gen2LSS[6]                                         ->Fill(0);
    if(chargematch_ele0)                     h.gen2LSS[6]->Fill(1);
    if(chargematch_ele1)                     h.gen2LSS[6]->Fill(2);
    if(chargematch_ele0 && chargematch_ele1) h.gen2LSS[6]->Fill(3);
    
  }

  //------------
  //e-mu events:
  //------------
  if(evt_em){
    
    //leading:
    float dRmin_lep0 = 999;
    int genind_lep0 = -1;
    bool genmatch_lep0 = false;
    bool chargematch_lep0 = false;
    for(int i=0; i<(int)genLightLepton.size(); i++){
      if( fabs(LightLepton.at(0).id)==fabs(genLightLepton.at(i).id) ){ //has to be the same particle
	float dR_temp = LightLepton.at(0).v.DeltaR(genLightLepton.at(i).v);
	if(dR_temp < dRmin_lep0){
	  dRmin_lep0 = dR_temp;
	  genind_lep0 = i;
	}
      }
    }
    if(dRmin_lep0 < 0.4) genmatch_lep0 = true;
    if(genmatch_lep0 && LightLepton.at(0).charge==genLightLepton.at(genind_lep0).charge) chargematch_lep0 = true;
    h.gen2LSS[7]->Fill(dRmin_lep0, wt);
    if(genmatch_lep0) h.gen2LSS[9]->Fill(Electron.at(0).charge - genElectron.at(genind_lep0).charge, wt);
    else              h.gen2LSS[9]->Fill(-5);

    //sub-leading:
    float dRmin_lep1 = 999;
    int genind_lep1 = -1;
    bool genmatch_lep1 = false;
    bool chargematch_lep1 = false;

    for(int i=0; i<(int)genLightLepton.size(); i++){
      if( fabs(LightLepton.at(1).id) == fabs(genLightLepton.at(i).id) ){ //has to be the same particle
	float dR_temp = LightLepton.at(1).v.DeltaR(genLightLepton.at(i).v);
	if(dR_temp < dRmin_lep1){
	  dRmin_lep1 = dR_temp;
	  genind_lep1 = i;
	}
      }
    }
    if(dRmin_lep1 < 0.4) genmatch_lep1 = true;
    if(genmatch_lep1 && LightLepton.at(1).charge==genLightLepton.at(genind_lep1).charge) chargematch_lep1 = true;
    h.gen2LSS[8]->Fill(dRmin_lep1, wt);
    if(genmatch_lep1) h.gen2LSS[10]->Fill(Electron.at(1).charge - genElectron.at(genind_lep1).charge, wt);
    else              h.gen2LSS[10]->Fill(-5, wt);

    //Gen-matching rate:
    h.gen2LSS[11]                                   ->Fill(0);
    if(genmatch_lep0)                  h.gen2LSS[11]->Fill(1);
    if(genmatch_lep1)                  h.gen2LSS[11]->Fill(2);
    if(genmatch_lep0 && genmatch_lep1) h.gen2LSS[11]->Fill(3);

    //Charge-matching rate:
    h.gen2LSS[12]                                         ->Fill(0);
    if(chargematch_lep0)                     h.gen2LSS[12]->Fill(1);
    if(chargematch_lep1)                     h.gen2LSS[12]->Fill(2);
    if(chargematch_lep0 && chargematch_lep1) h.gen2LSS[12]->Fill(3);
  }

  //------------
  //mu-mu events:
  //------------
  if(evt_mm){
    
    //leading:
    float dRmin_mu0 = 999;
    int genind_mu0 = -1;
    bool genmatch_mu0 = false;
    bool chargematch_mu0 = false;
    for(int i=0; i<(int)genMuon.size(); i++){
      float dR_temp = Muon.at(0).v.DeltaR(genMuon.at(i).v);
      if(dR_temp < dRmin_mu0){
	dRmin_mu0 = dR_temp;
	genind_mu0 = i;
      }
    }
    if(dRmin_mu0 < 0.4) genmatch_mu0 = true;
    if(genmatch_mu0 && Muon.at(0).charge==genMuon.at(genind_mu0).charge) chargematch_mu0 = true;
    h.gen2LSS[13]->Fill(dRmin_mu0, wt);
    if(genmatch_mu0) h.gen2LSS[15]->Fill(Muon.at(0).charge - genMuon.at(genind_mu0).charge);
    else             h.gen2LSS[15]->Fill(-5);

    //sub-leading:
    float dRmin_mu1 = 999;
    int genind_mu1 = -1;
    bool genmatch_mu1 = false;
    bool chargematch_mu1 = false;
    for(int i=0; i<(int)genMuon.size(); i++){
      float dR_temp = Muon.at(1).v.DeltaR(genMuon.at(i).v);
      if(dR_temp < dRmin_mu1){
	dRmin_mu1 = dR_temp;
	genind_mu1 = i;
      }
    }
    if(dRmin_mu1 < 0.4) genmatch_mu1 = true;
    if(genmatch_mu1 && Muon.at(1).charge==genMuon.at(genind_mu1).charge) chargematch_mu1 = true;
    h.gen2LSS[14]->Fill(dRmin_mu1, wt);
    if(genmatch_mu1) h.gen2LSS[16]->Fill(Muon.at(1).charge - genMuon.at(genind_mu1).charge);
    else             h.gen2LSS[16]->Fill(-5);

    //Gen-matching rate:
    h.gen2LSS[17]                                 ->Fill(0);
    if(genmatch_mu0)                 h.gen2LSS[17]->Fill(1);
    if(genmatch_mu1)                 h.gen2LSS[17]->Fill(2);
    if(genmatch_mu0 && genmatch_mu1) h.gen2LSS[17]->Fill(3);

    //Charge-matching rate:
    h.gen2LSS[18]                                       ->Fill(0);
    if(chargematch_mu0)                    h.gen2LSS[18]->Fill(1);
    if(chargematch_mu1)                    h.gen2LSS[18]->Fill(2);
    if(chargematch_mu0 && chargematch_mu1) h.gen2LSS[18]->Fill(3);
    
  }
  
}
