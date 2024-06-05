#include "correction.h"

auto egammajson2018      = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/EGM/2018_UL/electron.json");
auto egammaID2018        = egammajson2018->at("UL-Electron-ID-SF");
//auto egammaScalejson2018 = correction::CorrectionSet::from_file("setup/POG/EGM/2018_UL/EGM_ScaleUnc.json.gz");
//auto egammaScale2018     = egammaScalejson2018->at("UL-EGM_ScaleUnc");

auto egammajson2017      = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/EGM/2017_UL/electron.json");
auto egammaID2017        = egammajson2017->at("UL-Electron-ID-SF");
//auto egammaScalejson2017 = correction::CorrectionSet::from_file("setup/POG/EGM/2017_UL/EGM_ScaleUnc.json.gz");
//auto egammaScale2017     = egammaScalejson2017->at("UL-EGM_ScaleUnc");

auto egammajson2016postVFP      = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/EGM/2016postVFP_UL/electron.json");
auto egammaID2016postVFP        = egammajson2016postVFP->at("UL-Electron-ID-SF");
//auto egammaScalejson2016postVFP = correction::CorrectionSet::from_file("setup/POG/EGM/2016postVFP_UL/EGM_ScaleUnc.json.gz");
//auto egammaScale2016postVFP     = egammaScalejson2016postVFP->at("UL-EGM_ScaleUnc");

auto egammajson2016preVFP      = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/EGM/2016preVFP_UL/electron.json");
auto egammaID2016preVFP        = egammajson2016preVFP->at("UL-Electron-ID-SF");
//auto egammaScalejson2016preVFP = correction::CorrectionSet::from_file("setup/POG/EGM/2016preVFP_UL/EGM_ScaleUnc.json.gz");
//auto egammaScale2016preVFP     = egammaScalejson2016preVFP->at("UL-EGM_ScaleUnc");

float AnaScript::correctionlib_egmIDSF(Particle electron, string mode){

  //https://cms-nanoaod-integration.web.cern.ch/commonJSONSFs/summaries/EGM_2016postVFP_UL_electron.html
  float pt = electron.v.Pt();
  float eta = electron.v.Eta();
  
  string mode_;
  if(      mode=="nom"  ) mode_="sf";
  else if( mode=="up"   ) mode_="sfup";
  else if( mode=="down" ) mode_="sfdown";

  string era_;
  if( _campaign=="2018_UL" )            era_ = "2018";
  else if( _campaign=="2017_UL" )       era_ = "2017";
  else if( _campaign=="2016preVFP_UL" ) era_ = "2016preVFP";
  else if( _campaign=="2016preVFP_UL" ) era_ = "2016postVFP";
  else cout<<"egamma.h : Provide correct campaign name!"<<endl;
  
  std::vector<correction::Variable::Type>  values;
  values.emplace_back(era_);
  values.emplace_back(mode_);
  values.emplace_back("Medium");
  values.emplace_back(eta);//Do we need to add deltaEtaSC??
  values.emplace_back(pt);
  
  float sf = 1.0;
  if( pt >10.){
    if( _campaign=="2018_UL" )        sf = egammaID2018->evaluate(values); 
    if( _campaign=="2017_UL" )        sf = egammaID2017->evaluate(values); 
    if( _campaign=="2016postVFP_UL" ) sf = egammaID2016postVFP->evaluate(values);
    if( _campaign=="2016preVFP_UL" )  sf = egammaID2016preVFP->evaluate(values); 
  }
  
  return sf;
  
}
