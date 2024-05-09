#include "correction.h"

auto muonjson2018 = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/MUO/2018_UL/muon_Z_v2.json");
auto muoniso2018  = muonjson2018->at("NUM_TightRelIso_DEN_MediumID");
auto muonid2018   = muonjson2018->at("NUM_MediumID_DEN_genTracks");

auto muonjson2017 = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/MUO/2017_UL/muon_Z_v2.json");
auto muoniso2017  = muonjson2017->at("NUM_TightRelIso_DEN_MediumID");
auto muonid2017   = muonjson2017->at("NUM_MediumID_DEN_genTracks");

auto muonjson2016postVFP = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/MUO/2016postVFP_UL/muon_Z_v2.json");
auto muoniso2016postVFP  = muonjson2016postVFP->at("NUM_TightRelIso_DEN_MediumID");
auto muonid2016postVFP   = muonjson2016postVFP->at("NUM_MediumID_DEN_genTracks");

auto muonjson2016preVFP = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/MUO/2016preVFP_UL/muon_Z_v2.json");
auto muoniso2016preVFP  = muonjson2016preVFP->at("NUM_TightRelIso_DEN_MediumID");
auto muonid2016preVFP   = muonjson2016preVFP->at("NUM_MediumID_DEN_genTracks");


float AnaScript::correctionlib_muonIDSF(Particle muon, string mode){

  float pt = muon.v.Pt();
  float eta = muon.v.Eta();

  string mode_;
  if(      mode=="nom"  ) mode_="sf";
  else if( mode=="up"   ) mode_="systup";
  else if( mode=="down" ) mode_="systdown";

  //Campaign names examples:
  //2018_UL
  //2017_UL
  //2016postVFP_UL
  //2016preVFP_UL
  
  string era_ = _campaign.Data();
  
  std::vector<correction::Variable::Type>  values;
  values.emplace_back(era_);
  values.emplace_back(fabs(eta));
  values.emplace_back(pt);
  values.emplace_back(mode_);

  float sf = 1.0;
  if( pt >15. && fabs(eta)<2.4){
    if(_campaign=="2018_UL")             sf = muonid2018->evaluate(values);
    else if(_campaign=="2017_UL")        sf = muonid2017->evaluate(values); 
    else if(_campaign=="2016postVFP_UL") sf = muonid2016postVFP->evaluate(values); 
    else if(_campaign=="2016preVFP_UL")  sf = muonid2016preVFP->evaluate(values); 
    else cout<<"muon.h : Warning: Provide correct campaign name!"<<endl;
  }
  return sf;
}

float AnaScript::correctionlib_muonIsoSF(Particle muon, string mode){

  float pt = muon.v.Pt();
  float eta = muon.v.Eta();

  string mode_;
  if(      mode=="nom"  ) mode_="sf";
  else if( mode=="up"   ) mode_="systup";
  else if( mode=="down" ) mode_="systdown";
  
  string era_ = _campaign.Data();
  
  std::vector<correction::Variable::Type> values;
  values.emplace_back(era_);
  values.emplace_back(fabs(eta));
  values.emplace_back(pt);
  values.emplace_back(mode_);
  
  float sf = 1.0;
  if( pt >15. && fabs(eta)<2.4){
    if(_campaign=="2018_UL" )            sf = muonid2018->evaluate(values); 
    else if(_campaign=="2017_UL" )       sf = muonid2017->evaluate(values); 
    else if(_campaign=="2016postVFP_UL") sf = muonid2016postVFP->evaluate(values); 
    else if(_campaign=="2016preVFP_UL")  sf = muonid2016preVFP->evaluate(values);
    else cout<<"muon.h : Warning: Provide correct campaign name!"<<endl;
  }
  return sf;
}
