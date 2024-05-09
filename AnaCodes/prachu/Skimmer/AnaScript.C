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

void AnaScript::Begin(TTree * /*tree*/)
{
  TString option = GetOption();
}

void AnaScript::SlaveBegin(TTree *tree)
{
  time(&start);
  cout<<"\nn-events \t time_taken (sec)"<<endl;
  batch_index = 0;
  batch_size = 10000;
  
  TString option = GetOption();
  nEvtTotal = 0;
  nEvtRan = 0;
  nEvtTrigger=0;
  nEvtPass=0;
  nEvtBad=0; 

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

  cout<<"---------------------------------------------"<<endl;
  cout<<"Summary:"<<endl;
  cout<<"nEvtTotal = "<<nEvtTotal<<endl;
  cout<<"nEvtRan = "<<nEvtRan<<" ("<<goodevtfrac*100<<" %)"<<endl;
  cout<<"nEvtTrigger = "<<nEvtTrigger<<" ("<<trigevtfrac*100<<" %)"<<endl;
  cout<<"nEvtSkimmed = "<<nEvtPass<<" ("<<passevtfrac*100<<" %)"<<endl;
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

  fReader.SetLocalEntry(entry);
  if(_data == 0)
    fReader_MC  .SetLocalEntry(entry);
  if(_data == 1)
    fReader_Data.SetLocalEntry(entry);

  ReadBranch(); //for skimmer

  //Setting verbosity:
  time(&buffer);
  double time_taken_so_far = double(buffer-start);
  if(_verbosity==0 && nEvtTotal%10000==0)     cout<<nEvtTotal<<" \t "<<time_taken_so_far<<endl;
  else if(_verbosity>0 && nEvtTotal%10000==0) cout<<nEvtTotal<<" \t "<<time_taken_so_far<<endl;

  //Setting year (overriding SetYear());
  if(_campaign == "2018_UL") _year = 2018;
  else if(_campaign == "2017_UL") _year = 2017;
  else if((_campaign == "2016preVFP_UL") || (_campaign == "2016postVFP_UL")) _year = 2016;
  else cout<<"main: Provide correct campaign name"<<endl;

  nEvtTotal++;
  
  GoodEvt2018 = (_year==2018 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);
  GoodEvt2017 = (_year==2017 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);
  GoodEvt2016 = (_year==2016 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);

  GoodEvt = GoodEvt2016 && GoodEvt2017 && GoodEvt2018;

  if(GoodEvt){
    nEvtRan++; //only good events

    triggerRes=true; //default, always true for MC

    if(_data==1){
      triggerRes=false;
      bool muon_trigger = false;
      bool electron_trigger = false;
      if     (_year==2016) {muon_trigger = (*HLT_IsoMu24==1); electron_trigger = (*HLT_Ele27_WPTight_Gsf==1);}
      else if(_year==2017) {muon_trigger = (*HLT_IsoMu27==1); electron_trigger = (*HLT_Ele32_WPTight_Gsf==1);}
      else if(_year==2018) {muon_trigger = (*HLT_IsoMu24==1); electron_trigger = (*HLT_Ele27_WPTight_Gsf==1);}

      //Muons are preferrred over electrons.
      //For the electron dataset, pick up only those events which do not fire a Muon trigger.
      //Otherwise there will be overcounting.
      triggerRes = muon_trigger || (!muon_trigger && electron_trigger); //The union of two sets.
      if(_flag == "egamma" && muon_trigger) triggerRes = false; //To stop overcounting in the EGamma dataset.
      //triggerRes = electron_trigger;
    }
    
    if(triggerRes){
      nEvtTrigger++; //only triggered events
      
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
	bool samesign = LightLepton.at(0).charge == LightLepton.at(1).charge;
	bool trigger = false;
	if(fabs(LightLepton.at(0).id)==11 && LightLepton.at(0).v.Pt()>32) trigger = true;
	if(fabs(LightLepton.at(0).id)==13 && LightLepton.at(0).v.Pt()>26) trigger = true;
	bool reject_low_resonances = (LightLepton.at(0).v + LightLepton.at(1).v).M() > 15;
	
	if(samesign && trigger && reject_low_resonances) keep_this_event = true;
      }

      /*
      //2muSS skim:
      if((int)Muon.size()==2 && (int)Electron.size()==0){
	bool reject_low_resonances = (Muon.at(0).v + Muon.at(1).v).M() > 15;
	bool samesign = Muon.at(0).charge == Muon.at(1).charge;
	bool ptcut = Muon.at(0).v.Pt()>26;	
	bool basic_cuts = samesign && ptcut && reject_low_resonances;
	if(basic_cuts) keep_this_event = true;
	}*/
      
      if(keep_this_event){
	nEvtPass++;
	skimTree->Fill();
      }
      
    }//TriggeredEvts
  }//GoodEvt
  return kTRUE;
}
