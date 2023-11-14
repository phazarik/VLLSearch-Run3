void AnaScript::EventSelection(){

  evt_1L0J      = false;
  evt_1L1J      = false;
  evt_1L2J_incl = false;
  evt_2LOS      = false;
  evt_2LSS      = false;
  evt_3L        = false;
  evt_4L_incl   = false;

  /*
  if((int)LightLepton.size()==1){
    if((int)Jet.size()==0)            evt_1L0J      = true;
    else if ((int)Jet.size()==1)      evt_1L1J      = true;
    else if ((int)Jet.size()<=2)      evt_1L2J_incl = true;
  }
  else if((int)LightLepton.size()==2){
    if(LightLepton.at(0).charge != LightLepton.at(1).charge) evt_2LOS = true;
    else evt_2LSS = true;
  }
  else if((int)LightLepton.size()==3) evt_3L        = true;
  else if((int)LightLepton.size()>=4) evt_4L_incl   = true;*/

  //Selecting events in the order 4L>3L>2L
  if((int)LightLepton.size()>=4)      evt_4L_incl   = true;
  else if((int)LightLepton.size()==3) evt_3L        = true;
  else if((int)LightLepton.size()==2){
    if(LightLepton.at(0).charge != LightLepton.at(1).charge) evt_2LOS = true;
    else evt_2LSS = true;
  }
  else if((int)LightLepton.size()==1){
    if((int)Jet.size()==0)            evt_1L0J      = true;
    else if ((int)Jet.size()==1)      evt_1L1J      = true;
    else if ((int)Jet.size()<=2)      evt_1L2J_incl = true;
  }

}
