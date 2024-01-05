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
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Studies/signal_plots.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Studies/evt_2LSS_plots.h"
#include "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/HistMaker/BookHistograms.h"

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
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/GetEventWeight.h"

void AnaScript::Begin(TTree * /*tree*/)
{
  TString option = GetOption();
}

void AnaScript::SlaveBegin(TTree * /*tree*/)
{
  time(&start);

  TString option = GetOption();
  nEvtTotal = 0;
  nEvtRan = 0;
  nEvtTrigger=0;
  nEvtPass=0;
  nEvtBad=0; 

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

  _HstFile = new TFile(_HstFileName,"recreate");
  BookHistograms();

  if(_flag=="doublet")  cout<<"Removing invalid VLLD decay modes ..."<<endl;
  else if(_flag=="qcd") cout<<"Scaling QCD files ..."<<endl;
}

void AnaScript::SlaveTerminate()
{
  _HstFile->Write();
  _HstFile->Close();

  float goodevtfrac = ((float)nEvtRan)/((float)nEvtTotal);
  float trigevtfrac = ((float)nEvtTrigger)/((float)nEvtTotal);
  float passevtfrac = ((float)nEvtPass)/((float)nEvtTotal);
  float badevtfrac  = ((float)nEvtBad)/((float)nEvtTotal);//for VLLD

  cout<<"---------------------------------------------"<<endl;
  cout<<"Summary:"<<endl;
  cout<<"nEvtTotal = "<<nEvtTotal<<endl;
  cout<<"nEvtRan = "<<nEvtRan<<" ("<<goodevtfrac*100<<" %)"<<endl;
  cout<<"nEvtTrigger = "<<nEvtTrigger<<" ("<<trigevtfrac*100<<" %)"<<endl;
  cout<<"nEvtPass = "<<nEvtPass<<" ("<<passevtfrac*100<<" %)"<<endl;
  cout<<"nEvtBad = "<<nEvtBad<<" ("<<badevtfrac*100<<" %)"<<endl;
  cout<<"---------------------------------------------"<<endl;

  cout<<"Event counts (raw):"<<endl;
  cout<<"4L   = "<<n4l<<endl;
  cout<<"3L   = "<<n3l<<endl;
  cout<<"2LSS = "<<n2lss<<endl;
  cout<<"2LOS = "<<n2los<<endl;
  cout<<"1L2J = "<<n1l2j<<endl;
  cout<<"1L1J = "<<n1l1j<<endl;
  cout<<"1L0J = "<<n1l0j<<endl;

  cout<<"\nLumiScale = "<<59800/_lumi<<endl;
  
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

  //Setting verbosity:
  //Verbosity determines the number of processed events after which
  //the root prompt is supposed to display a status update.
  if(_verbosity==0 && nEvtTotal%10000==0)cout<<"Processed "<<nEvtTotal<<" event..."<<endl;      
  else if(_verbosity>0 && nEvtTotal%10000==0)cout<<"Processed "<<nEvtTotal<<" event..."<<endl;

  nEvtTotal++;
  h.nevt->Fill(0);

  //Let's plot some flags/triiger which are used later.
  h.hist[0]->Fill(*Flag_goodVertices);
  h.hist[1]->Fill(*Flag_globalSuperTightHalo2016Filter);
  h.hist[2]->Fill(*Flag_HBHENoiseFilter);
  h.hist[3]->Fill(*Flag_EcalDeadCellTriggerPrimitiveFilter);
  h.hist[4]->Fill(*Flag_BadPFMuonFilter);
  h.hist[5]->Fill(*HLT_IsoMu24);
  h.hist[6]->Fill(*HLT_IsoMu27);
  h.hist[7]->Fill(*HLT_Ele27_WPTight_Gsf);
  h.hist[8]->Fill(*HLT_Ele32_WPTight_Gsf);
  
  GoodEvt2018 = (_year==2018 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);
  GoodEvt2017 = (_year==2017 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);
  GoodEvt2016 = (_year==2016 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);

  GoodEvt = GoodEvt2016 && GoodEvt2017 && GoodEvt2018;

  if(GoodEvt){
    nEvtRan++; //only good events
    h.nevt->Fill(1);

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
      triggerRes = muon_trigger || (!muon_trigger && electron_trigger);      
    }
    
    if(triggerRes){
      nEvtTrigger++; //only triggered events
      h.nevt->Fill(2);

      //###################
      //Gen particle block
      //###################

      genMuon.clear();
      genElectron.clear();
      genLightLepton.clear();
      vllep.clear();
      vlnu.clear();

      bad_event = false;
      
      if(_data==0){
	createGenLightLeptons();
        SortGenObjects();
	//SortPt(genMuon);
	//SortPt(genElectron);
	//SortPt(genLightLepton);
	
	createSignalArrays();
	SortVLL();

	//Correcting the Doublet model (flagging out the invalid decays)
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
      LooseLepton.clear();
      
      createLightLeptons();
      createPhotons();
      createTaus();
      createJets();

      SortRecoObjects();

      //----------------------------------------------------------------
      //Event-selection is done right after creating the object arrays.
      //evt_wt is also calculated alongwith.
      //This is done before any plotting.
      EventSelection();
      evt_wt = getEventWeight();
      //----------------------------------------------------------------
      /*
      //Basic object-level plots:
      //Electrons
      h.ele[0]->Fill((int)Electron.size());
      for(int i=0; i<(int)Electron.size(); i++){
	h.ele[1]->Fill(Electron.at(i).v.Pt(), evt_wt);
	h.ele[2]->Fill(Electron.at(i).v.Eta(), evt_wt);
	h.ele[3]->Fill(Electron.at(i).v.Phi(), evt_wt);
	h.ele[4]->Fill(Electron.at(i).reliso03, evt_wt);
      }
      //Muons
      h.mu[0]->Fill((int)Muon.size());
      for(int i=0; i<(int)Muon.size(); i++){
	h.mu[1]->Fill(Muon.at(i).v.Pt(), evt_wt);
	h.mu[2]->Fill(Muon.at(i).v.Eta(), evt_wt);
	h.mu[3]->Fill(Muon.at(i).v.Phi(), evt_wt);
	h.mu[4]->Fill(Muon.at(i).reliso03, evt_wt);
      }
      //Photons
      h.pho[0]->Fill((int)Photon.size());
      for(int i=0; i<(int)Photon.size(); i++){
	h.pho[1]->Fill(Photon.at(i).v.Pt(), evt_wt);
	h.pho[2]->Fill(Photon.at(i).v.Eta(), evt_wt);
	h.pho[3]->Fill(Photon.at(i).v.Phi(), evt_wt);
	h.pho[4]->Fill(Photon.at(i).reliso03, evt_wt);
      }
      //Taus
      h.tau[0]->Fill((int)Tau.size());
      for(int i=0; i<(int)Tau.size(); i++){
	h.tau[1]->Fill(Tau.at(i).v.Pt(), evt_wt);
	h.tau[2]->Fill(Tau.at(i).v.Eta(), evt_wt);
	h.tau[3]->Fill(Tau.at(i).v.Phi(), evt_wt);
      }
      //Jets
      h.jet[0]->Fill((int)Jet.size());
      for(int i=0; i<(int)Jet.size(); i++){
	h.jet[1]->Fill(Jet.at(i).v.Pt(), evt_wt);
	h.jet[2]->Fill(Jet.at(i).v.Eta(), evt_wt);
	h.jet[3]->Fill(Jet.at(i).v.Phi(), evt_wt);
      }
      //bJets
      h.bjet[0]->Fill((int)bJet.size());
      for(int i=0; i<(int)bJet.size(); i++){
	h.bjet[1]->Fill(bJet.at(i).v.Pt(), evt_wt);
	h.bjet[2]->Fill(bJet.at(i).v.Eta(), evt_wt);
	h.bjet[3]->Fill(bJet.at(i).v.Phi(), evt_wt);
	}*/
     
      //_______________________________________________________________________________________________________
      
      //                         Analysis block
      //_______________________________________________________________________________________________________

      //lumiscaling:
      //if(_data == 2018)	evt_wt = evt_wt*(59800/_lumi);
      double lumiscale = (59800/_lumi);
      h.evtweight[3]->Fill(lumiscale);

      //Make_evt2LSS_plots(evt_wt);
      if(evt_2LSS && evt_trigger){
	//nEvtPass++;
	h.nevt->Fill(3);
	Make_evt2LSS_plots(evt_wt); //This function is updating evt_wt for QCD
      }
      
      if(_data==0){

	if(bad_event) h.hist[9]->Fill(0);//Counting invalid events.
	else h.hist[9]->Fill(1);
	
	//Counting signal events before passing the trigger:
	//Rest of them are being filled in signal_plots.h
	if(!bad_event){
	  float wt = 1.0;
	  h.sig[1]->Fill((int)0, wt); //All events
	  h.sig[2]->Fill((int)0, wt); //All events
	  h.sig[3]->Fill((int)0, wt); //All events
	  h.sig[4]->Fill((int)0, wt); //All events
	}
	
	//if(evt_trigger) MakeSignalPlots(evt_wt);

      }
      
      
    }//TriggeredEvts
  }//GoodEvt
  return kTRUE;
}
