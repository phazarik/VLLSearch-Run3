//This function is called in the main C file.
void AnaScript::MakebJetSFPlots(){
  
    // This function sets values to the variables that goes into the tree.
  bool basic_evt_selection = false;
  bool ee = false;
  bool em = false;
  bool me = false;
  bool mm = false;
  
  //Offline cuts on the leptons:
  float ptcut_mu  = 26;
  float ptcut_ele = 35; 
  if(_year==2016) {ptcut_ele = 30; ptcut_mu = 26;}
  if(_year==2017) {ptcut_ele = 37; ptcut_mu = 29;}

  bool evt_2LSS = false;
  bool evt_2LOS = false;

  //Baseline selection
  if((int)LightLepton.size()==2){

    //Check offline trigger:
    bool trigger = false;
    for(int i=0; i<(int)LightLepton.size(); i++){
      int lepton_id = fabs(LightLepton.at(i).id);
      float lepton_pt = LightLepton.at(i).v.Pt();
      if(lepton_id == 11 && lepton_pt > ptcut_ele) trigger = true;
      if(lepton_id == 13 && lepton_pt > ptcut_mu)  trigger = true;
    }    
    if(LooseLepton.size() > 2) trigger = false; //Veto additional loose leptons:

    //Check resonance-cut:
    bool reject_low_resonances = (LightLepton.at(0).v + LightLepton.at(1).v).M() > 15;
    bool reject_most_resonances = (LightLepton.at(0).v + LightLepton.at(1).v).M() > 150;

    //Check lepton charges:
    bool samesign = LightLepton.at(0).charge == LightLepton.at(1).charge;

    //Define events:
    if(trigger && reject_low_resonances && samesign)   evt_2LSS = true; //2LSS
    if(trigger && reject_most_resonances && !samesign) evt_2LOS = true; //2LOS

    //Veto additional events
    bool veto_3L4L_event = Veto3L4L();
    bool veto_HEM_event  = VetoHEM(Jet);
    bool veto_this_event = veto_3L4L_event || veto_HEM_event;
    if(veto_this_event){
      nEvtVeto++;
      evt_2LSS = false;
      evt_2LOS = false;
      return;
    }
    
    if(evt_2LSS){ //IMPORTANT: SELECT ONLY ONE FINAL STATE
      
      int flav0 = fabs(LightLepton.at(0).id);
      int flav1 = fabs(LightLepton.at(1).id);
      
      if(     flav0 == 13 && flav1 == 13){ mm = true; }
      else if(flav0 == 13 && flav1 == 11){ me = true; }
      else if(flav0 == 11 && flav1 == 13){ em = true; }
      else if(flav0 == 11 && flav1 == 11){ ee = true; }
    }
  }
  
  //-------------------------------------------
  // Select the channel :
  basic_evt_selection = mm || me || em || ee ;
  //-------------------------------------------
  
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
