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
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ApplyCorrections.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ScaleFactors/ScaleFactors_2016UL_preVFP.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ScaleFactors/ScaleFactors_2016UL_postVFP.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ScaleFactors/ScaleFactors_2017UL.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ScaleFactors/ScaleFactors_2018UL.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/TriggerEfficiency.h"


void AnaScript::Begin(TTree * /*tree*/)
{
  TString option = GetOption();
}

void AnaScript::SlaveBegin(TTree *tree /*tree*/)
{
  time(&start);

  TString option = GetOption();
  nEvtTotal = 0;
  nEvtRan = 0;
  nEvtTrigger=0;
  nEvtPass=0;

  //Counters:
  n2l=0; n2l_2e0mu=0; n2l_1e1mu=0; n2l_0e2mu=0;
  n3l=0; n3l_3e0mu=0; n3l_2e1mu=0; n3l_1e2mu=0; n3l_0e3mu=0;
  n4l=0;

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

  cout<<"---------------------------------------------"<<endl;
  cout<<"Summary:"<<endl;
  cout<<"nEvtTotal = "<<nEvtTotal<<endl;
  cout<<"nEvtRan = "<<nEvtRan<<" ("<<goodevtfrac*100<<" %)"<<endl;
  cout<<"nEvtTrigger = "<<nEvtTrigger<<" ("<<trigevtfrac*100<<" %)"<<endl;
  cout<<"nEvtPass = "<<nEvtPass<<" ("<<passevtfrac*100<<" %)"<<endl;
  cout<<"---------------------------------------------"<<endl;
  
  time(&end);

  double time_taken = double(end-start);
  cout<<"Time taken by the programe is= "<<fixed<<time_taken<<setprecision(5);
  cout<<"sec"<<endl;
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

  //Setting verbosity:
  //Verbosity determines the number of processed events after which
  //the root prompt is supposed to display a status update.
  if(_verbosity==0 && nEvtTotal%10000==0)cout<<"Processed "<<nEvtTotal<<" event..."<<endl;      
  else if(_verbosity>0 && nEvtTotal%10000==0)cout<<"Processed "<<nEvtTotal<<" event..."<<endl;

  nEvtTotal++;
  //h.nevt->Fill(0);

  /*
  //Let's plot some flags/triiger which are used later.
  h.hist[0]->Fill(*Flag_goodVertices);
  h.hist[1]->Fill(*Flag_globalSuperTightHalo2016Filter);
  h.hist[2]->Fill(*Flag_HBHENoiseFilter);
  h.hist[3]->Fill(*Flag_EcalDeadCellTriggerPrimitiveFilter);
  h.hist[4]->Fill(*Flag_BadPFMuonFilter);
  h.hist[5]->Fill(*HLT_IsoMu24);
  h.hist[6]->Fill(*HLT_IsoMu27);
  h.hist[7]->Fill(*HLT_Ele27_WPTight_Gsf);
  h.hist[8]->Fill(*HLT_Ele32_WPTight_Gsf);*/
  
  GoodEvt2018 = (_year==2018 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);
  GoodEvt2017 = (_year==2017 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);
  GoodEvt2016 = (_year==2016 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);

  GoodEvt = GoodEvt2016 && GoodEvt2017 && GoodEvt2018;

  if(GoodEvt){
    nEvtRan++; //only good events
    //h.nevt->Fill(1);

    triggerRes=true; //Always true for MC

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
      triggerRes = muon_trigger || (!muon_trigger && electron_trigger);      
    }
    
    if(triggerRes){
      nEvtTrigger++; //only triggered events
      //h.nevt->Fill(2);

      //###################
      //Gen particle block
      //###################

      genMuon.clear();
      genElectron.clear();
      genLightLepton.clear();

      if(_data==0){
	createGenLightLeptons();
	SortPt(genMuon);
	SortPt(genElectron);
	SortPt(genLightLepton);

	SortGenObjects();
	//Make gen-level plots here.
      }

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
      LooseLepton.clear();
      
      createLightLeptons();
      createPhotons();
      createTaus();
      createJets();

      SortRecoObjects();

      /*
      //Basic object-level plots:
      //ELectrons
      h.ele[0]->Fill((int)Electron.size());
      for(int i=0; i<(int)Electron.size(); i++){
	h.ele[1]->Fill(Electron.at(i).v.Pt(), evtwt);
	h.ele[2]->Fill(Electron.at(i).v.Eta(), evtwt);
	h.ele[3]->Fill(Electron.at(i).v.Phi(), evtwt);
	h.ele[4]->Fill(Electron.at(i).reliso03, evtwt);
      }
      //Muons
      h.mu[0]->Fill((int)Muon.size());
      for(int i=0; i<(int)Muon.size(); i++){
	h.mu[1]->Fill(Muon.at(i).v.Pt(), evtwt);
	h.mu[2]->Fill(Muon.at(i).v.Eta(), evtwt);
	h.mu[3]->Fill(Muon.at(i).v.Phi(), evtwt);
	h.mu[4]->Fill(Muon.at(i).reliso03, evtwt);
      }
      //Photons
      h.pho[0]->Fill((int)Photon.size());
      for(int i=0; i<(int)Photon.size(); i++){
	h.pho[1]->Fill(Photon.at(i).v.Pt(), evtwt);
	h.pho[2]->Fill(Photon.at(i).v.Eta(), evtwt);
	h.pho[3]->Fill(Photon.at(i).v.Phi(), evtwt);
	h.pho[4]->Fill(Photon.at(i).reliso03, evtwt);
      }
      //Taus
      h.tau[0]->Fill((int)Tau.size());
      for(int i=0; i<(int)Tau.size(); i++){
	h.tau[1]->Fill(Tau.at(i).v.Pt(), evtwt);
	h.tau[2]->Fill(Tau.at(i).v.Eta(), evtwt);
	h.tau[3]->Fill(Tau.at(i).v.Phi(), evtwt);
      }
      //Jets
      h.jet[0]->Fill((int)Jet.size());
      for(int i=0; i<(int)Jet.size(); i++){
	h.jet[1]->Fill(Jet.at(i).v.Pt(), evtwt);
	h.jet[2]->Fill(Jet.at(i).v.Eta(), evtwt);
	h.jet[3]->Fill(Jet.at(i).v.Phi(), evtwt);
      }
      //bJets
      h.bjet[0]->Fill((int)bJet.size());
      for(int i=0; i<(int)bJet.size(); i++){
	h.bjet[1]->Fill(bJet.at(i).v.Pt(), evtwt);
	h.bjet[2]->Fill(bJet.at(i).v.Eta(), evtwt);
	h.bjet[3]->Fill(bJet.at(i).v.Phi(), evtwt);
	}*/

      //_______________________________________________________________________________________________________
      
      // Applying corrections like SF, trigger efficiency etc. to the MC
      //_______________________________________________________________________________________________________      


      evtwt = 1.0; //Default value
      if(_data==0){

	float scalefactor = 1.0;
	float triggeff = 1.0;
	
	//Apply corrections on MC
	if((int)LightLepton.size()>=3){ //3L or more
	  float lep0SF = LeptonIDSF(LightLepton.at(0).id, LightLepton.at(0).v.Pt(), LightLepton.at(0).v.Eta());
	  float lep1SF = LeptonIDSF(LightLepton.at(1).id, LightLepton.at(1).v.Pt(), LightLepton.at(1).v.Eta());
	  float lep2SF = LeptonIDSF(LightLepton.at(2).id, LightLepton.at(2).v.Pt(), LightLepton.at(2).v.Eta());
	  scalefactor = lep0SF * lep1SF * lep2SF;

	  float e1=SingleLepTrigger_eff(LightLepton.at(0).id, LightLepton.at(0).v.Pt(), LightLepton.at(0).v.Eta());
	  float e2=SingleLepTrigger_eff(LightLepton.at(1).id, LightLepton.at(1).v.Pt(), LightLepton.at(1).v.Eta());
	  float e3=SingleLepTrigger_eff(LightLepton.at(2).id, LightLepton.at(2).v.Pt(), LightLepton.at(2).v.Eta());
	  triggeff=1-((1-e1)*(1-e2)*(1-e3));
	}
	else if((int)LightLepton.size()==2){ //2L exclusive
	  float lep0SF = LeptonIDSF(LightLepton.at(0).id, LightLepton.at(0).v.Pt(), LightLepton.at(0).v.Eta());
	  float lep1SF = LeptonIDSF(LightLepton.at(1).id, LightLepton.at(1).v.Pt(), LightLepton.at(1).v.Eta());
	  scalefactor = lep0SF * lep1SF;
	  
	  float e1=SingleLepTrigger_eff(LightLepton.at(0).id, LightLepton.at(0).v.Pt(), LightLepton.at(0).v.Eta());
	  float e2=SingleLepTrigger_eff(LightLepton.at(1).id, LightLepton.at(1).v.Pt(), LightLepton.at(1).v.Eta());
	  triggeff=1-((1-e1)*(1-e2));
	}
	else if((int)LightLepton.size()==1){//1L events:
	  scalefactor = LeptonIDSF(LightLepton.at(0).id, LightLepton.at(0).v.Pt(), LightLepton.at(0).v.Eta());
	  triggeff = SingleLepTrigger_eff(LightLepton.at(0).id, LightLepton.at(0).v.Pt(), LightLepton.at(0).v.Eta());
	}

	evtwt = scalefactor * triggeff;

	h.evtweight[0]->Fill(scalefactor);
	h.evtweight[1]->Fill(triggeff);
	h.evtweight[2]->Fill(evtwt);
      }
     
      //_______________________________________________________________________________________________________
      
      //                         Analysis block
      //_______________________________________________________________________________________________________


      //Event selection:
      if(true){

	EventSelection();

	//For a particular final state, fillup the tree.
	//Edit the funtion while changing the final state,
	//otherwise it will give a segmentation error.

	if(evt_2LSS){
	  
	  //UInt_t nlep;
	  //nlep = (UInt_t)LightLepton.size();
	  //mytree->Branch("nlep", &nlep);
	  
	  FillTree(mytree);	  
	  mytree->Fill();
	}

      }  

      
    }//TriggeredEvts
  }//GoodEvt
  return kTRUE;
}
