void AnaScript::EventSelection(){
  
  //Defining the final states:
  evt_1L0J      = false;
  evt_1L1J      = false;
  evt_1L2J_incl = false;
  evt_2LOS      = false;
  evt_2LSS      = false;
  evt_3L        = false;
  evt_4L_incl   = false;
  evt_trigger   = false;

  bool all_leptons_isolated = true;
  for(int i=0; i<(int)LightLepton.size(); i++){
    if(LightLepton.at(i).reliso03 > 0.15) all_leptons_isolated = false;
  }

  Float_t HT=0; for(Int_t i=0; i<(Int_t)Jet.size(); i++) HT = HT + Jet.at(i).v.Pt();
  
  //Flagging out the bad events from VLLD:
  if(!bad_event){
  
    //Selecting events in the order 4L>3L>2L
    if((int)LightLepton.size()>3       && all_leptons_isolated)  evt_4L_incl = true;
    else if((int)LightLepton.size()==3 && all_leptons_isolated)  evt_3L        = true;
    else if((int)LightLepton.size()==2 && all_leptons_isolated){

      //Selecting a QCD enhanced region:
      //bool qcd_region = 0.25 < LightLepton.at(0).reliso03 && LightLepton.at(0).reliso03 < 1.0 && LightLepton.at(1).reliso03 > 0.15;
      //qcd_region = qcd_region && HT<500; //Putting a global HT cut

      //Putting a global HT cut
      if(HT<500){
	if(LightLepton.at(0).charge == (-1)*LightLepton.at(1).charge) evt_2LOS = true;
	else evt_2LSS = true;
      }
      
    }
    else if((int)LightLepton.size()==1 && all_leptons_isolated){
      if ((int)Jet.size()>1)        evt_1L2J_incl = true;
      else if ((int)Jet.size()==1)  evt_1L1J      = true;     
      else if((int)Jet.size()==0)   evt_1L0J      = true;
    }

    //Applying trigger:
    //Note: The triggerring lepton has to be a part of the objects in the event selection.
    //First I am selecting the event, then I am applying the trigger.

    if(evt_4L_incl){
      //Only Checking the first 4 light leptons.
      for(int i=0; i<4; i++){
	if( fabs(LightLepton.at(i).id) == 11 && LightLepton.at(i).v.Pt() > 32 ) evt_trigger = true;
	if( fabs(LightLepton.at(i).id) == 13 && LightLepton.at(i).v.Pt() > 26 ) evt_trigger = true;
      }
      if(evt_trigger) n4l++;
    }

    else if(evt_3L){
      //Checking the 3 light leptons.
      for(int i=0; i<3; i++){
	if( fabs(LightLepton.at(i).id) == 11 && LightLepton.at(i).v.Pt() > 32 ) evt_trigger = true;
	if( fabs(LightLepton.at(i).id) == 13 && LightLepton.at(i).v.Pt() > 26 ) evt_trigger = true;
      }
      if(evt_trigger) n3l++;
    }

    else if(evt_2LSS || evt_2LOS){
      //Checking the 2 light leptons.
      for(int i=0; i<2; i++){
	if( fabs(LightLepton.at(i).id) == 11 && LightLepton.at(i).v.Pt() > 32 ) evt_trigger = true;
	if( fabs(LightLepton.at(i).id) == 13 && LightLepton.at(i).v.Pt() > 26 ) evt_trigger = true;
      }
    
      if(evt_trigger && evt_2LSS)      n2lss++;
      else if(evt_trigger && evt_2LOS) n2los++;
    }
  
    else if(evt_1L2J_incl || evt_1L1J || evt_1L0J){
      //There is only one light lepton to check.
      if( fabs(LightLepton.at(0).id) == 11 && LightLepton.at(0).v.Pt() > 32 ) evt_trigger = true;
      if( fabs(LightLepton.at(0).id) == 13 && LightLepton.at(0).v.Pt() > 26 ) evt_trigger = true;
    
      if(evt_trigger && evt_1L2J_incl)      n1l2j++;
      else if(evt_trigger && evt_1L1J)      n1l1j++;
      else if(evt_trigger && evt_1L0J)      n1l0j++;
    }

  }
  
}
