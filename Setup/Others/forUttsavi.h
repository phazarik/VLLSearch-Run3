void AnaScript::MakePlotsForUttsavi(){

  h.hist[13]->Fill(0); //This is counting all events in the input file that pass the GoodEvt filter.;
  
  //Step1:
  //Creating the arrays:
  //This is done here - "../ProduceRecoCollection.h".

  /*
  The functions for creating the reco objects is called in the main script.
  Muons have pT>10, eta<2.4, mediumID, reliso04<0.15
  Jets have pT>30, eta<2.4, dRcleaned from muons by 0.4, jetID is not used.
  ForwardJets have have pT>30, eta<4.7, dRcleaned from muons by 0.4, jetID is not used.
  Medium bJets have Jet_btagDeepFlavB[i]>0.2783
  */

  /*
  Note to self:
  Once this is done, I need to -
  1. Set Muon reliso back to 03
  2. Clean the jets from all leptons and use jetID
  */
  
  //Step2:
  //Sorting the new arrays:
  for(int i=0; i<(int)ForwardJet.size()-1; i++){
    for(int j=i+1; j<(int)ForwardJet.size(); j++){
      if(ForwardJet[i].v.Pt() < ForwardJet[j].v.Pt() ) swap(ForwardJet.at(i), ForwardJet.at(j));}}
  for(int i=0; i<(int)MediumbJet.size()-1; i++){
    for(int j=i+1; j<(int)MediumbJet.size(); j++){
      if(MediumbJet[i].v.Pt() < MediumbJet[j].v.Pt() ) swap(MediumbJet.at(i), MediumbJet.at(j));}}
  for(int i=0; i<(int)ForwardMediumbJet.size()-1; i++){
    for(int j=i+1; j<(int)ForwardMediumbJet.size(); j++){
      if(ForwardMediumbJet[i].v.Pt() < ForwardMediumbJet[j].v.Pt() ) swap(ForwardMediumbJet.at(i), ForwardMediumbJet.at(j));}}
  //(Others are pT-sorted in the Sort() function)
  
  //Step3:
  //Event selection
  //First, let's pick the events with exactly one muon and calculate the event weight:
  if((int)Muon.size()==1 && Muon.at(0).v.Pt()>26){
    
    double scalefactor = Muon_2018UL_Reco(Muon.at(0).v.Pt(), Muon.at(0).v.Eta());
    double triggereff  = TrigEff_2018_IsoMu24_Data(Muon.at(0).v.Pt(), Muon.at(0).v.Eta());
    double eventweight = scalefactor*triggereff;
      
    //I am selecting 1mu2J inclusive (at least one Jet AND at least one medium-bJet)
    //for two cases: with regular jets, and with forward jets.
    bool evt_regular = (int)Jet.size()>1 && (int)MediumbJet.size()>0;
    bool evt_forward = (int)ForwardJet.size()>1 && (int)ForwardMediumbJet.size()>0;

    //Let's count events for the first case.
    if(evt_regular){

      //counting events that pass the basic selection:
      nbasicpass++;
      h.hist[13]->Fill((int)1, eventweight);

      //Defining event level variables:
      float HT=0; for(int i=0; i<(int)Jet.size(); i++) HT = HT + Jet.at(i).v.Pt();
      float dR_mujet0 = Muon.at(0).v.DeltaR(Jet.at(0).v);
      float sumbscore = Jet.at(0).btagscore + Jet.at(1).btagscore;
      float HT70 = 0;
      if(Jet.at(0).v.Pt()>70){
	for(int i=0; i<(int)Jet.size(); i++) HT70 = HT70 + Jet.at(i).v.Pt();
      }//HT70 is the sum of all Jet pT, when the leading Jet has pT>70

      //Putting cuts on these event-level variables:
      bool event_selection = (HT>150 &&
			      dR_mujet0 < 3.2 &&
			      sumbscore > 1.0 &&
			      HT70 > 20);

      //counting events that pass the advanced selection:
      if(event_selection){
	nadvancedpass++;
	h.hist[13]->Fill((int)2, eventweight);
      }
    }

    //Repeating the previous steps, with the forward jets.
    if(evt_forward){

      nbasicpass2++;
      h.hist[13]->Fill((int)3, eventweight);

      float regularmetpt = *MET_pt;
      float regularmetphi = *MET_phi;
      float mT = transv_mass(Muon.at(0).v.E(), Muon.at(0).v.Phi(), regularmetpt, regularmetphi);
      bool event_selection = mT>50;

      if(event_selection){
	nadvancedpass2++;
	h.hist[13]->Fill((int)4, eventweight);
      }
      
    }
    
  }

}
