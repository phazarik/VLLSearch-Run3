//This function is called in the main C file.
void AnaScript::MakebJetSFPlots(){
  
  //Picking 2LSS events
  bool basic_evt_selection = false;
  bool ee = false;
  bool em = false;
  bool me = false;
  bool mm = false;

  //Offline cuts on the leptons:
  float ptcut_mu  = 26; if(_year==2017) ptcut_mu  = 29;
  float ptcut_ele = 35; if(_year==2017) ptcut_ele = 37; if(_year==2016) ptcut_ele = 30;
  
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
	if(     flav0 == 13 && flav1 == 13) mm = true;
	else if(flav0 == 13 && flav1 == 11) me = true;
	else if(flav0 == 11 && flav1 == 13) em = true;
	else if(flav0 == 11 && flav1 == 11) ee = true;
      }
    }
  }

  basic_evt_selection = ee || em || me || mm;

  //Making sure all leptons are isolated:
  bool all_leptons_isolated = true;
  for(int i=0; i<(int)LightLepton.size(); i++){if(LightLepton.at(i).reliso03 > 0.15) all_leptons_isolated = false;}
  basic_evt_selection = basic_evt_selection && all_leptons_isolated;
  
  if(basic_evt_selection && _data==0){

    bool veto_3L4L_event = Veto3L4L();
    bool veto_HEM_event  = VetoHEM(Jet);
    bool veto_this_event = veto_3L4L_event || veto_HEM_event;
    if(veto_this_event){
      nEvtVeto++;
      return;
    }

    nEvtPass++;
    
    //Plotting all the candidates:
    for(int i=0; i<(int)Jet.size(); i++){
      
      h.hist[0]->Fill(Jet.at(i).hadronflavor);

      //-------------------------------------
      // For all the candidates: (denominator)
      //-------------------------------------
      if(Jet.at(i).hadronflavor == 5)      h.bJets[0]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta());
      else if(Jet.at(i).hadronflavor == 4) h.cJets[0]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta());
      else if(Jet.at(i).hadronflavor == 0) h.lJets[0]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta());

      float WPth = _btagWP;
      bool medium_pass = Jet.at(i).btagscore > WPth;

      //------------------------------------------------------------
      // For all the candidates that pass the threshold: (numerator)
      //------------------------------------------------------------
      if(medium_pass){
	if(Jet.at(i).hadronflavor == 5)      h.bJets[1]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta());
	else if(Jet.at(i).hadronflavor == 4) h.cJets[1]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta());
	else if(Jet.at(i).hadronflavor == 0) h.lJets[1]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta());
      }
            
    }//Jet loop
  }//Baseline selection
}//Function end
