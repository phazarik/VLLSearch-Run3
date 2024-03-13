//#################################################################################################
// Scale-Factors from POG:
// These numbers are multiplied to jet_eff to calculate jet_prob_data
// The final probs for the jets is given by product of prob_data and prob_mc for each jet.
// The scale factor for the event is prob_data / prob_mc (this is an event level correction to MC)
//#################################################################################################

double AnaScript::getScaleFactors_bTagJets_MedWP_UL18(float eta, float pt){
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
  return scale_factor;
}
  
double AnaScript::getScaleFactors_cTagJets_Mis_UL18(float eta, float pt){
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
  return scale_factor;
}

double AnaScript::getScaleFactors_LightTagJets_Mis_UL18(float eta, float pt){
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
  return scale_factor;
}
