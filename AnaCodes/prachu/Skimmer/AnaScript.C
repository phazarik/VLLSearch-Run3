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

//DON'T CHANGE THE FOLLOWING:
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/CustomFunctions.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/ProduceGenCollection.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/ProduceRecoCollection.h"
#include "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/Skimmer/skimmerHelper.h"

//json:
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/nlohmann/json_setup.h"

void AnaScript::Begin(TTree * /*tree*/)
{
  TString option = GetOption();
}

void AnaScript::SlaveBegin(TTree *tree)
{
  //---------------------------------------------------
  //Set Campaign manually : (important for corrections)
  //_campaign = "2018_UL";
  //_campaign = "2017_UL";
  //_campaign = "2016preVFP_UL";
  //_campaign = "2016postVFP_UL";
  //---------------------------------------------------
  time(&start);

  //Setting parameters:
  cout<<"Campaign set to: "<<_campaign<<endl;
  //Setting year (overriding SetYear());
  if     (_campaign == "2018_UL") _year = 2018;
  else if(_campaign == "2017_UL") _year = 2017;
  else if((_campaign == "2016preVFP_UL") || (_campaign == "2016postVFP_UL")) _year = 2016;
  else if(_campaign == "Summer22") _year = 2022;
  else cout<<"main: Provide correct campaign name"<<endl;
  cout<<"Year set to: "<<_year<<endl;

  jsondata = loadJson();
  cout<<"\nn-events \t time_taken (sec)"<<endl;
  batch_index = 0;
  batch_size = 10000;
  
  TString option = GetOption();
  nEvtTotal = 0;
  nEvtRan = 0;
  nEvtTrigger=0;
  nEvtPass=0;
  nEvtBad=0;
  nThrown=0;

  evt_wt = 1.0;
  evt_trigger = false;
  //_campaign = "2018_UL";

  //For skimmer
  tree->SetBranchStatus("*",0);
  ActivateBranch(tree);

}

void AnaScript::SlaveTerminate()
{
  //For skimmer
  cout<<"Writing Skim File....";
  skimTree->Write();
  _SkimFile->Close();
  cout<<"   Done!  "<<endl;

  float goodevtfrac = ((float)nEvtRan)/((float)nEvtTotal);
  float trigevtfrac = ((float)nEvtTrigger)/((float)nEvtTotal);
  float passevtfrac = ((float)nEvtPass)/((float)nEvtTotal);
  float notgoldenevtfrac  = ((float)nThrown)/((float)nEvtTotal);

  cout<<"---------------------------------------------"<<endl;
  cout<<"Summary:"<<endl;
  cout<<"nEvtTotal = "<<nEvtTotal<<endl;
  cout<<"nEvtRan = "<<nEvtRan<<" ("<<goodevtfrac*100<<" %)"<<endl;
  cout<<"nEvtTrigger = "<<nEvtTrigger<<" ("<<trigevtfrac*100<<" %)"<<endl;
  cout<<"nEvtSkimmed = "<<nEvtPass<<" ("<<passevtfrac*100<<" %)"<<endl;
  if(_data!=0) cout<<"nEvents not in golden json = "<<nThrown<<" ("<<notgoldenevtfrac*100<<" %)"<<endl;
  cout<<"---------------------------------------------"<<endl;
  
  time(&end);

  double time_taken = double(end-start);
  cout<<"\nTime taken by the programe is= "<<fixed<<time_taken<<setprecision(5);
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

  ReadBranch(); //for skimmer

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

    triggerRes=true; //default, always true for MC
    //In the skimmer, I am keeping all events from data, including the overcounted ones.
    //This is because I have the control to play with the trigger paths later.

    /*
    if(_data==1){
      triggerRes=false;
      bool muon_trigger = false;
      bool electron_trigger = false;
      if     (_year==2016) {muon_trigger = (*HLT_IsoMu24==1); electron_trigger = (*HLT_Ele27_WPTight_Gsf==1);}
      else if(_year==2017) {muon_trigger = (*HLT_IsoMu27==1); electron_trigger = (*HLT_Ele32_WPTight_Gsf==1);}
      else if(_year==2018) {muon_trigger = (*HLT_IsoMu24==1); electron_trigger = (*HLT_Ele27_WPTight_Gsf==1);}

      bool overlapping_events = muon_trigger && electron_trigger;

      triggerRes = muon_trigger || electron_trigger;
      //This is the union of both datasets (may overlap).
      //Removing the overlapping events from the EGamma dataset as follows:
      if(_flag == "egamma" && overlapping_events) triggerRes = false;

      triggerRes = true;
    }*/

    if(_data !=0 ){
      //Throw awaying bad data that are not included in the GoldenJson:
      int runno = (int)*run;
      int lumisection = (int)*luminosityBlock;
      bool golden_event = checkJson(true, runno, lumisection);
      if(!golden_event){
	nThrown++;
	//cout<<"Event not in golden json: "<<nEvtTotal<<endl;
	//triggerRes = false; //Keep everything
      }
    }
    
    if(triggerRes){
      nEvtTrigger++; //only triggered events
      bad_event = false;
      
      //###################
      //Gen particle block
      //###################
 
      genMuon.clear();
      genElectron.clear();
      genLightLepton.clear();
      genJet.clear();
      vllep.clear();
      vlnu.clear();

      if(_data==0){
	//createGenLightLeptons();
	//createGenJets();
        //SortGenObjects();
		
	createSignalArrays();
	SortVLL();

	//Correcting the Doublet model (flagging out the invalid decays)
	if(_flag=="doublet"){ //for VLLD files
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

      //Counting bad events:
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
      //For Uttsavi:
      ForwardJet.clear();
      ForwardMediumbJet.clear();
      
      createLightLeptons();
      //createPhotons();
      //createTaus();
      //createJets();

      SortRecoObjects();

      //#####################
      //   Event Selection
      //#####################

      bool keep_this_event = false;

      //2LSS skim:
      if((int)LightLepton.size()==2){
	//Condition 1: SS
	bool samesign = LightLepton.at(0).charge == LightLepton.at(1).charge;
	//Condition 2: finding a triggerable object
	bool trigger = false;
	for(int i=0; i<(int)LightLepton.size(); i++){
	  int lepton_id = fabs(LightLepton.at(i).id);
	  float lepton_pt = LightLepton.at(i).v.Pt();
	  if(lepton_id == 11 && lepton_pt > 35) trigger = true;
	  if(lepton_id == 13 && lepton_pt > 26) trigger = true;
	}
	//Condition 3: low-res veto
	bool reject_low_resonances = (LightLepton.at(0).v + LightLepton.at(1).v).M() > 15;
	
	if(trigger && reject_low_resonances && samesign) keep_this_event = true;
      }

      if(bad_event) keep_this_event = false;
      
      if(keep_this_event){
	nEvtPass++;
	skimTree->Fill();
      }
      
    }//TriggeredEvts
  }//GoodEvt
  return kTRUE;
}
