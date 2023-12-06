void AnaScript::EventSelection(){

  evt_1L0J      = false;
  evt_1L1J      = false;
  evt_1L2J_incl = false;
  evt_2LOS      = false;
  evt_2LSS      = false;
  evt_3L        = false;
  evt_4L_incl   = false;

  
  if((int)LightLepton.size()>0){
    
    //Trigger:
    bool evt_trigger = false;
    if(abs(LightLepton.at(0).id)==13      && LightLepton.at(0).v.Pt()>26) evt_trigger=true; //Muon > 26
    else if(abs(LightLepton.at(0).id)==11 && LightLepton.at(0).v.Pt()>32) evt_trigger=true; //Electron > 32
    
    if(evt_trigger){

      //Selecting events in the order 4L>3L>2L
      if((int)LightLepton.size()>3)        evt_4L_incl   = true;
      else if((int)LightLepton.size()==3)  evt_3L        = true;
      else if((int)LightLepton.size()==2){
	if(LightLepton.at(0).charge == (-1)*LightLepton.at(1).charge) evt_2LOS = true;
	else evt_2LSS = true;
      }
      else if((int)LightLepton.size()==1){
	if ((int)Jet.size()>1)        evt_1L2J_incl = true;
	else if ((int)Jet.size()==1)  evt_1L1J      = true;     
	else if((int)Jet.size()==0)   evt_1L0J      = true;
      }

      //Counters:
      if(evt_4L_incl)        n4l++;
      else if(evt_3L)        n3l++;
      else if(evt_2LSS)      n2lss++;
      else if(evt_2LOS)      n2los++;
      else if(evt_1L2J_incl) n1l2j++;
      else if(evt_1L1J)      n1l1j++;
      else if(evt_1L0J)      n1l0j++;
      
    }//trigger

  }//llep>0

}
