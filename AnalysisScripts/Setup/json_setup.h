//For pasring json files:
#include <filesystem>

json AnaScript::loadJson(){
  string jsonfilename;
  
  if(     _year==2016) jsonfilename = "Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON.json"; 
  else if(_year==2017) jsonfilename = "Cert_294927-306462_13TeV_UL2017_Collisions17_GoldenJSON.json";
  else if(_year==2018) jsonfilename = "Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON.json";
  else if(_year==2022) jsonfilename = "Cert_Collisions2022_355100_362760_Golden.json";
  else if(_year==2023) jsonfilename = "Cert_Collisions2023_366442_370790_Golden.json";
  else {
    cout<<"json file not loaded: invalid campaign!"<<endl;
    return {};
  }
  string thisdir = std::filesystem::path(__FILE__).parent_path().string();
  string jsonfilepath = thisdir+"/GoldenJsons/"+jsonfilename;
  std::ifstream f(jsonfilepath);

  if(!f){
    cout << "\033[31mError: Could not open the json file!\033[0m" <<endl;
    return {};
  }
  
  json jsondata = json::parse(f);
  f.close();

  if (jsondata.empty()) cout << "\033[31mWarning: JSON data is empty!\033[0m" << endl;

  cout<<"json file loaded."<<endl;
  return jsondata;
}

bool AnaScript::checkJson(bool isData, int runno, int lumisection){

  if(!isData) return 1;

  int year=0;
  bool keep_this_event = false;
  
  //The years are  defined using the run numbers:
  //if( runno==271036 || runno==284044 || (runno>271036 && runno<284044) ){ year=2016; }
  //if( runno==294927 || runno==306462 || (runno>294927 && runno<306462) ){ year=2017; }
  //if( runno==314472 || runno==325175 || (runno>314472 && runno<325175) ){ year=2018; }
  year = _year;
  
  std::string strrunno = std::to_string(runno);
  auto checkrun = jsondata.find(strrunno);
  
  if( checkrun != jsondata.end()){
    auto runfilter  = *checkrun;
    for( unsigned int i=0; i<runfilter.size(); i++ ){
      int runfilterbegin = runfilter.at(i).at(0);
      int runfilterend   = runfilter.at(i).at(1);
      if(
	 lumisection==runfilterbegin ||
	 lumisection==runfilterend ||
	 (lumisection>runfilterbegin && lumisection<runfilterend)
	 ){
	keep_this_event = true;
	break;
      }
    }
  }
  
  return keep_this_event;

}
