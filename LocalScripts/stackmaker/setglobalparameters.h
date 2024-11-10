#ifndef SETGLOBALPARAMETERS_H
#define SETGLOBALPARAMETERS_H

#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;

//Global variables being used by the class:
extern float globalSbyB;
extern float globalObsbyExp;
extern float globalObsbyExpErr;
extern double QCDscale, TOPscale, DYscale;
extern nlohmann::json lumiData;
extern double datalumi;
extern bool toLog;
extern bool toZoom;
extern TString campaign, campaign_name;
extern TString channel;
extern TString tag, tag2, tag3, info;

void InitializeValues(){
  globalSbyB = 0;
  toLog = true;
  toZoom = false;

  QCDscale = 1.0;
  TOPscale = 1.0;
  DYscale  = 1.0;

  campaign_name = "";

  //Custom settings/global-scaling for each campaign:
  if(campaign == "2016preVFP_UL"){
    campaign_name = "19.7 fb^{-1} (2016-preVFP)";
  }
  else if(campaign == "2016postVFP_UL"){
    campaign_name = "16.2 fb^{-1} (2016-postVFP)";
  }
  else if(campaign == "2017_UL"){
    campaign_name = "41.5 fb^{-1} (2017)";
  }
  else if(campaign == "2018_UL"){
    campaign_name = "59.8 fb^{-1} (2018)";
    if(channel == "mm")      QCDscale = 0.155758;
    else if(channel == "me") QCDscale = 0.094731;
    else if(channel == "em") QCDscale = 0.192381;
    else if(channel == "ee") QCDscale = 0.303599;
  }

  //Names:
  if     (channel == "ee") tag2 = info+" (ee)";
  else if(channel == "em") tag2 = info+" (e#mu)";
  else if(channel == "me") tag2 = info+" (#mu e)";
  else if(channel == "mm") tag2 = info+" (#mu#mu)";
  else cout<<"\033[31mGlobal: Please provide correct channel name!\033[0m"<<endl;
}

void loadLuminosityData(string jsonfilepath) {
  std::ifstream jsonFile(jsonfilepath);
  if (jsonFile.is_open()) {
    jsonFile >> lumiData;
    jsonFile.close(); 
  }
  else cout<<"\033[31mGlobal: Error: Failed to open JSON file.\033[0m"<<endl;
}

void SetLumi(){

  if(campaign == "2018_UL"){
    datalumi = 59830;
    loadLuminosityData("../../InputJsons/lumidata_2018.json");
    //cout<<"Setting datalumi = "<<datalumi<<endl;
  }
  else if (campaign == "2016preVFP_UL"){
    datalumi = 19692;
    loadLuminosityData("../../Metadata/jsondata/lumidata_2016preVFP_UL.json");
    //cout<<"Setting datalumi = "<<datalumi<<endl;
  }
}

TString sample_alias(TString newname){

  //correctname -> incorrectname {to read from json, not to load file}
  
  TString oldname = newname;
  if (campaign == "2016preVFP_UL"){
    if      (newname == "M10to50")                    oldname = "M10To50";
    else if (newname == "tW_AntiTop_InclusiveDecays") oldname = "tW_AntiTop_InclusiceDecays";
    else if (newname == "WZTo2Q2L")                   oldname = "WZTo2L2Q";
    else if (newname == "TTWToLNu")                   oldname = "TTWToLnu";
    else if (newname == "EGamma")             oldname = "SingleElectron";
    else if (newname == "EGamma_B")           oldname = "SingleElectronBpreVFP_ver2";
    else if (newname == "EGamma_C")           oldname = "SingleElectronCpreVFP";
    else if (newname == "EGamma_D")           oldname = "SingleElectronDpreVFP";
    else if (newname == "EGamma_E")           oldname = "SingleElectronEpreVFP";
    else if (newname == "EGamma_F")           oldname = "SingleElectronFpreVFP";
    
    //if(newname != oldname) cout<<"\033[93mWarning: "+newname+" -> "+oldname+"\033[0m"<<endl;
  }

  return oldname; 
}

#endif //SETGLOBALPARAMETERS_H
