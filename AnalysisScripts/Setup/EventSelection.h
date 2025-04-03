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

  float ptcut_e  = 35;
  float ptcut_mu = 26;
  if(_year == 2017){
    ptcut_e  = 35;
    ptcut_mu = 29;
  }

  /*
  bool all_leptons_isolated = true;
  for(int i=0; i<(int)LightLepton.size(); i++){
    if(LightLepton.at(i).reliso03 > 0.15) all_leptons_isolated = false;
    }*/
  
  // Flagging out the bad events from VLLD:
  if(!bad_event){
  
    //Selecting events in the order 4L>3L>2L
    if((int)LightLepton.size()>3)        evt_4L_incl   = true;
    else if((int)LightLepton.size()==3)  evt_3L        = true;
    else if((int)LightLepton.size()==2){
	if(LightLepton.at(0).charge == (-1)*LightLepton.at(1).charge) evt_2LOS = true;
	else if(LightLepton.at(0).charge == LightLepton.at(1).charge) evt_2LSS = true;
	//else cout<<"2L events error : L1, L2 charge mismatch!"<<endl;
    }
    else if((int)LightLepton.size()==1){
      if ((int)Jet.size()>1)        evt_1L2J_incl = true;
      else if ((int)Jet.size()==1)  evt_1L1J      = true;     
      else if((int)Jet.size()==0)   evt_1L0J      = true;
    }

    // Mimicking the trigger:
    // Note: The triggerring lepton has to be a part of the objects in the event selection.
    // First I am selecting the event, then I am checking for a triggerable object in a For loop.
    // If there is one such object, I am breaking the For loop.

    if(evt_4L_incl){
      //Only Checking the first 4 light leptons.
      for(int i=0; i<4; i++){
	if( fabs(LightLepton.at(i).id) == 11 && LightLepton.at(i).v.Pt() > ptcut_e ) {evt_trigger = true; break;}
	if( fabs(LightLepton.at(i).id) == 13 && LightLepton.at(i).v.Pt() > ptcut_mu ){evt_trigger = true; break;}
      }
      if(evt_trigger) n4l++;
    }

    else if(evt_3L){
      //Checking the 3 light leptons.
      for(int i=0; i<3; i++){
	if( fabs(LightLepton.at(i).id) == 11 && LightLepton.at(i).v.Pt() > ptcut_e ) {evt_trigger = true; break;}
	if( fabs(LightLepton.at(i).id) == 13 && LightLepton.at(i).v.Pt() > ptcut_mu ){evt_trigger = true; break;}
      }
      if(evt_trigger) n3l++;
    }

    else if(evt_2LSS || evt_2LOS){
      //Checking the 2 light leptons.
      for(int i=0; i<2; i++){
	if( fabs(LightLepton.at(i).id) == 11 && LightLepton.at(i).v.Pt() > ptcut_e ) {evt_trigger = true; break;}
	if( fabs(LightLepton.at(i).id) == 13 && LightLepton.at(i).v.Pt() > ptcut_mu ){evt_trigger = true; break;}
      }
    
      if(evt_trigger && evt_2LSS)      n2lss++;
      else if(evt_trigger && evt_2LOS) n2los++;
    }
  
    else if(evt_1L2J_incl || evt_1L1J || evt_1L0J){
      //There is only one light lepton to check.
      if( fabs(LightLepton.at(0).id) == 11 && LightLepton.at(0).v.Pt() > ptcut_e ) {evt_trigger = true;}
      if( fabs(LightLepton.at(0).id) == 13 && LightLepton.at(0).v.Pt() > ptcut_mu ){evt_trigger = true;}
    
      if(evt_trigger && evt_1L2J_incl)      n1l2j++;
      else if(evt_trigger && evt_1L1J)      n1l1j++;
      else if(evt_trigger && evt_1L0J)      n1l0j++;
    }

  }
  
}
