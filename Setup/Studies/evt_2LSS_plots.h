void AnaScript::Make2LSSPlots(){
  
  //This function is intended to run for 2LSS events which pass a trigger.
  //The event selections are defined in EVentSelections.h
  //This function is called in the main C file.

  //##################################################################################################
  //Corrections to Jets:
  float jec = 1.0;
  float jer = 1.0;
  if(_data == 0){
    for(int i=0; i<(int)Jet.size(); i++){
      
      jec = correctionlib_jetSF(Jet.at(i),"nom");
      jer = correctionlib_jetRF(Jet.at(0),genJet,*fixedGridRhoFastjetAll,"nom");
      
      //Jet Energy Correction:
      Jet.at(i).v = Jet.at(i).v * jec;
      Jet.at(i).v = Jet.at(i).v * jer;
      
      h.evtweight[5]->Fill(jec);
      h.evtweight[6]->Fill(jer);
      h.evtweight[7]->Fill(jec*jer);
    }
  }
  //##################################################################################################
  
  //Picking events with exactly 2muons with same-sign, where the leading muon pT>26 GeV.
  bool basic_evt_selection = false;
  bool ee = false;
  bool em = false;
  bool mm = false;

  //Offline cuts on the leptons:
  float ptcut_mu  = 26; if(_campaign=="2017_UL") ptcut_mu = 32;
  float ptcut_ele = 35;
  
  if(LightLepton.size() == 2){
    if(LightLepton.at(0).charge == LightLepton.at(1).charge){

      int flav0 = fabs(LightLepton.at(0).id);
      int flav1 = fabs(LightLepton.at(1).id);
      //At least one of these two leptons must satisfy the offline trigger cut.
      bool offline_trigger = false;
      if(      flav0 == 13 && LightLepton.at(0).v.Pt() > ptcut_mu)  offline_trigger = true;
      else if (flav0 == 11 && LightLepton.at(0).v.Pt() > ptcut_ele) offline_trigger = true;
      if(      flav1 == 13 && LightLepton.at(1).v.Pt() > ptcut_mu)  offline_trigger = true;
      else if (flav1 == 11 && LightLepton.at(1).v.Pt() > ptcut_ele) offline_trigger = true;
      
      if(offline_trigger){
	if(     flav0 == 13 && flav1 == 13){ n2muss++; mm = true; }
	else if(flav0 == 13 && flav1 == 11){ nemuss++; em = true; }
	else if(flav0 == 11 && flav1 == 13){ nemuss++; em = true; }
	else if(flav0 == 11 && flav1 == 11){ n2ess++;  ee = true; }
      }
    }
  }
  
  //#######################
  // Select the channel :
  //
  basic_evt_selection = mm;
  //
  //#######################
  
  if(basic_evt_selection){

    //Calculating event weights:
    double wt = 1.0;
    double lepIdIsoSF = 1.0;
    double triggerEff = 1.0;
    double bjetSF = 1.0;
    /*
    if(_data==0){
      double sf0 = Muon_2018UL_Reco(Muon.at(0).v.Pt(), Muon.at(0).v.Eta());
      double sf1 = Muon_2018UL_Reco(Muon.at(1).v.Pt(), Muon.at(1).v.Eta());
      scalefactor = sf0*sf1;
      double ef0 = TrigEff_2018_IsoMu24_Data(Muon.at(0).v.Pt(), Muon.at(0).v.Eta());
      double ef1 = TrigEff_2018_IsoMu24_Data(Muon.at(1).v.Pt(), Muon.at(1).v.Eta());
      triggereff = 1-((1-ef0)*(1-ef1));
      wt = scalefactor*triggereff;
      //Corrections for Jets:
      double jeteff = 1.0;
      if(_campaign == "UL18") bjeteff = bTagEff_UL2018(Jet, 0); //Tweaking the SF from POG by 0 %
      wt = wt*bjeteff;
      }*/

    //Using correctionlib:
    if(_data==0){

      //Corrections for muon reconstruction:
      //Options: "nom", "up", "down"
      double sf0 = 1.0; double sf1 = 1.0;
      if(fabs(LightLepton.at(0).id) == 13){
	sf0 = correctionlib_muonIDSF(LightLepton.at(0),"nom") * correctionlib_muonIsoSF(LightLepton.at(0),"nom");
	sf1 = correctionlib_muonIDSF(LightLepton.at(1),"nom") * correctionlib_muonIsoSF(LightLepton.at(1),"nom");
      }
      else if(fabs(LightLepton.at(0).id) == 11){
	sf0 = correctionlib_egmIDSF(LightLepton.at(0), "nom");
	sf1 = correctionlib_egmIDSF(LightLepton.at(1), "nom");
      }
      lepIdIsoSF = sf0*sf1;

      //Trigger efficiencies:
      double ef0 = GetLeptonTriggerEfficiency(LightLepton.at(0));
      double ef1 = GetLeptonTriggerEfficiency(LightLepton.at(1));
      triggerEff = 1-((1-ef0)*(1-ef1));

      //Corrections for bJet identification:
      //Options: "nom", "upUncorrelated", "upCorrelated", "downUncorrelated", "downCorrelated"
      bjetSF = correctionlib_btagIDSF(Jet, "nom");
      
      wt = lepIdIsoSF*triggerEff;

    }
    
    h.evtweight[0]->Fill(lepIdIsoSF);
    h.evtweight[1]->Fill(triggerEff);
    h.evtweight[2]->Fill(bjetSF);
    h.evtweight[3]->Fill(wt);
    
    //Finding the closest jet and plotting its deepjet score:
    float drmin0 = 1000; float drmin1=1000;
    int closest_Jet_index0=-1; int closest_Jet_index1=-1;
    for(int i=0; i<(int)Jet.size(); i++){
      float dr0temp = LightLepton.at(0).v.DeltaR(Jet.at(i).v);
      float dr1temp = LightLepton.at(1).v.DeltaR(Jet.at(i).v);
      //Leading muon:
      if(dr0temp < drmin0){
	drmin0 = dr0temp;
	closest_Jet_index0 = i;
	LightLepton.at(0).btagscore = Jet.at(i).btagscore;
      }
      else LightLepton.at(0).btagscore = -1;
      //Subleading muon:
      if(dr1temp < drmin1){
	drmin1 = dr1temp;
	closest_Jet_index1 = i;
	LightLepton.at(1).btagscore = Jet.at(i).btagscore;
      }
      else LightLepton.at(1).btagscore = -1;      
    }

    //Event level variabls:
    //Counts:
    UInt_t nlep  = (UInt_t)LightLepton.size();
    UInt_t njet  = (UInt_t)Jet.size();
    UInt_t nbjet = (UInt_t)MediumbJet.size();

    //Object-level variables: First lepton
    Float_t lep0_pt  = (Float_t)LightLepton.at(0).v.Pt();
    Float_t lep0_eta = (Float_t)LightLepton.at(0).v.Eta();
    Float_t lep0_phi = (Float_t)LightLepton.at(0).v.Phi();
    Float_t lep0_iso = (Float_t)LightLepton.at(0).reliso03;
    Float_t lep0_sip3d = (Float_t)LightLepton.at(0).sip3d;
    Float_t lep0_mt    = (Float_t)transv_mass(LightLepton.at(0).v.E(), LightLepton.at(0).v.Phi(), metpt, metphi);
    Float_t lep0_deepjet = (Float_t)LightLepton.at(0).btagscore;
    Float_t lep0_hovere  = (Float_t)LightLepton.at(0).hovere;
    Float_t lep0_r9      = (Float_t)LightLepton.at(0).r9;
    //Second lepton
    Float_t lep1_pt  = (Float_t)LightLepton.at(1).v.Pt();
    Float_t lep1_eta = (Float_t)LightLepton.at(1).v.Eta();
    Float_t lep1_phi = (Float_t)LightLepton.at(1).v.Phi();
    Float_t lep1_iso = (Float_t)LightLepton.at(1).reliso03;
    Float_t lep1_sip3d = (Float_t)LightLepton.at(1).sip3d;
    Float_t lep1_mt    = (Float_t)transv_mass(LightLepton.at(1).v.E(), LightLepton.at(1).v.Phi(), metpt, metphi);
    Float_t lep1_deepjet = (Float_t)LightLepton.at(1).btagscore;
    Float_t lep1_hovere  = (Float_t)LightLepton.at(1).hovere;
    Float_t lep1_r9      = (Float_t)LightLepton.at(1).r9;
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
    Float_t LT= LightLepton.at(0).v.Pt() + LightLepton.at(1).v.Pt();
    Float_t STvis = HT + LT;
    Float_t ST = STvis + metpt;
    Float_t STfrac = 0; if(ST>0) STfrac = LT/ST;
    Float_t HTMETllpt = HT + dilep_pt + metpt;
    Float_t dphi_metlep0 = (Float_t)delta_phi(LightLepton.at(0).v.Phi(), metphi);
    Float_t dphi_metlep1 = (Float_t)delta_phi(LightLepton.at(1).v.Phi(), metphi);
    Float_t dphi_metdilep = (Float_t)delta_phi(dilep.Phi(), metphi);
    Float_t dphi_metlep_max = (Float_t)max(dphi_metlep0, dphi_metlep1);
    Float_t dphi_metlep_min = (Float_t)min(dphi_metlep0, dphi_metlep1);

    //Event selections:
    basic_evt_selection = basic_evt_selection && dilep_mass>15; //syncing with skimmed version

    //The leading lepton is well-isolated:
    //bool baseline = basic_evt_selection && lep0_iso<0.15 && lep0_sip3d<20;
    bool baseline = basic_evt_selection;
    
    //Vetoing DY in case of ee channel:
    bool removed_dy_from_ee = true;
    if(basic_evt_selection == ee){
      if(76 < dilep_mass && dilep_mass < 106)
	removed_dy_from_ee = false;
    }
    baseline = baseline && removed_dy_from_ee;
    
    //Picking a QCD enhanced region:
    bool QCD_enhanced_region = baseline && ST<100 && (0.20<lep1_iso && lep1_iso<0.45);
    bool QCD_CR = QCD_enhanced_region && metphi<0;
    bool QCD_VR = QCD_enhanced_region && metphi>0;
    
    //Working region:
    //bool isoregion = baseline && (lep1_iso<0.20 && lep1_sip3d<30); //Both leptons are isolated
    bool isoregion = baseline && (lep1_iso<0.15 && lep1_sip3d<20); //Both leptons are isolated
    bool highSTiso = isoregion && ST>100; //Outisde the QCD control region
    bool highSTisoClean = highSTiso && fabs(dilep_eta)<5 && STfrac<0.8; 

    //Controlling ttbar:
    bool topEnhanced = highSTisoClean && nbjet>=1;
    bool ttbarCR = highSTisoClean && nbjet>=2;
    bool SR = highSTisoClean && nbjet==0;

    //DY enhanced region for charge misID measurement:

    //---------------------------------------------
    //Final event selection that used in the plots:
    bool event_selection = basic_evt_selection;
    //---------------------------------------------
    
    //------------------------
    // QCD scaling (globally):
    //------------------------
    //double qcdscale = 0.284324926; //Global Scaling
    //if(_flag == "qcd") wt = wt*qcdscale;

    if(_data==1) wt = 1.0;

    if(event_selection){
      h.nevt->Fill(3);
      h.nevt->Fill(4, wt);
      nEvtPass++;
    
      h.evt2LSS[0] ->Fill(nlep, wt);
      h.evt2LSS[1] ->Fill(njet, wt);
      h.evt2LSS[2] ->Fill(nbjet, wt);

      h.evt2LSS[3] ->Fill(lep0_pt, wt);
      h.evt2LSS[4] ->Fill(lep0_eta, wt);
      h.evt2LSS[5] ->Fill(lep0_phi, wt);
      h.evt2LSS[6] ->Fill(lep0_iso, wt);
      h.evt2LSS[7] ->Fill(lep0_sip3d, wt);
      h.evt2LSS[8] ->Fill(lep0_mt, wt);
      h.evt2LSS[9] ->Fill(lep0_deepjet, wt);
      
      h.evt2LSS[10]->Fill(lep1_pt, wt);
      h.evt2LSS[11]->Fill(lep1_eta, wt);
      h.evt2LSS[12]->Fill(lep1_phi, wt);
      h.evt2LSS[13]->Fill(lep1_iso, wt);
      h.evt2LSS[14]->Fill(lep1_sip3d, wt);
      h.evt2LSS[15]->Fill(lep1_mt, wt);
      h.evt2LSS[16]->Fill(lep1_deepjet, wt);

      h.evt2LSS[17]->Fill(dilep_pt, wt);
      h.evt2LSS[18]->Fill(dilep_eta, wt);
      h.evt2LSS[19]->Fill(dilep_phi, wt);
      h.evt2LSS[20]->Fill(dilep_mass, wt);
      h.evt2LSS[21]->Fill(dilep_mt, wt);
      h.evt2LSS[22]->Fill(dilep_deta, wt);
      h.evt2LSS[23]->Fill(dilep_dphi, wt);
      h.evt2LSS[24]->Fill(dilep_dR, wt);
      h.evt2LSS[25]->Fill(dilep_ptratio, wt);

      h.evt2LSS[26]->Fill(HT, wt);
      h.evt2LSS[27]->Fill(LT, wt);
      h.evt2LSS[28]->Fill(STvis, wt);
      h.evt2LSS[29]->Fill(ST, wt);
      h.evt2LSS[30]->Fill(STfrac, wt);
      h.evt2LSS[31]->Fill(HTMETllpt, wt);
      h.evt2LSS[32]->Fill(metpt, wt);
      h.evt2LSS[33]->Fill(metphi, wt);

      h.evt2LSS[34]->Fill(dphi_metlep0, wt);
      h.evt2LSS[35]->Fill(dphi_metlep1, wt);
      h.evt2LSS[36]->Fill(dphi_metdilep, wt);
      h.evt2LSS[37]->Fill(dphi_metlep_max, wt);
      h.evt2LSS[38]->Fill(dphi_metlep_min, wt);

      //Special variables:
      if(fabs(LightLepton.at(0).id) == 11){
	h.evt2LSS[39]->Fill(LightLepton.at(0).hovere, wt);
	h.evt2LSS[40]->Fill(LightLepton.at(0).r9, wt);
      }
      if(fabs(LightLepton.at(1).id) == 11){
	h.evt2LSS[41]->Fill(LightLepton.at(1).hovere, wt);
	h.evt2LSS[42]->Fill(LightLepton.at(1).r9, wt);
      }

      //flavor check:
      for(int i=0; i<(int)LightLepton.size(); i++){
	h.flav[0]->Fill(fabs(LightLepton.at(i).id));
	h.flav[3]->Fill(LightLepton.at(i).id);
      }
      h.flav[1]->Fill(fabs(LightLepton.at(0).id));
      h.flav[2]->Fill(fabs(LightLepton.at(1).id));
      h.flav[4]->Fill(LightLepton.at(0).id);
      h.flav[5]->Fill(LightLepton.at(1).id);

      //Weights check:
      h.evtweight[8] ->Fill(jec);
      h.evtweight[9] ->Fill(jer);
      h.evtweight[10]->Fill(jec*jer);
      h.evtweight[11]->Fill(lepIdIsoSF);
      h.evtweight[12]->Fill(triggerEff);
      h.evtweight[13]->Fill(bjetSF);
      h.evtweight[14]->Fill(wt);

      //Trigger check:
      h.flag[0]->Fill(*HLT_IsoMu24, wt);
      h.flag[1]->Fill(*HLT_IsoMu27, wt);
      h.flag[2]->Fill(*HLT_Ele27_WPTight_Gsf, wt);
      h.flag[3]->Fill(*HLT_Ele32_WPTight_Gsf, wt);
      h.flag[4]->Fill((int)0, wt);
      if(muon_trigger)       h.flag[4]->Fill((int)1, wt);
      if(electron_trigger)   h.flag[4]->Fill((int)2, wt);
      if(overlapping_events) h.flag[4]->Fill((int)3, wt); //Should not appear in EGamma dataset

    }//custom event selection

    //Other calculations:
    
    //#########################
    // ChargeMisId measurement:
    // Specifically, in 2LSS events, what fraction of events comes from Z->LL, where one of the charges flip?
    // 1. Pick a DY enhanced region, and choose a Z window (91-106).
    // 2. Estimate the natural background from the sideband. (55-70 U 110-125)
    // 3. Misidentified events = onZ events - sideband events
    // 4. Fraction of misId events = (onZ events - sideband events)/onZ events
    // 5. Do this in pT bins of the leading lepton, which is usually the prompt lepton.

    bool ptbin0 = lep0_pt < 50;
    bool ptbin1 = 50  <= lep0_pt && lep0_pt < 150;
    bool ptbin2 = 150 <= lep0_pt;
    
    bool onZ      = 91<dilep_mass && dilep_mass<106;
    bool sideband = (55<dilep_mass && dilep_mass<70) || (110<dilep_mass && dilep_mass<125);

    if(event_selection){
      if(ptbin0){
	h.chargeflip[0]->Fill((int)0, wt);
	if(onZ)      h.chargeflip[0]->Fill((int)1, wt);
	if(sideband) h.chargeflip[0]->Fill((int)2, wt);
      }
      else if(ptbin1){
	h.chargeflip[1]->Fill((int)0, wt);
	if(onZ)      h.chargeflip[1]->Fill((int)1, wt);
	if(sideband) h.chargeflip[1]->Fill((int)2, wt);
      }
      else if(ptbin2){
	h.chargeflip[2]->Fill((int)0, wt);
	if(onZ)      h.chargeflip[2]->Fill((int)1, wt);
	if(sideband) h.chargeflip[2]->Fill((int)2, wt);
      }
    }//custom event selection
        
  }//Baseline event selection

}
