void AnaScript::MakebJetSFPlots(){

  //This function is intended to run for 2LSS events which pass a trigger.
  //This function is called in the main C file.
  
  //##################################################################################################
  //Corrections to Jets:
  float jec = 1.0;
  float jer = 1.0;
  if(_data == 0){
    for(int i=0; i<(int)Jet.size(); i++){
      
      jec = correctionlib_jetSF(Jet.at(i),"nom");
      float rho = 0.0;
      if(ptr_fixedGridRhoFastjetAll != nullptr) rho = **ptr_fixedGridRhoFastjetAll;
      //cout<<"Test: rho= "<<rho<<endl;
      //------------------------------------  Note  -----------------------------------------
      //ptr_fixedGridRhoFastjetAll is a pointer to a TTreeReaderValue<Float_t>.
      //*ptr_fixedGridRhoFastjetAll gives you the TTreeReaderValue<Float_t> object.
      //**ptr_fixedGridRhoFastjetAll gives you the actual float value stored in that object.
      jer = correctionlib_jetRF(Jet.at(0),genJet,rho,"nom");
      //-------------------------------------------------------------------------------------
      //jer = correctionlib_jetRF(Jet.at(0),genJet,1.0,"nom");
      
      //Jet Energy Correction:
      Jet.at(i).v = Jet.at(i).v * jec;
      Jet.at(i).v = Jet.at(i).v * jer;
    }
  }
  //##################################################################################################
  //Making sure all leptons are isolated:
  /*
  bool all_leptons_isolated = true;
  for(int i=0; i<(int)LightLepton.size(); i++){if(LightLepton.at(i).reliso03 > 0.15) all_leptons_isolated = false;}
  basic_evt_selection = basic_evt_selection && all_leptons_isolated;*/

  //Picking 2LSS events
  bool basic_evt_selection = false;
  bool ee = false;
  bool em = false;
  bool me = false;
  bool mm = false;

  //Offline cuts on the leptons:
  float ptcut_mu  = 26; if(_campaign=="2017_UL") ptcut_mu  = 32;
  float ptcut_ele = 35; if(_campaign=="2017_UL") ptcut_ele = 37;
  
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
	else if(flav0 == 13 && flav1 == 11){ nemuss++; me = true; }
	else if(flav0 == 11 && flav1 == 13){ nemuss++; em = true; }
	else if(flav0 == 11 && flav1 == 11){ n2ess++;  ee = true; }
      }
    }
  }

  basic_evt_selection = ee || em || me || mm;

  if(basic_evt_selection && _data==0){

    nEvtPass++;
    
    //Plotting all the candidates:
    for(int i=0; i<(int)Jet.size(); i++){
      
      h.btagsf[0]->Fill(Jet.at(i).hadronflavor);
      
      //For all the candidates:
      if(Jet.at(i).hadronflavor == 5)      h.bJets[0]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta()); //b-candidates (all, irrespective of whether it passes the b-tag medium WP or not)
      else if(Jet.at(i).hadronflavor == 4) h.cJets[0]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta()); //c-candidates
      else if(Jet.at(i).hadronflavor == 0) h.lJets[0]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta()); //light-candidates

      //For all the candiadtes that pass the b-tag medium threshold:
      float WPth = 0.;
      if(_campaign =="2018_UL")              WPth=0.2783;
      else if (_campaign =="2017_UL")        WPth=0.3040;
      else if (_campaign =="2016preVFP_UL")  WPth=0.2598;
      else if (_campaign =="2016postVFP_UL") WPth=0.2489;
      else cout<<"bJetScaleFactorCalculator.h : Provide correct campaign name!"<<endl;
      bool medium_pass = Jet.at(i).btagscore > WPth;

      if(Jet.at(i).hadronflavor == 5 && medium_pass)      h.bJets[1]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta()); //b-tagged b-candidate (true positive)
      else if(Jet.at(i).hadronflavor == 4 && medium_pass) h.cJets[1]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta()); //b-tagged c-candidate (false positive) 
      else if(Jet.at(i).hadronflavor == 0 && medium_pass) h.lJets[1]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta()); //b-tagged light-candidate (false positive) 

    }
  }
}
