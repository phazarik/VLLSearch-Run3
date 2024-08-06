void AnaScript::MakebJetSFPlots(){
  
  bool basic_evt_selection = true;

  //Making sure all leptons are isolated:
  bool all_leptons_isolated = true;
  for(int i=0; i<(int)LightLepton.size(); i++){if(LightLepton.at(i).reliso03 > 0.15) all_leptons_isolated = false;}
  basic_evt_selection = basic_evt_selection && all_leptons_isolated;

  //Picking 2L events:
  bool trigger = false;
  if((int)LightLepton.size()==2){
    //Finding a triggerable object:
    for(int i=0; i<(int)LightLepton.size(); i++){
      if(      fabs(LightLepton.at(i).id) == 11 && LightLepton.at(i).v.Pt() > 32 ) trigger = true;
      else if( fabs(LightLepton.at(i).id) == 13 && LightLepton.at(i).v.Pt() > 26 ) trigger = true;
    }
  }
  basic_evt_selection = basic_evt_selection && trigger;

  //Filling the 2D histograms:
  if(basic_evt_selection && _data==0){

    //Plotting all the candidates:
    for(int i=0; i<(int)Jet.size(); i++){
      
      h.btagsf[0]->Fill(Jet.at(i).hadronflavor);

      //For all the candidates:
      if(Jet.at(i).hadronflavor == 5)      h.bJets[0]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta()); //b-candidates (all, irrespective of whether it passes the b-tag medium WP or not)
      else if(Jet.at(i).hadronflavor == 4) h.cJets[0]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta()); //c-candidates
      else if(Jet.at(i).hadronflavor == 0) h.lJets[0]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta()); //light-candidates

      //For all the candiadtes that pass the b-tag medium threshold:
      bool medium_pass = Jet.at(i).btagscore > 0.2783;

      if(Jet.at(i).hadronflavor == 5 && medium_pass)      h.bJets[1]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta()); //b-tagged b-candidate (true positive)
      else if(Jet.at(i).hadronflavor == 4 && medium_pass) h.cJets[1]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta()); //b-tagged c-candidate (false positive) 
      else if(Jet.at(i).hadronflavor == 0 && medium_pass) h.lJets[1]->Fill(Jet.at(i).v.Pt(), Jet.at(i).v.Eta()); //b-tagged light-candidate (false positive) 

    }

  }

}
