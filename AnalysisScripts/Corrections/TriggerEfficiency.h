//###################################################################################
//                        Trigger Efficiency scale factors
//###################################################################################

double AnaScript::GetLeptonTriggerEfficiency(Particle lepton){
  
  //This is the main function which is called in the analysis script

  double sf = 1.0; 
  int flav = fabs(lepton.id);

  // Method 1:
  // Directly applying the trigger efficiency measured in data on MC (The HLT flags are not used in MC) 
  
  if(flav == 13)      sf = TrigEFF_allCampaign_Isomu24_Data(lepton);
  else if(flav == 11) sf = TrigEFF_allCampaign_Ele27or32WPTightGSF_Data(lepton);

  /*
  //Method 2:
  //Using the HLT flags in both data and MC, and then calculating a scale factor from the trigger efficiency ratio
  if(flav == 13)      sf = TrigEFF_allCampaign_Isomu24_Data(lepton) / TrigEFF_allCampaign_Isomu24_MC(lepton);
  else if(flav == 11) sf = TrigEFF_allCampaign_Ele27or32WPTightGSF_Data(lepton) / TrigEFF_allCampaign_Ele27or32WPTightGSF_MC(lepton);*/
  
  else cout<<"Warning: Provide correct lepton while calculating trigger sf."<<endl;

  return sf;
  
}

//--------------------
// SingleMuon trigger
//--------------------
//__________________________________________

double AnaScript::TrigEFF_allCampaign_Isomu24_MC(Particle muon){

  double eff = 0.0;
  
  float pt  = muon.v.Pt();
  float eta = fabs(muon.v.Eta());

  if(_campaign == "2016preVFP_UL"){
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479) eff = 0.5*0.971934*(1.0+TMath::Erf((pt-23.4424)/(2.0*0.178376))); //barrel
    else if(fabs(eta)>1.479)  eff = 0.5*0.966306*(1.0+TMath::Erf((pt-23.9592)/(2.0*0.487943))); //endcap
    return eff;
  }
  else if(_campaign == "2016postVFP_UL"){
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479) eff = 0.5*0.974532*(1.0+TMath::Erf((pt-23.3954)/(2.0*0.151737)));
    else if(fabs(eta)>1.479)  eff = 0.5*0.970343*(1.0+TMath::Erf((pt-23.977)/(2.0*0.445236)));
    return eff;
  }
  else if(_campaign == "2017_UL"){
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479) eff = 0.5*0.960212*(1.0+TMath::Erf((pt-26.9652)/(2.0*0.597564))); 
    else if(fabs(eta)>1.479)  eff = 0.5*0.94303*(1.0+TMath::Erf((pt-26.9123)/(2.0*0.747516)));
    return eff;
  }
  else if(_campaign == "2018_UL"){
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479) eff = 0.5*0.971878*(1.0+TMath::Erf((pt-23.9589)/(2.0*0.356627)));
    else if(fabs(eta)>1.479)  eff = 0.5*0.958908*(1.0+TMath::Erf((pt-23.9493)/(2.0*0.408312)));
    return eff;
  }
  else if(_campaign == "Run3Summer22"){
    return 1.0;
  }
  else{
    cout<<"Warning: Give proper campaign name."<<endl;
    return 1.0;
  }
}

double AnaScript::TrigEFF_allCampaign_Isomu24_Data(Particle muon){

  double eff = 0.0;
  
  float pt  = muon.v.Pt();
  float eta = fabs(muon.v.Eta());

  if(_campaign == "2016preVFP_UL"){
    float eff = 0.0; //default value
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479) eff = 0.5*0.952501*(1.0+TMath::Erf((pt-23.9491)/(2.0*0.405232))); //barrel
    else if(fabs(eta)>1.479){
      if(pt<=100)             eff = 0.5*0.951936*(1.0+TMath::Erf((pt-23.9399)/(2.0*0.533316)));
      else                    eff = 0.774;
    }
    return eff;
  }
  else if(_campaign == "2016postVFP_UL"){
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479) eff = 0.5*0.963135*(1.0+TMath::Erf((pt-23.9546)/(2.0*0.363316)));
    else if(fabs(eta)>1.479)  eff = 0.5*0.96043*(1.0+TMath::Erf((pt-23.9677)/(2.0*0.463984)));
    return eff;
  }
  else if(_campaign == "2017_UL"){
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479){
      if(pt<=100)            eff = 0.5*0.929318*(1.0+TMath::Erf((pt-26.9424)/(2.0*0.612448)));
      else                   eff = 0.958305;
    }
    else if(fabs(eta)>1.479) eff = 0.5*0.94163*(1.0+TMath::Erf((pt-26.9092)/(2.0*0.844543)));
    return eff;
  }
  else if(_campaign == "2018_UL"){
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479) eff = 0.5*0.950463*(1.0+TMath::Erf((pt-23.9593)/(2.0*0.375996)));
    else if(fabs(eta)>1.479)  eff = 0.5*0.953162*(1.0+TMath::Erf((pt-23.9459)/(2.0*0.457351)));
    return eff;
  }
  else if(_campaign == "Run3Summer22"){
    return 1.0;
  }
  else{
    cout<<"Warning: Give proper campaign name."<<endl;
    return 1.0;
  }
}


//------------------------
// SingleElectron trigger
//------------------------

double AnaScript::TrigEFF_allCampaign_Ele27or32WPTightGSF_MC(Particle electron){

  double eff = 0.0;
  
  float pt  = electron.v.Pt();
  float eta = fabs(electron.v.Eta());

  if(_campaign == "2016preVFP_UL"){
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479) eff = 0.5*0.984513*(1.0+TMath::Erf((pt-27.5914)/(2.0*1.0585)));
    else if(fabs(eta)>1.479)  eff = 0.5*0.989446*(1.0+TMath::Erf((pt-26.0586)/(2.0*1.85888)));
    return eff;
  }
  else if(_campaign == "2016postVFP_UL"){
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479) eff = 0.5*0.983934*(1.0+TMath::Erf((pt-27.5947)/(2.0*1.06344)));
    else if(fabs(eta)>1.479)  eff = 0.5*0.988555*(1.0+TMath::Erf((pt-26.0556)/(2.0*1.85987)));
    return eff;
  }
  else if(_campaign == "2017_UL"){
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479) eff = 0.5*0.96683*(1.0+TMath::Erf((pt-31.6521)/(2.0*1.16952)));
    else if(fabs(eta)>1.479)  eff = 0.5*0.977357*(1.0+TMath::Erf((pt-32.7302)/(2.0*1.98741)));
    return eff;
  }
  else if(_campaign == "2018_UL"){
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479) eff = 0.5*0.971841*(1.0+TMath::Erf((pt-33.0664)/(2.0*0.830281)));
    else if(fabs(eta)>1.479)  eff = 0.5*0.981582*(1.0+TMath::Erf((pt-33.9066)/(2.0*1.55882)));
    return eff;
  }
  else if(_campaign == "Run3Summer22"){
    return 1.0;
  }
  else{
    cout<<"Warning: Give proper campaign name."<<endl;
    return 1.0;
  }
}

double AnaScript::TrigEFF_allCampaign_Ele27or32WPTightGSF_Data(Particle electron){

  double eff = 0.0;
  
  float pt  = electron.v.Pt();
  float eta = fabs(electron.v.Eta());

  if(_campaign == "2016preVFP_UL"){
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479) eff = 0.5*0.980431*(1.0+TMath::Erf((pt-26.6311)/(2.0*0.977291)));
    else if(fabs(eta)>1.479)  eff = 0.5*0.984774*(1.0+TMath::Erf((pt-27.5187)/(2.0*1.33533)));
    return eff;
  }
  else if(_campaign == "2016postVFP_UL"){
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479) eff = 0.5*0.975241*(1.0+TMath::Erf((pt-26.6834)/(2.0*1.08336)));
    else if(fabs(eta)>1.479)  eff = 0.5*0.981091*(1.0+TMath::Erf((pt-27.7339)/(2.0*1.44615)));
    return eff;
  }
  else if(_campaign == "2017_UL"){
    if( pt<10 || eta>2.4 ) return 0.0;
    else if(fabs(eta)<=1.479) eff = 0.5*0.955945*(1.0+TMath::Erf((pt-33.0345)/(2.0*0.885676)));
    else if(fabs(eta)>1.479)  eff = 0.5*0.962208*(1.0+TMath::Erf((pt-33.9927)/(2.0*1.55814)));
    return eff;
  }
  /*
  else if(_campaign == "2018_UL"){ 
    if(eta<=1.479)     eff = 0.5*0.950463*(1.0+TMath::Erf((pt-23.9593)/(2.0*0.375996)));
    else if(eta>1.479) eff = 0.5*0.953162*(1.0+TMath::Erf((pt-23.9459)/(2.0*0.457351)));
    return eff; //Taken from Yash on 05-06-2024 //HLT_Ele32_WPTight_Gsf
    }*/
  else if(_campaign == "2018_UL"){ 
    if(eta<=1.479)     eff = 0.5*0.962897*(1.0+TMath::Erf((pt-33.1188)/(2.0*0.844886)));
    else if(eta>1.479) eff = 0.5*0.975043*(1.0+TMath::Erf((pt-32.9805)/(2.0*1.18094)));
    return eff;
  }
  else if(_campaign == "Run3Summer22"){
    return 1.0;
  }
  else{
    cout<<"Warning: Give proper campaign name."<<endl;
    return 1.0;
  }
}
