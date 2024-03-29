void AnaScript::Make_evt2LSS_plots(float wt){
  
  //This function is intended to run for 2LSS events which pass a trigger.
  //The event selections are defined in EVentSelections.h
  //This function is called in the main C file.

  //----------------------
  // Declaring variables:
  //----------------------

  //Counts:
  UInt_t nlep  = (UInt_t)LightLepton.size();
  UInt_t njet  = (UInt_t)Jet.size();
  UInt_t nbjet = (UInt_t)bJet.size();

  //Object-level variables: First lepton
  Float_t lep0_pt  = (Float_t)LightLepton.at(0).v.Pt();
  Float_t lep0_eta = (Float_t)LightLepton.at(0).v.Eta();
  Float_t lep0_phi = (Float_t)LightLepton.at(0).v.Phi();
  Float_t lep0_iso = (Float_t)LightLepton.at(0).reliso03;
  Float_t lep0_mt  = (Float_t)transv_mass(LightLepton.at(0).v.E(), LightLepton.at(0).v.Phi(), metpt, metphi);
  int lep0_charge  = LightLepton.at(0).charge; 
  //Second lepton
  Float_t lep1_pt  = (Float_t)LightLepton.at(1).v.Pt();
  Float_t lep1_eta = (Float_t)LightLepton.at(1).v.Eta();
  Float_t lep1_phi = (Float_t)LightLepton.at(1).v.Phi();
  Float_t lep1_iso = (Float_t)LightLepton.at(1).reliso03;
  Float_t lep1_mt  = (Float_t)transv_mass(LightLepton.at(1).v.E(), LightLepton.at(1).v.Phi(), metpt, metphi);
  int lep1_charge  = LightLepton.at(1).charge;
  
  //Dilepton system:
  TLorentzVector dilep = LightLepton.at(0).v + LightLepton.at(1).v;
  Float_t dilep_pt   = (Float_t)dilep.Pt();
  Float_t dilep_eta  = (Float_t)dilep.Eta();
  Float_t dilep_phi  = (Float_t)dilep.Phi();
  Float_t dilep_mass = (Float_t)dilep.M();
  Float_t dilep_mt   = (Float_t)transv_mass(dilep.E(), dilep.Phi(), metpt, metphi);
  int dilep_charge   = LightLepton.at(0).charge + LightLepton.at(1).charge;
  Float_t dilep_deta    = (Float_t)fabs(LightLepton.at(0).v.Eta() - LightLepton.at(1).v.Eta());
  Float_t dilep_dphi    = (Float_t)delta_phi(LightLepton.at(0).v.Phi(), LightLepton.at(1).v.Phi());
  Float_t dilep_dR      = (Float_t)LightLepton.at(0).v.DeltaR(LightLepton.at(1).v);
  Float_t dilep_ptratio = (Float_t)lep1_pt/lep0_pt;

  //Event level:
  Float_t HT=0; for(Int_t i=0; i<(Int_t)Jet.size(); i++) HT = HT + Jet.at(i).v.Pt();
  Float_t ST = HT + dilep_pt;
  Float_t STfrac = 0; if(ST>0) STfrac = dilep_pt/ST;
  Float_t dphi_metlep0 = (Float_t)delta_phi(LightLepton.at(0).v.Phi(), metphi);
  Float_t dphi_metlep1 = (Float_t)delta_phi(LightLepton.at(1).v.Phi(), metphi);
  Float_t dphi_metdilep = (Float_t)delta_phi(dilep.Phi(), metphi);
  Float_t dphi_metlep_max = (Float_t)max(dphi_metlep0, dphi_metlep1);
  Float_t dphi_metlep_min = (Float_t)min(dphi_metlep0, dphi_metlep1);

  //-----------------------------------
  // Deciding further event selections:
  //-----------------------------------

  //For correcting QCD:
  bool qcd_enhanced = nbjet==0 && lep0_pt > 50;
  bool qcd_CR = qcd_enhanced && dilep_phi < 0;
  bool qcd_VR = qcd_enhanced && dilep_phi > 0;

  //Basic filtering of events
  bool basic_filtering = dilep_pt > 30 && fabs(dilep_eta) < 4;

  //Splitting into ee, emu and mumu:
  bool evt_ee = false; bool evt_mm = false; bool evt_em = false;
  if(fabs(LightLepton.at(0).id) == 11 && fabs(LightLepton.at(1).id) == 11)        evt_ee = true;
  else if(fabs(LightLepton.at(0).id) == 13 && fabs(LightLepton.at(1).id) == 13)   evt_mm = true;
  else if((fabs(LightLepton.at(0).id) == 13 && fabs(LightLepton.at(1).id) == 11)||
	  (fabs(LightLepton.at(0).id) == 11 && fabs(LightLepton.at(1).id) == 13)) evt_em = true;

  //--------------
  //Correcting DY:
  //--------------
  double scale_dy = 1.0;
  double scale_dy_ee = 1.0;
  
  bool inZwindow   = 76<dilep_mass && dilep_mass < 106;
  bool inZsideband = (55<dilep_mass && dilep_mass<70) || (110<dilep_mass && dilep_mass<125);

  if(evt_ee && nbjet==0){
    
    //Global scale calculation:
    if(inZwindow)   h.hist[10]->Fill((int)0, wt); //onZ
    else            h.hist[10]->Fill((int)1, wt); //offZ
    if(inZsideband) h.hist[10]->Fill((int)2, wt); //sideband
  

    //pt binned scale calculation:
    //Counting onZ and sidebandZ events in pT bins of the leading lepton.
    if(lep0_pt<50){ //bin0
      if(inZwindow)   h.hist[11]->Fill((int)0, wt); //onZ
      if(inZsideband) h.hist[12]->Fill((int)0, wt); //sideband
      scale_dy_ee = 0.0242;
    }
    else if(50<=lep0_pt && lep0_pt<100){ //bin1
      if(inZwindow)   h.hist[11]->Fill((int)1, wt); //onZ
      if(inZsideband) h.hist[12]->Fill((int)1, wt); //sideband
      scale_dy_ee = 0.01875;
    }
    else if(100<=lep0_pt && lep0_pt<200){ //bin2
      if(inZwindow)   h.hist[11]->Fill((int)2, wt); //onZ
      if(inZsideband) h.hist[12]->Fill((int)2, wt); //sideband
      scale_dy_ee = 0.01104;
    }
    else if(200<=lep0_pt){ //bin3
      if(inZwindow)   h.hist[11]->Fill((int)3, wt); //onZ
      if(inZsideband) h.hist[12]->Fill((int)3, wt); //sideband
      scale_dy_ee = 0.01368;
    }
  }
  
  scale_dy = scale_dy_ee;
  //if(_flag == "dy") wt = wt*scale_dy;

  //------------------------
  // QCD scaling in HT bins:
  //------------------------
  double qcdscale = 1.0; //default

  if(HT < 50)                 qcdscale = 0.0212517;
  else if (50<=HT  && HT<100) qcdscale = 0.0485609;
  else if (100<=HT && HT<150) qcdscale = 0.0745748;
  else if (150<=HT && HT<200) qcdscale = 0.0303177;
  else if (200<=HT && HT<250) qcdscale = 0.0225880;
  else if (250<=HT && HT<300) qcdscale = 0.0179411;
  else if (300<=HT && HT<350) qcdscale = 0.0118273;
  else if (350<=HT && HT<400) qcdscale = 0.0151473;
  else if (400<=HT && HT<450) qcdscale = 0.0103587;
  else                        qcdscale = 0.0105359;
  if(_flag == "qcd") wt = wt*qcdscale;

  //-----------
  // Plotting:
  //-----------
  bool event_selection = evt_ee && nbjet==0 && inZwindow;
  
  if(event_selection){
    nEvtPass++;
    
    h.evt2LSS[0] ->Fill(nlep, wt);
    h.evt2LSS[1] ->Fill(njet, wt);
    h.evt2LSS[2] ->Fill(nbjet, wt);

    h.evt2LSS[3] ->Fill(lep0_pt, wt);
    h.evt2LSS[4] ->Fill(lep0_eta, wt);
    h.evt2LSS[5] ->Fill(lep0_phi, wt);
    h.evt2LSS[6] ->Fill(lep0_iso, wt);
    h.evt2LSS[7] ->Fill(lep0_mt, wt);

    h.evt2LSS[8] ->Fill(lep1_pt, wt);
    h.evt2LSS[9] ->Fill(lep1_eta, wt);
    h.evt2LSS[10]->Fill(lep1_phi, wt);
    h.evt2LSS[11]->Fill(lep1_iso, wt);
    h.evt2LSS[12]->Fill(lep1_mt, wt);

    h.evt2LSS[13]->Fill(dilep_pt, wt);
    h.evt2LSS[14]->Fill(dilep_eta, wt);
    h.evt2LSS[15]->Fill(dilep_phi, wt);
    h.evt2LSS[16]->Fill(dilep_mass, wt);
    h.evt2LSS[17]->Fill(dilep_mt, wt);
    h.evt2LSS[18]->Fill(dilep_deta, wt);
    h.evt2LSS[19]->Fill(dilep_dphi, wt);
    h.evt2LSS[20]->Fill(dilep_dR, wt);
    h.evt2LSS[21]->Fill(dilep_ptratio, wt);

    h.evt2LSS[22]->Fill(HT, wt);
    h.evt2LSS[23]->Fill(ST, wt);
    h.evt2LSS[24]->Fill(STfrac, wt);

    h.evt2LSS[25]->Fill(dphi_metlep0, wt);
    h.evt2LSS[26]->Fill(dphi_metlep1, wt);
    h.evt2LSS[27]->Fill(dphi_metdilep, wt);
    h.evt2LSS[28]->Fill(dphi_metlep_max, wt);
    h.evt2LSS[29]->Fill(dphi_metlep_min, wt);

    h.evt2LSS[30]->Fill(metpt, wt);
    h.evt2LSS[31]->Fill(metphi, wt);

    //Charge and ID check:
    h.evt2LSS[32]->Fill(lep0_charge, wt);
    h.evt2LSS[33]->Fill(lep1_charge, wt);
    h.evt2LSS[34]->Fill(dilep_charge, wt);
    h.evt2LSS[35]->Fill(fabs(LightLepton.at(0).id), wt);
    h.evt2LSS[36]->Fill(fabs(LightLepton.at(1).id), wt);
    
  }
  
  

}
