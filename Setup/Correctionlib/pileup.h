#include "correction.h"

auto pujson2018 = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/LUM/2018_UL/puWeights.json.gz");
auto pu2018     = pujson2018->at("Collisions18_UltraLegacy_goldenJSON");

auto pujson2017 = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/LUM/2017_UL/puWeights.json.gz");
auto pu2017     = pujson2017->at("Collisions17_UltraLegacy_goldenJSON");

auto pujson2016preVFP = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/LUM/2016preVFP_UL/puWeights.json.gz");
auto pu2016preVFP     = pujson2016preVFP->at("Collisions16_UltraLegacy_goldenJSON");

auto pujson2016postVFP = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/LUM/2016postVFP_UL/puWeights.json.gz");
auto pu2016postVFP     = pujson2016postVFP->at("Collisions16_UltraLegacy_goldenJSON");


float AnaScript::correctionlib_pileupWt(float nTrueInt, string mode){

  string mode_;
  if      (mode=="nom"  ) mode_="nominal";
  else if (mode=="up"   ) mode_="up";
  else if (mode=="down" ) mode_="down";
  
  std::vector<correction::Variable::Type> values;
  values.emplace_back(nTrueInt);
  values.emplace_back(mode_);
  
  float pileupWt = 1.0;
  if      (_campaign=="2018_UL"        ) pileupWt = pu2018->evaluate(values);
  else if (_campaign=="2017_UL"        ) pileupWt = pu2017->evaluate(values);
  else if (_campaign=="2016preVFP_UL"  ) pileupWt = pu2016preVFP->evaluate(values);
  else if (_campaign=="2016postVFP_UL" ) pileupWt = pu2016postVFP->evaluate(values);
  else if( _campaign=="Summer22" )       return 1.0;
  else cout<<"pileup.h : Provide correct campaign name!"<<endl;
  
  return pileupWt;
}
