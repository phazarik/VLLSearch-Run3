//#################################################################################################
// Scale-Factors from POG:
// These numbers are multiplied to jet_eff to calculate jet_prob_data
// The final probs for the jets is given by product of prob_data and prob_mc for each jet.
// The scale factor for the event is prob_data / prob_mc (this is an event level correction to MC)
//#################################################################################################

//##############
// 2016-preVFP
//##############

double AnaScript::getScaleFactors_bTagJets_MedWP_UL2016preVFP(float eta, float pt, double tweak){
  double scale_factor = 1.0;
  if(fabs(eta)<0.8){
    if( 20<pt && pt<30)    scale_factor =  0.9973199796684669;
    if( 30<pt && pt<50)    scale_factor =  0.9960854563465826;
    if( 50<pt && pt<70)    scale_factor =  0.9944348404862913;
    if( 70<pt && pt<100)   scale_factor =  0.9923641670362914;
    if( 100<pt && pt<140)  scale_factor =  0.989451315699134;
    if( 140<pt && pt<200)  scale_factor =  0.9869415777702947;
    if( 200<pt && pt<300)  scale_factor =  0.9784879070520077;
    if( 300<pt && pt<600)  scale_factor =  0.9655481022208018;
    if( 600<pt && pt<1000) scale_factor =  0.92943712325938; 
  }
  else if(0.8<fabs(eta) && fabs(eta)<1.6){
    if( 20<pt && pt<30)    scale_factor =  0.9973199796684669;
    if( 30<pt && pt<50)    scale_factor =  0.9960854563465826;
    if( 50<pt && pt<70)    scale_factor =  0.9944348404862913;
    if( 70<pt && pt<100)   scale_factor =  0.9923641670362914;
    if( 100<pt && pt<140)  scale_factor =  0.989451315699134;
    if( 140<pt && pt<200)  scale_factor =  0.9869415777702947;
    if( 200<pt && pt<300)  scale_factor =  0.9784879070520077;
    if( 300<pt && pt<600)  scale_factor =  0.9655481022208018;
    if( 600<pt && pt<1000) scale_factor =  0.92943712325938;
  }
  else if(1.6<fabs(eta) && fabs(eta)<2.5){
    if( 20<pt && pt<30)    scale_factor =  0.9973199796684669;
    if( 30<pt && pt<50)    scale_factor =  0.9960854563465826;
    if( 50<pt && pt<70)    scale_factor =  0.9944348404862913;
    if( 70<pt && pt<100)   scale_factor =  0.9923641670362914;
    if( 100<pt && pt<140)  scale_factor =  0.989451315699134;
    if( 140<pt && pt<200)  scale_factor =  0.9869415777702947;
    if( 200<pt && pt<300)  scale_factor =  0.9784879070520077;
    if( 300<pt && pt<600)  scale_factor =  0.9655481022208018;
    if( 600<pt && pt<1000) scale_factor =  0.92943712325938;
  }
  scale_factor = scale_factor*(1+tweak); //tweak is some % up or down (for calculating systematic uncertainty)
  return scale_factor;
}
double AnaScript::getScaleFactors_cTagJets_Mis_UL2016preVFP(float eta, float pt, double tweak){
  double scale_factor = 1.0;
  if(fabs(eta)<0.8){
    if( 20<pt && pt<30)    scale_factor =  0.9973199796684669;
    if( 30<pt && pt<50)    scale_factor =  0.9960854563465826;
    if( 50<pt && pt<70)    scale_factor =  0.9944348404862913;
    if( 70<pt && pt<100)   scale_factor =  0.9923641670362914;
    if( 100<pt && pt<140)  scale_factor =  0.989451315699134;
    if( 140<pt && pt<200)  scale_factor =  0.9869415777702947;
    if( 200<pt && pt<300)  scale_factor =  0.9784879070520077;
    if( 300<pt && pt<600)  scale_factor =  0.9655481022208018;
    if( 600<pt && pt<1000) scale_factor =  0.92943712325938; 
  }
  else if(0.8<fabs(eta) && fabs(eta)<1.6){
    if( 20<pt && pt<30)    scale_factor =  0.9973199796684669;
    if( 30<pt && pt<50)    scale_factor =  0.9960854563465826;
    if( 50<pt && pt<70)    scale_factor =  0.9944348404862913;
    if( 70<pt && pt<100)   scale_factor =  0.9923641670362914;
    if( 100<pt && pt<140)  scale_factor =  0.989451315699134;
    if( 140<pt && pt<200)  scale_factor =  0.9869415777702947;
    if( 200<pt && pt<300)  scale_factor =  0.9784879070520077;
    if( 300<pt && pt<600)  scale_factor =  0.9655481022208018;
    if( 600<pt && pt<1000) scale_factor =  0.92943712325938;
  }
  else if(1.6<fabs(eta) && fabs(eta)<2.5){
    if( 20<pt && pt<30)    scale_factor =  0.9973199796684669;
    if( 30<pt && pt<50)    scale_factor =  0.9960854563465826;
    if( 50<pt && pt<70)    scale_factor =  0.9944348404862913;
    if( 70<pt && pt<100)   scale_factor =  0.9923641670362914;
    if( 100<pt && pt<140)  scale_factor =  0.989451315699134;
    if( 140<pt && pt<200)  scale_factor =  0.9869415777702947;
    if( 200<pt && pt<300)  scale_factor =  0.9784879070520077;
    if( 300<pt && pt<600)  scale_factor =  0.9655481022208018;
    if( 600<pt && pt<1000) scale_factor =  0.92943712325938;
  }
  scale_factor = scale_factor*(1+tweak);
  return scale_factor;
}

double AnaScript::getScaleFactors_LightTagJets_Mis_UL2016preVFP(float eta, float pt, double tweak){
  double scale_factor = 1.0;
  if(fabs(eta)<0.8){
    if( 20<pt && pt<30)    scale_factor =  1.4415005474437501;
    if( 30<pt && pt<50)    scale_factor =  1.4084120494560002;
    if( 50<pt && pt<70)    scale_factor =  1.3927668012760002;
    if( 70<pt && pt<100)   scale_factor =  1.386676428214456;
    if( 100<pt && pt<140)  scale_factor =  1.386541965104;
    if( 140<pt && pt<200)  scale_factor =  1.3895547079750001;
    if( 200<pt && pt<300)  scale_factor =  1.4390376456;
    if( 300<pt && pt<600)  scale_factor =  1.4390376456;
    if( 600<pt && pt<1000) scale_factor =  1.5338593324000003; 
  }
  else if(0.8<fabs(eta) && fabs(eta)<1.6){
    if( 20<pt && pt<30)    scale_factor =  1.4415005474437501;
    if( 30<pt && pt<50)    scale_factor =  1.4084120494560002;
    if( 50<pt && pt<70)    scale_factor =  1.3927668012760002;
    if( 70<pt && pt<100)   scale_factor =  1.386676428214456;
    if( 100<pt && pt<140)  scale_factor =  1.386541965104;
    if( 140<pt && pt<200)  scale_factor =  1.3895547079750001;
    if( 200<pt && pt<300)  scale_factor =  1.4390376456;
    if( 300<pt && pt<600)  scale_factor =  1.4390376456;
    if( 600<pt && pt<1000) scale_factor =  1.5338593324000003; 
  }
  else if(1.6<fabs(eta) && fabs(eta)<2.5){
    if( 20<pt && pt<30)    scale_factor =  1.4415005474437501;
    if( 30<pt && pt<50)    scale_factor =  1.4084120494560002;
    if( 50<pt && pt<70)    scale_factor =  1.3927668012760002;
    if( 70<pt && pt<100)   scale_factor =  1.386676428214456;
    if( 100<pt && pt<140)  scale_factor =  1.386541965104;
    if( 140<pt && pt<200)  scale_factor =  1.3895547079750001;
    if( 200<pt && pt<300)  scale_factor =  1.4390376456;
    if( 300<pt && pt<600)  scale_factor =  1.4390376456;
    if( 600<pt && pt<1000) scale_factor =  1.5338593324000003;
  }
  scale_factor = scale_factor*(1+tweak);
  return scale_factor;
}

//###############
// 2016-postVFP
//###############

double AnaScript::getScaleFactors_bTagJets_MedWP_UL2016postVFP(float eta, float pt, double tweak){
  double scale_factor = 1.0;
  if(fabs(eta)<0.8){
    if( 20<pt && pt<30)    scale_factor =  0.9740263431488866;
    if( 30<pt && pt<50)    scale_factor =  0.9757661802546029;
    if( 50<pt && pt<70)    scale_factor =  0.9763278405794318;
    if( 70<pt && pt<100)   scale_factor =  0.9756356762965536;
    if( 100<pt && pt<140)  scale_factor =  0.9734584748572837;
    if( 140<pt && pt<200)  scale_factor =  0.9710701377517904;
    if( 200<pt && pt<300)  scale_factor =  0.9620675182241363;
    if( 300<pt && pt<600)  scale_factor =  0.9486070554691469;
    if( 600<pt && pt<1000) scale_factor =  0.917817192814847; 
  }
  else if(0.8<fabs(eta) && fabs(eta)<1.6){
    if( 20<pt && pt<30)    scale_factor =  0.9740263431488866;
    if( 30<pt && pt<50)    scale_factor =  0.9757661802546029;
    if( 50<pt && pt<70)    scale_factor =  0.9763278405794318;
    if( 70<pt && pt<100)   scale_factor =  0.9756356762965536;
    if( 100<pt && pt<140)  scale_factor =  0.9734584748572837;
    if( 140<pt && pt<200)  scale_factor =  0.9710701377517904;
    if( 200<pt && pt<300)  scale_factor =  0.9620675182241363;
    if( 300<pt && pt<600)  scale_factor =  0.9486070554691469;
    if( 600<pt && pt<1000) scale_factor =  0.917817192814847;
  }
  else if(1.6<fabs(eta) && fabs(eta)<2.5){
    if( 20<pt && pt<30)    scale_factor =  0.9740263431488866;
    if( 30<pt && pt<50)    scale_factor =  0.9757661802546029;
    if( 50<pt && pt<70)    scale_factor =  0.9763278405794318;
    if( 70<pt && pt<100)   scale_factor =  0.9756356762965536;
    if( 100<pt && pt<140)  scale_factor =  0.9734584748572837;
    if( 140<pt && pt<200)  scale_factor =  0.9710701377517904;
    if( 200<pt && pt<300)  scale_factor =  0.9620675182241363;
    if( 300<pt && pt<600)  scale_factor =  0.9486070554691469;
    if( 600<pt && pt<1000) scale_factor =  0.917817192814847;
  }
  scale_factor = scale_factor*(1+tweak);
  return scale_factor;
}

double AnaScript::getScaleFactors_cTagJets_Mis_UL2016postVFP(float eta, float pt, double tweak){
  double scale_factor = 1.0;
  if(fabs(eta)<0.8){
    if( 20<pt && pt<30)    scale_factor =  0.9740263431488866;
    if( 30<pt && pt<50)    scale_factor =  0.9757661802546029;
    if( 50<pt && pt<70)    scale_factor =  0.9763278405794318;
    if( 70<pt && pt<100)   scale_factor =  0.9756356762965536;
    if( 100<pt && pt<140)  scale_factor =  0.9734584748572837;
    if( 140<pt && pt<200)  scale_factor =  0.9710701377517904;
    if( 200<pt && pt<300)  scale_factor =  0.9620675182241363;
    if( 300<pt && pt<600)  scale_factor =  0.9486070554691469;
    if( 600<pt && pt<1000) scale_factor =  0.917817192814847;
  }
  else if(0.8<fabs(eta) && fabs(eta)<1.6){
    if( 20<pt && pt<30)    scale_factor =  0.9740263431488866;
    if( 30<pt && pt<50)    scale_factor =  0.9757661802546029;
    if( 50<pt && pt<70)    scale_factor =  0.9763278405794318;
    if( 70<pt && pt<100)   scale_factor =  0.9756356762965536;
    if( 100<pt && pt<140)  scale_factor =  0.9734584748572837;
    if( 140<pt && pt<200)  scale_factor =  0.9710701377517904;
    if( 200<pt && pt<300)  scale_factor =  0.9620675182241363;
    if( 300<pt && pt<600)  scale_factor =  0.9486070554691469;
    if( 600<pt && pt<1000) scale_factor =  0.917817192814847;
  }
  else if(1.6<fabs(eta) && fabs(eta)<2.5){
    if( 20<pt && pt<30)    scale_factor =  0.9740263431488866;
    if( 30<pt && pt<50)    scale_factor =  0.9757661802546029;
    if( 50<pt && pt<70)    scale_factor =  0.9763278405794318;
    if( 70<pt && pt<100)   scale_factor =  0.9756356762965536;
    if( 100<pt && pt<140)  scale_factor =  0.9734584748572837;
    if( 140<pt && pt<200)  scale_factor =  0.9710701377517904;
    if( 200<pt && pt<300)  scale_factor =  0.9620675182241363;
    if( 300<pt && pt<600)  scale_factor =  0.9486070554691469;
    if( 600<pt && pt<1000) scale_factor =  0.917817192814847;   
  }
  scale_factor = scale_factor*(1+tweak);
  return scale_factor;
}

double AnaScript::getScaleFactors_LightTagJets_Mis_UL2016postVFP(float eta, float pt, double tweak){
  double scale_factor = 1.0;
  if(fabs(eta)<0.8){
    if( 20<pt && pt<30)    scale_factor =  0.8635390112500001;
    if( 30<pt && pt<50)    scale_factor =  0.7324906708;
    if( 50<pt && pt<70)    scale_factor =  0.6704593434666667;
    if( 70<pt && pt<100)   scale_factor =  0.64586460205;
    if( 100<pt && pt<140)  scale_factor =  0.6437977105333333;
    if( 140<pt && pt<200)  scale_factor =  0.6535650716666668;
    if( 200<pt && pt<300)  scale_factor =  0.7087390450000001;
    if( 300<pt && pt<600)  scale_factor =  0.800244505;
    if( 600<pt && pt<1000) scale_factor =  0.9801504325000001;
  }
  else if(0.8<fabs(eta) && fabs(eta)<1.6){
    if( 20<pt && pt<30)    scale_factor =  0.8635390112500001;
    if( 30<pt && pt<50)    scale_factor =  0.7324906708;
    if( 50<pt && pt<70)    scale_factor =  0.6704593434666667;
    if( 70<pt && pt<100)   scale_factor =  0.64586460205;
    if( 100<pt && pt<140)  scale_factor =  0.6437977105333333;
    if( 140<pt && pt<200)  scale_factor =  0.6535650716666668;
    if( 200<pt && pt<300)  scale_factor =  0.7087390450000001;
    if( 300<pt && pt<600)  scale_factor =  0.800244505;
    if( 600<pt && pt<1000) scale_factor =  0.9801504325000001;
  }
  else if(1.6<fabs(eta) && fabs(eta)<2.5){
    if( 20<pt && pt<30)    scale_factor =  0.8635390112500001;
    if( 30<pt && pt<50)    scale_factor =  0.7324906708;
    if( 50<pt && pt<70)    scale_factor =  0.6704593434666667;
    if( 70<pt && pt<100)   scale_factor =  0.64586460205;
    if( 100<pt && pt<140)  scale_factor =  0.6437977105333333;
    if( 140<pt && pt<200)  scale_factor =  0.6535650716666668;
    if( 200<pt && pt<300)  scale_factor =  0.7087390450000001;
    if( 300<pt && pt<600)  scale_factor =  0.800244505;
    if( 600<pt && pt<1000) scale_factor =  0.9801504325000001;
  }
  scale_factor = scale_factor*(1+tweak);
  return scale_factor;
}

//#############
//    2017
//#############

double AnaScript::getScaleFactors_bTagJets_MedWP_UL17(float eta, float pt, double tweak){
  double scale_factor = 1.0;
  if(fabs(eta)<0.8){
    if( 20<pt && pt<30)    scale_factor =  0.9519682519133669;
    if( 30<pt && pt<50)    scale_factor =  0.9563256715252757;
    if( 50<pt && pt<70)    scale_factor =  0.9608449158737093;
    if( 70<pt && pt<100)   scale_factor =  0.9652537745696852;
    if( 100<pt && pt<140)  scale_factor =  0.9700498537954898;
    if( 140<pt && pt<200)  scale_factor =  0.9733563500565519;
    if( 200<pt && pt<300)  scale_factor =  0.9814313982850057;
    if( 300<pt && pt<600)  scale_factor =  0.9893550594758295;
    if( 600<pt && pt<1000) scale_factor =  1.0016175321086023;
  }
  else if(0.8<fabs(eta) && fabs(eta)<1.6){
    if( 20<pt && pt<30)    scale_factor =  0.9519682519133669;
    if( 30<pt && pt<50)    scale_factor =  0.9563256715252757;
    if( 50<pt && pt<70)    scale_factor =  0.9608449158737093;
    if( 70<pt && pt<100)   scale_factor =  0.9652537745696852;
    if( 100<pt && pt<140)  scale_factor =  0.9700498537954898;
    if( 140<pt && pt<200)  scale_factor =  0.9733563500565519;
    if( 200<pt && pt<300)  scale_factor =  0.9814313982850057;
    if( 300<pt && pt<600)  scale_factor =  0.9893550594758295;
    if( 600<pt && pt<1000) scale_factor =  1.0016175321086023;
  }
  else if(1.6<fabs(eta) && fabs(eta)<2.5){
    if( 20<pt && pt<30)    scale_factor =  0.9519682519133669;
    if( 30<pt && pt<50)    scale_factor =  0.9563256715252757;
    if( 50<pt && pt<70)    scale_factor =  0.9608449158737093;
    if( 70<pt && pt<100)   scale_factor =  0.9652537745696852;
    if( 100<pt && pt<140)  scale_factor =  0.9700498537954898;
    if( 140<pt && pt<200)  scale_factor =  0.9733563500565519;
    if( 200<pt && pt<300)  scale_factor =  0.9814313982850057;
    if( 300<pt && pt<600)  scale_factor =  0.9893550594758295;
    if( 600<pt && pt<1000) scale_factor =  1.0016175321086023;
  }
  scale_factor = scale_factor*(1+tweak);
  return scale_factor;
}

double AnaScript::getScaleFactors_cTagJets_Mis_UL17(float eta, float pt, double tweak){
  double scale_factor = 1.0;
  if(fabs(eta)<0.8){
    if( 20<pt && pt<30)    scale_factor =  0.9519682519133669;
    if( 30<pt && pt<50)    scale_factor =  0.9563256715252757;
    if( 50<pt && pt<70)    scale_factor =  0.9608449158737093;
    if( 70<pt && pt<100)   scale_factor =  0.9652537745696852;
    if( 100<pt && pt<140)  scale_factor =  0.9700498537954898;
    if( 140<pt && pt<200)  scale_factor =  0.9733563500565519;
    if( 200<pt && pt<300)  scale_factor =  0.9814313982850057;
    if( 300<pt && pt<600)  scale_factor =  0.9893550594758295;
    if( 600<pt && pt<1000) scale_factor =  1.0016175321086023; 
  }
  else if(0.8<fabs(eta) && fabs(eta)<1.6){
    if( 20<pt && pt<30)    scale_factor =  0.9519682519133669;
    if( 30<pt && pt<50)    scale_factor =  0.9563256715252757;
    if( 50<pt && pt<70)    scale_factor =  0.9608449158737093;
    if( 70<pt && pt<100)   scale_factor =  0.9652537745696852;
    if( 100<pt && pt<140)  scale_factor =  0.9700498537954898;
    if( 140<pt && pt<200)  scale_factor =  0.9733563500565519;
    if( 200<pt && pt<300)  scale_factor =  0.9814313982850057;
    if( 300<pt && pt<600)  scale_factor =  0.9893550594758295;
    if( 600<pt && pt<1000) scale_factor =  1.0016175321086023; 
  }
  else if(1.6<fabs(eta) && fabs(eta)<2.5){
    if( 20<pt && pt<30)    scale_factor =  0.9519682519133669;
    if( 30<pt && pt<50)    scale_factor =  0.9563256715252757;
    if( 50<pt && pt<70)    scale_factor =  0.9608449158737093;
    if( 70<pt && pt<100)   scale_factor =  0.9652537745696852;
    if( 100<pt && pt<140)  scale_factor =  0.9700498537954898;
    if( 140<pt && pt<200)  scale_factor =  0.9733563500565519;
    if( 200<pt && pt<300)  scale_factor =  0.9814313982850057;
    if( 300<pt && pt<600)  scale_factor =  0.9893550594758295;
    if( 600<pt && pt<1000) scale_factor =  1.0016175321086023;
  }
  scale_factor = scale_factor*(1+tweak);
  return scale_factor;
}

double AnaScript::getScaleFactors_LightTagJets_Mis_UL17(float eta, float pt, double tweak){
  double scale_factor = 1.0;
  if(fabs(eta)<0.8){
    if( 20<pt && pt<30)    scale_factor =  1.253307440625;
    if( 30<pt && pt<50)    scale_factor =  1.2712693499999999;
    if( 50<pt && pt<70)    scale_factor =  1.2714768433333332;
    if( 70<pt && pt<100)   scale_factor =  1.2610167726838233;
    if( 100<pt && pt<140)  scale_factor =  1.2405895966666667;
    if( 140<pt && pt<200)  scale_factor =  1.2216736958333334;
    if( 200<pt && pt<300)  scale_factor =  1.1608665224999999;
    if( 300<pt && pt<600)  scale_factor =  1.088226675;
    if( 600<pt && pt<1000) scale_factor =  1.0281731374999998;
  }
  else if(0.8<fabs(eta) && fabs(eta)<1.6){
    if( 20<pt && pt<30)    scale_factor =  1.253307440625;
    if( 30<pt && pt<50)    scale_factor =  1.2712693499999999;
    if( 50<pt && pt<70)    scale_factor =  1.2714768433333332;
    if( 70<pt && pt<100)   scale_factor =  1.2610167726838233;
    if( 100<pt && pt<140)  scale_factor =  1.2405895966666667;
    if( 140<pt && pt<200)  scale_factor =  1.2216736958333334;
    if( 200<pt && pt<300)  scale_factor =  1.1608665224999999;
    if( 300<pt && pt<600)  scale_factor =  1.088226675;
    if( 600<pt && pt<1000) scale_factor =  1.0281731374999998;
  }
  else if(1.6<fabs(eta) && fabs(eta)<2.5){
    if( 20<pt && pt<30)    scale_factor =  1.253307440625;
    if( 30<pt && pt<50)    scale_factor =  1.2712693499999999;
    if( 50<pt && pt<70)    scale_factor =  1.2714768433333332;
    if( 70<pt && pt<100)   scale_factor =  1.2610167726838233;
    if( 100<pt && pt<140)  scale_factor =  1.2405895966666667;
    if( 140<pt && pt<200)  scale_factor =  1.2216736958333334;
    if( 200<pt && pt<300)  scale_factor =  1.1608665224999999;
    if( 300<pt && pt<600)  scale_factor =  1.088226675;
    if( 600<pt && pt<1000) scale_factor =  1.0281731374999998;
  }
  scale_factor = scale_factor*(1+tweak);
  return scale_factor;
}


//#############
//    2018
//#############
double AnaScript::getScaleFactors_bTagJets_MedWP_UL18(float eta, float pt, double tweak){
  double scale_factor = 1.0;
  if(fabs(eta)<0.8){
    if( 20<pt && pt<30)    scale_factor =  0.937523671980794;
    if( 30<pt && pt<50)    scale_factor =  0.9496917070284739;
    if( 50<pt && pt<70)    scale_factor =  0.9591118079992195;
    if( 70<pt && pt<100)   scale_factor =  0.965108901498324;
    if( 100<pt && pt<140)  scale_factor =  0.9679656837494776;
    if( 140<pt && pt<200)  scale_factor =  0.9676505555943556;
    if( 200<pt && pt<300)  scale_factor =  0.9587194332404444;
    if( 300<pt && pt<600)  scale_factor =  0.9380435330652059;
    if( 600<pt && pt<1000) scale_factor =  0.8804050401452473;    
  }  
  else if(0.8<fabs(eta) && fabs(eta)<1.6){
    if( 20<pt && pt<30)    scale_factor =  0.937523671980794;
    if( 30<pt && pt<50)    scale_factor =  0.9496917070284739;
    if( 50<pt && pt<70)    scale_factor =  0.9591118079992195;
    if( 70<pt && pt<100)   scale_factor =  0.965108901498324;
    if( 100<pt && pt<140)  scale_factor =  0.9679656837494776;
    if( 140<pt && pt<200)  scale_factor =  0.9676505555943556;
    if( 200<pt && pt<300)  scale_factor =  0.9587194332404444;
    if( 300<pt && pt<600)  scale_factor =  0.9380435330652059;
    if( 600<pt && pt<1000) scale_factor =  0.8804050401452473;
  }  
  else if(1.6<fabs(eta) && fabs(eta)<2.5){
    if( 20<pt && pt<30)    scale_factor =  0.937523671980794;
    if( 30<pt && pt<50)    scale_factor =  0.9496917070284739;
    if( 50<pt && pt<70)    scale_factor =  0.9591118079992195;
    if( 70<pt && pt<100)   scale_factor =  0.965108901498324;
    if( 100<pt && pt<140)  scale_factor =  0.9679656837494776;
    if( 140<pt && pt<200)  scale_factor =  0.9676505555943556;
    if( 200<pt && pt<300)  scale_factor =  0.9587194332404444;
    if( 300<pt && pt<600)  scale_factor =  0.9380435330652059;
    if( 600<pt && pt<1000) scale_factor =  0.8804050401452473;    
  }
  scale_factor = scale_factor*(1+tweak);
  return scale_factor;
}
  
double AnaScript::getScaleFactors_cTagJets_Mis_UL18(float eta, float pt, double tweak){
  double scale_factor = 1.0;
  if(fabs(eta)<0.8){
    if( 20<pt && pt<30)    scale_factor =  0.937523671980794;
    if( 30<pt && pt<50)    scale_factor =  0.9496917070284739;
    if( 50<pt && pt<70)    scale_factor =  0.9591118079992195;
    if( 70<pt && pt<100)   scale_factor =  0.965108901498324;
    if( 100<pt && pt<140)  scale_factor =  0.9679656837494776;
    if( 140<pt && pt<200)  scale_factor =  0.9676505555943556;
    if( 200<pt && pt<300)  scale_factor =  0.9587194332404444;
    if( 300<pt && pt<600)  scale_factor =  0.9380435330652059;
    if( 600<pt && pt<1000) scale_factor =  0.8804050401452473;
  }
  else if(0.8<fabs(eta) && fabs(eta)<1.6){
    if( 20<pt && pt<30)    scale_factor =  0.937523671980794;
    if( 30<pt && pt<50)    scale_factor =  0.9496917070284739;
    if( 50<pt && pt<70)    scale_factor =  0.9591118079992195;
    if( 70<pt && pt<100)   scale_factor =  0.965108901498324;
    if( 100<pt && pt<140)  scale_factor =  0.9679656837494776;
    if( 140<pt && pt<200)  scale_factor =  0.9676505555943556;
    if( 200<pt && pt<300)  scale_factor =  0.9587194332404444;
    if( 300<pt && pt<600)  scale_factor =  0.9380435330652059;
    if( 600<pt && pt<1000) scale_factor =  0.8804050401452473;
  }
  else if(1.6<fabs(eta) && fabs(eta)<2.5){
    if( 20<pt && pt<30)    scale_factor =  0.937523671980794;
    if( 30<pt && pt<50)    scale_factor =  0.9496917070284739;
    if( 50<pt && pt<70)    scale_factor =  0.9591118079992195;
    if( 70<pt && pt<100)   scale_factor =  0.965108901498324;
    if( 100<pt && pt<140)  scale_factor =  0.9679656837494776;
    if( 140<pt && pt<200)  scale_factor =  0.9676505555943556;
    if( 200<pt && pt<300)  scale_factor =  0.9587194332404444;
    if( 300<pt && pt<600)  scale_factor =  0.9380435330652059;
    if( 600<pt && pt<1000) scale_factor =  0.8804050401452473;
  }
  scale_factor = scale_factor*(1+tweak);
  return scale_factor;
}

double AnaScript::getScaleFactors_LightTagJets_Mis_UL18(float eta, float pt, double tweak){
  double scale_factor = 1.0;
  if(fabs(eta)<0.8){
    if( 20<pt && pt<30)    scale_factor =  1.36152779875;
    if( 30<pt && pt<50)    scale_factor =  1.3770484468;
    if( 50<pt && pt<70)    scale_factor =  1.3746623594666667;
    if( 70<pt && pt<100)   scale_factor =  1.3614068326088236;
    if( 100<pt && pt<140)  scale_factor =  1.3375798545333333;
    if( 140<pt && pt<200)  scale_factor =  1.3161379216666667;
    if( 200<pt && pt<300)  scale_factor =  1.249236655;
    if( 300<pt && pt<600)  scale_factor =  1.173893605;
    if( 600<pt && pt<1000) scale_factor =  1.1438456825;
  }
  else if(0.8<fabs(eta) && fabs(eta)<1.6){
    if( 20<pt && pt<30)    scale_factor =  1.36152779875;
    if( 30<pt && pt<50)    scale_factor =  1.3770484468;
    if( 50<pt && pt<70)    scale_factor =  1.3746623594666667;
    if( 70<pt && pt<100)   scale_factor =  1.3614068326088236;
    if( 100<pt && pt<140)  scale_factor =  1.3375798545333333;
    if( 140<pt && pt<200)  scale_factor =  1.3161379216666667;
    if( 200<pt && pt<300)  scale_factor =  1.249236655;
    if( 300<pt && pt<600)  scale_factor =  1.173893605;
    if( 600<pt && pt<1000) scale_factor =  1.1438456825;
  }
  else if(1.6<fabs(eta) && fabs(eta)<2.5){
    if( 20<pt && pt<30)    scale_factor =  1.36152779875;
    if( 30<pt && pt<50)    scale_factor =  1.3770484468;
    if( 50<pt && pt<70)    scale_factor =  1.3746623594666667;
    if( 70<pt && pt<100)   scale_factor =  1.3614068326088236;
    if( 100<pt && pt<140)  scale_factor =  1.3375798545333333;
    if( 140<pt && pt<200)  scale_factor =  1.3161379216666667;
    if( 200<pt && pt<300)  scale_factor =  1.249236655;
    if( 300<pt && pt<600)  scale_factor =  1.173893605;
    if( 600<pt && pt<1000) scale_factor =  1.1438456825;
  }
  scale_factor = scale_factor*(1+tweak);
  return scale_factor;
}
