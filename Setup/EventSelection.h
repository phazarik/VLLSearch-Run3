void AnaScript::EventSelection(){

  evt_2L_exclusive = false;
  evt_3L_exclusive = false;
  evt_4L_inclusive = false;
  
  if((int)LightLepton.size()==2){
    n2l++;
    evt_2L_exclusive = true;
  }

  else if((int)LightLepton.size()==3){
    n3l++;
    evt_3L_exclusive = true;
  }

  else if((int)LightLepton.size()>=4){
    n4l++;
    evt_4L_inclusive = true;
  }

}
