#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/local/bJet/JetEff_DeepJet_MediumWP_2018_UL_2LSS.h"
#include "correction.h"

auto btvjson2018      = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/BTV/2018_UL/btagging.json.gz");
//auto bcjet2018        = btvjson2018->at("deepJet_comb");
auto bcjet2018        = btvjson2018->at("deepJet_mujets");
auto lightjet2018     = btvjson2018->at("deepJet_incl");

auto btvjson2017      = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/BTV/2017_UL/btagging.json.gz");
auto bcjet2017        = btvjson2017->at("deepJet_comb");
auto lightjet2017     = btvjson2017->at("deepJet_incl");

auto btvjson2016postVFP      = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/BTV/2016postVFP_UL/btagging.json.gz");
auto bcjet2016postVFP        = btvjson2016postVFP->at("deepJet_comb");
auto lightjet2016postVFP     = btvjson2016postVFP->at("deepJet_incl");

auto btvjson2016preVFP      = correction::CorrectionSet::from_file("/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/POG/BTV/2016preVFP_UL/btagging.json.gz");
auto bcjet2016preVFP        = btvjson2016preVFP->at("deepJet_comb");
auto lightjet2016preVFP     = btvjson2016preVFP->at("deepJet_incl");


float AnaScript::correctionlib_btagWPSFfromPOG(Particle jet, string mode){

  // ------------------------------------------------------------------------------------------
  // This function loads the correction factors provided by POG. It runs for one jet at a time.
  // It is used on-the-fly while running over the jet collection in the next function
  //--------------------------------------------------------------------------------------------

  float pt = jet.v.Pt();
  float eta = jet.v.Eta();
  int flav = jet.hadronflavor;

  string mode_;
  if(      mode=="nom"              ) mode_="central";
  else if( mode=="upUncorrelated"   ) mode_="up_uncorrelated";
  else if( mode=="upCorrelated"     ) mode_="up_correlated";
  else if( mode=="downUncorrelated" ) mode_="down_uncorrelated";
  else if( mode=="downCorrelated"   ) mode_="down_correlated";
  
  string wp = "M";   //Working Point of b-tagging algorithm
  
  std::vector<correction::Variable::Type>  values;
  values.emplace_back(mode_);
  values.emplace_back(wp);
  values.emplace_back(flav);
  values.emplace_back(abs(eta));
  values.emplace_back(pt);

  float sf =1.0;
  
  if(pt>20.0 && fabs(eta)<2.5){
    if( _campaign=="2018_UL" ){
      if(flav==5 || flav==4){
	sf = bcjet2018->evaluate(values);
	h.evtweight[15]->Fill(sf);
      }
      else{
	sf = lightjet2018->evaluate(values);
	h.evtweight[16]->Fill(sf);
      }
    }
    else if( _campaign=="2017_UL" ){
      if(flav==5 || flav==4){ sf = bcjet2017->evaluate(values);    }
      else                  { sf = lightjet2017->evaluate(values); }
    }
    else if( _campaign=="2016postVFP_UL" ){
      if(flav==5 || flav==4){ sf = bcjet2016postVFP->evaluate(values);    }
      else                  { sf = lightjet2016postVFP->evaluate(values); }
    }
    else if( _campaign=="2016preVFP_UL" ){
      if(flav==5 || flav==4){ sf = bcjet2016preVFP->evaluate(values);    }    
      else                  { sf = lightjet2016preVFP->evaluate(values); }
    }
    else if( _campaign=="Summer22" ) return 1.0;
    else cout<<"btv_deepjet.h POG: Provide correct campaign name!"<<endl;
  }
  
  return sf;
}


float AnaScript::correctionlib_btagIDSF(vector<Particle> Jet, string mode){

  
  float probability_mc = 1.0;
  float probability_data = 1.0;

  //btag SF is basically an event reweighting procedure
  for (int i = 0; i < (int)Jet.size(); i++){
    float jet_prob_mc   = 1.0;
    float jet_prob_data = 1.0;
    float jet_eff = 1.0; //MC efficiency
    
    //get MC efficiency
    if(_campaign=="2018_UL"){
      jet_eff = correctionlib_btagMCeff_2018UL(Jet.at(i));
      h.evtweight[20]->Fill(jet_eff);
    }
    else { jet_eff = correctionlib_btagMCeff_2018UL(Jet.at(i)); //Using the same numbers for now
      cout<<"btv_deepjet.h : Provide correct campaign name!"<<endl;
    }
    
    //SFfromPOG
    float SFfromPOG = correctionlib_btagWPSFfromPOG(Jet.at(i), mode);
    
    //DeepJetWPThreshold (https://btv-wiki.docs.cern.ch/ScaleFactors/#useful-links)
    //  Year          L           M          T
    //  2018         0.0490     0.2783     0.7100
    //  2017         0.0532     0.3040     0.7476
    //  2016preVFP   0.0508     0.2598     0.6502
    //  2016postVFP  0.0480     0.2489     0.6377
    
    float WPth = 0.;
    if(_campaign =="2018_UL")              WPth=0.2783;
    else if (_campaign =="2017_UL")        WPth=0.3040;
    else if (_campaign =="2016preVFP_UL")  WPth=0.2598;
    else if (_campaign =="2016postVFP_UL") WPth=0.2489;
    else if (_campaign =="Summer22")       return 1.0;
    else cout<<"btv_deepjet.h : Provide correct campaign name!"<<endl;

    //Skipping trailing jets.
    //if(i>2) return 1.0;
      
    // check if jet is tagged or not
    if (Jet.at(i).btagscore > WPth){
      jet_prob_data = jet_eff*SFfromPOG;
      jet_prob_mc   = jet_eff;
    }
    else {    
      jet_prob_data = 1-(jet_eff*SFfromPOG);
      jet_prob_mc   = 1-jet_eff;
    }
    
    probability_data *= jet_prob_data;
    probability_mc   *= jet_prob_mc;
  }
  
  float scaleFactor = 1.0;
  if (probability_mc > 0.0) scaleFactor = probability_data/probability_mc;
  
  return scaleFactor;
  
}
