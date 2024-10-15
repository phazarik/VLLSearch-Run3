#define AnaScript_cxx
#include "AnaScript.h"
#include <TH2.h>
#include <TStyle.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
using namespace std;

//Including the header files:
#include "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/TreeMaker/TreeMakerHelper.h"
#include "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/TreeMaker/BookHistograms.h"

//DON'T CHANGE THE FOLLOWING:
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/CustomFunctions.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/EventSelection.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/ProduceGenCollection.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/ProduceRecoCollection.h"

//Correctionlib:
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/applyCorrections.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/local/TriggerEfficiency.h"

//json:
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/nlohmann/json_setup.h"

void AnaScript::Begin(TTree * /*tree*/)
{
  TString option = GetOption();
}

void AnaScript::SlaveBegin(TTree *tree /*tree*/)
{
  //-------------------------------------------
  //Set Campaign : (important for corrections)
  //_campaign = "2018_UL";
  //_campaign = "2017_UL";
  //_campaign = "2016preVFP_UL";
  //_campaign = "2016postVFP_UL";
  //-------------------------------------------

  time(&start);

  //Setting parameters:
  cout<<"Campaign set to: "<<_campaign<<endl;
  if(_campaign == "2018_UL") _year = 2018;
  else if(_campaign == "2017_UL") _year = 2017;
  else if((_campaign == "2016preVFP_UL") || (_campaign == "2016postVFP_UL")) _year = 2016;
  else if(_campaign == "Summer22") _year = 2022;
  else cout<<"main: Provide correct campaign name"<<endl;
  cout<<"Year set to: "<<_year<<endl;

  jsondata = loadJson();
  cout<<"\nn-events \t time_taken (sec)"<<endl;

  TString option = GetOption();
  nEvtTotal = 0;
  nEvtRan = 0;
  nEvtTrigger=0;
  nEvtPass=0;
  nEvtBad=0; //Events flagged out because of invalid decay
  nThrown=0;
  
  //Counters:
  n4l=0;
  n3l=0;
  n2lss=0;
  n2los=0;
  n1l2j=0;
  n1l1j=0;
  n1l0j=0;

  evt_wt = 1.0;
  bad_event = false;

  //Call the function to book the histograms we declared in Hists.
  BookHistograms();
  
  if(_flag=="doublet") cout<<"Removing invalid VLLD decay modes ..."<<endl;
}

void AnaScript::SlaveTerminate()
{
  //_HstFile->Write();
  //_HstFile->Close();

  mytree->Write();
  _TreeFile->Close();

  float goodevtfrac = ((float)nEvtRan)/((float)nEvtTotal);
  float trigevtfrac = ((float)nEvtTrigger)/((float)nEvtTotal);
  float passevtfrac = ((float)nEvtPass)/((float)nEvtTotal);
  float badevtfrac  = ((float)nEvtBad)/((float)nEvtTotal);//for VLLD
  float notgoldenevtfrac  = ((float)nThrown)/((float)nEvtTotal); //for data

  cout<<"---------------------------------------------"<<endl;
  cout<<"Summary:"<<endl;
  cout<<"nEvtTotal = "<<nEvtTotal<<endl;
  cout<<"nEvtRan = "<<nEvtRan<<" ("<<goodevtfrac*100<<" %)"<<endl;
  cout<<"nEvtTrigger = "<<nEvtTrigger<<" ("<<trigevtfrac*100<<" %)"<<endl;
  cout<<"nEvtPass = "<<nEvtPass<<" ("<<passevtfrac*100<<" %)"<<endl;
  cout<<"nEvtBad = "<<nEvtBad<<" ("<<badevtfrac*100<<" %)"<<endl;
  if(_data!=0) cout<<"nEvents not in golden json = "<<nThrown<<" ("<<notgoldenevtfrac*100<<" %)"<<endl;
  cout<<"---------------------------------------------"<<endl;
  
  time(&end);

  double time_taken = double(end-start);
  cout<<"\nTime taken by the programe is = "<<fixed<<time_taken<<setprecision(5);
  cout<<" sec \n"<<endl;
}

void AnaScript::Terminate()
{
  
}

Bool_t AnaScript::Process(Long64_t entry)
{
  // The Process() function is called for each entry in the tree (or possibly
  // keyed object in the case of PROOF) to be processed. The entry argument
  // specifies which entry in the currently loaded tree is to be processed.
  // When processing keyed objects with PROOF, the object is already loaded
  // and is available via the fObject pointer.
  //
  // This function should contain the \"body\" of the analysis. It can contain
  // simple or elaborate selection criteria, run algorithms on the data
  // of the event and typically fill histograms.
  //
  // The processing can be stopped by calling Abort().
  //
  // Use fStatus to set the return value of TTree::Process().
  //
  // The return value is currently not used.

  //------------------------------------------------------
  //Initializing fReaders:
  fReader.SetLocalEntry(entry);
  if(_run3)  fReader_Run3.SetLocalEntry(entry);
  else       fReader_Run2.SetLocalEntry(entry);
  if(_data == 0){
    fReader_MC.SetLocalEntry(entry);
    if(!_run3) fReader_Run2_MC.SetLocalEntry(entry);
    else       fReader_Run3_MC.SetLocalEntry(entry);
  } 
  //-------------------------------------------------------

  //Setting verbosity:
  time(&buffer);
  double time_taken_so_far = double(buffer-start);
  if(_verbosity==0 && nEvtTotal%10000==0)     cout<<nEvtTotal<<" \t "<<time_taken_so_far<<endl;
  else if(_verbosity>0 && nEvtTotal%10000==0) cout<<nEvtTotal<<" \t "<<time_taken_so_far<<endl;

  nEvtTotal++;
  
  GoodEvt2018 = (_year==2018 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);
  GoodEvt2017 = (_year==2017 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);
  GoodEvt2016 = (_year==2016 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);

  GoodEvt = GoodEvt2016 && GoodEvt2017 && GoodEvt2018;
  if(_campaign=="Summer22") GoodEvt = true;

  if(GoodEvt){
    nEvtRan++; //only good events
    //h.nevt->Fill(1);

    triggerRes=true; //Always true for MC
    muon_trigger       = false;
    electron_trigger   = false;
    overlapping_events = false;
    
    if     (_year==2016) {
      muon_trigger     = (*HLT_IsoMu24==1);
      electron_trigger = (*HLT_Ele32_WPTight_Gsf==1);
    }
    else if(_year==2017) {
      muon_trigger     = (*HLT_IsoMu27==1);
      electron_trigger = (*HLT_Ele32_WPTight_Gsf==1);
    }
    else if(_year==2018) {
      muon_trigger     = (*HLT_IsoMu24==1);
      electron_trigger = (*HLT_Ele32_WPTight_Gsf==1);
    }
    
    overlapping_events = muon_trigger && electron_trigger;

    if(_data!=0){

      //Strategy 1: Only considering the non-overlapping parts.
      //if(_flag != "egamma") triggerRes = muon_trigger && !electron_trigger; //For the SingleMuon dataset
      //if(_flag == "egamma") triggerRes = electron_trigger && !muon_trigger; //For the EGamma dataset

      //Strategy 2: Include the overlap in the muon dataset.
      if(_flag != "egamma") triggerRes = muon_trigger; //For the SingleMuon dataset, including overlap.
      if(_flag == "egamma") triggerRes = electron_trigger && !muon_trigger; //For the EGamma dataset

      //Strategy3: Write everything and handle the trigger later.
      //triggerRes = true;

      //Throw awaying bad data that are not included in the GoldenJson:
      int runno = (int)*run;
      int lumisection = (int)*luminosityBlock;
      bool golden_event = checkJson(true, runno, lumisection);
      if(!golden_event){
	nThrown++;
	triggerRes = false;
      }
    }
    
    if(_campaign=="Summer22") triggerRes = true;
    if(triggerRes){
      nEvtTrigger++; //only triggered events
      //h.nevt->Fill(2);

      //###################
      //Gen particle block
      //###################

      genMuon.clear();
      genElectron.clear();
      genLightLepton.clear();
      genJet.clear();
      vllep.clear();
      vlnu.clear();

      bad_event = false;
      if(_data==0){
	createGenLightLeptons();
	createGenJets();
	SortGenObjects();
	//SortPt(genMuon);
	//SortPt(genElectron);
	//SortPt(genLightLepton);
	
	createSignalArrays();
	SortVLL();

	//Correcting the Doublet model (flagging out the invalid decays)
	//This is donw because getting rid of these bad events later will be difficult
	if(_flag=="doublet"){ //for VLLD files
	  bad_event = false;
	  //a) The neutral particle cannot decay to H,nu or Z,nu.
	  // I am flagging out the events with Higgs(25) or the Z(23) as daughetrs of N
	  //cout<<"----"<<endl;
	  for(int i=0; i<(int)vlnu.size(); i++){
	    for(int j=0; j<(int)vlnu.at(i).dauid.size(); j++){
	      if(fabs(vlnu.at(i).dauid[j]) == 25)      bad_event = true;
	      else if(fabs(vlnu.at(i).dauid[j]) == 23) bad_event = true;
	      //cout<<fabs(vlnu.at(i).dauid[j])<<" ";
	    }
	    //cout<<""<<endl;
	  }
	  //if(bad_event) cout<<"bad"<<endl;
	  //else cout<<"good"<<endl;
	  //cout<<"----"<<endl;
	  
	  //b) The lepton cannot decay to a W,nu of the corresponding flavor (ele/mu):
	  // I am flagging out the events with W(24) as daughetrs of L
	  for(int i=0; i<(int)vllep.size(); i++){
	    for(int j=0; j<(int)vllep.at(i).dauid.size(); j++){
	      if(fabs(vllep.at(i).dauid[j]) == 24)     bad_event = true;
	    }
	  }
	}  
	//Make gen-level plots here.
      }
      if(bad_event) nEvtBad++;

      //###################
      //Reco particle block
      //###################

      metpt = *PuppiMET_pt;
      metphi = *PuppiMET_phi;
      
      Muon.clear();
      Electron.clear();
      LightLepton.clear();
      Photon.clear();
      Tau.clear();
      Jet.clear();
      bJet.clear();
      MediumbJet.clear();
      LooseLepton.clear();
      LooseMuon.clear();
      LooseElectron.clear();
      
      createLightLeptons();
      //createPhotons();
      createTaus();
      createJets();

      SortRecoObjects();
     
      //_______________________________________________________________________________________________________
      
      //                         Analysis block
      //______________________________________________________________________________________________________

      //Calculating variables, selecting events  and writing in tree:
      FillTree(mytree);     
      
    }//TriggeredEvts
  }//GoodEvt
  return kTRUE;
}
