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
  //Second lepton
  Float_t lep1_pt  = (Float_t)LightLepton.at(1).v.Pt();
  Float_t lep1_eta = (Float_t)LightLepton.at(1).v.Eta();
  Float_t lep1_phi = (Float_t)LightLepton.at(1).v.Phi();
  Float_t lep1_iso = (Float_t)LightLepton.at(1).reliso03;
  Float_t lep1_mt  = (Float_t)transv_mass(LightLepton.at(1).v.E(), LightLepton.at(1).v.Phi(), metpt, metphi);

  //Dilepton system:
  TLorentzVector dilep = LightLepton.at(0).v + LightLepton.at(1).v;
  Float_t dilep_pt   = (Float_t)dilep.Pt();
  Float_t dilep_eta  = (Float_t)dilep.Eta();
  Float_t dilep_phi  = (Float_t)dilep.Phi();
  Float_t dilep_mass = (Float_t)dilep.M();
  Float_t dilep_mt   = (Float_t)transv_mass(dilep.E(), dilep.Phi(), metpt, metphi);
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

  bool qcd_CR = nbjet==0 && dilep_mass > 50 && dphi_metdilep < 1.5;
  bool qcd_VR = nbjet==0 && dilep_mass > 50 && dphi_metdilep > 1.5;
  bool event_selection = qcd_CR;

  //------------------------
  // QCD scaling in HT bins:
  //------------------------
  float qcdscale = 1; //default
  if(HT < 50)                 qcdscale = 0.1414586;
  else if (HT<=50  && HT<100) qcdscale = 0.0552467;
  else if (HT<=100 && HT<150) qcdscale = 0.0351217;
  else if (HT<=150 && HT<200) qcdscale = 0.0296351;
  else if (HT<=200 && HT<250) qcdscale = 0.0264910;
  else                        qcdscale = 0.0175513;
  if(_flag == "qcd") wt = wt*qcdscale;

  //-----------
  // Plotting:
  //-----------
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
    
  }
  
  

}
