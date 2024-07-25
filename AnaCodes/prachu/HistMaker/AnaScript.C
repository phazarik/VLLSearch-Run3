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
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Studies/targeting_2muss.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Studies/gen_study.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Studies/bJetScaleFactorCalculator.h"
#include "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/HistMaker/BookHistograms.h"
//#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Others/forUttsavi.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Studies/basic_study.h"

//DON'T CHANGE THE FOLLOWING:
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/CustomFunctions.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/EventSelection.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/ProduceGenCollection.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/ProduceRecoCollection.h"

/*
//Old Corrections
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ApplyCorrections.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ScaleFactors/ScaleFactors_2016UL_preVFP.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ScaleFactors/ScaleFactors_2016UL_postVFP.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ScaleFactors/ScaleFactors_2017UL.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ScaleFactors/ScaleFactors_2018UL.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/bJetCorrections/JetEff_DeepJet_MediumWP_UL2018.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/TriggerEfficiency.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/GetEventWeight.h"
*/

//Correctionlib:
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/applyCorrections.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Correctionlib/local/TriggerEfficiency.h"

void AnaScript::Begin(TTree * /*tree*/)
{
  TString option = GetOption();
}

void AnaScript::SlaveBegin(TTree * /*tree*/)
{
  time(&start);
  cout<<"\nn-events \t time_taken (sec)"<<endl;

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
  n2muss=0;
  n2ess=0;
  nemuss=0;

  //For Uttsavi
  nbasicpass=0;
  nadvancedpass=0;
  nbasicpass2=0;
  nadvancedpass2=0;

  evt_wt = 1.0;
  bad_event = false;
  evt_trigger = false;

  //---------------------------------------------------
  //Set Campaign manually : (important for corrections)
  //_campaign = "2018_UL";
  //_campaign = "2017_UL";
  //_campaign = "2016preVFP_UL";
  //_campaign = "2016postVFP_UL";
  //---------------------------------------------------

  _HstFile = new TFile(_HstFileName,"recreate");
  BookHistograms();

  if(_flag=="doublet")  cout<<"Removing invalid VLLD decay modes ..."<<endl;
  else if(_flag=="qcd") cout<<"Scaling QCD files ..."<<endl;
  else if(_flag=="dy") cout<<"Scaling DY files ..."<<endl;
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
  cout<<"Additional counters:"<<endl;
  cout<<"n4l   = "<<   n4l<<endl;
  cout<<"n3l   = "<<   n3l<<endl;
  cout<<"------------------"<<endl;
  cout<<"n2lss = "<< n2lss<<endl;
  cout<<n2muss<<" + "<<n2ess<<" + "<<nemuss<<endl;
  cout<<"------------------"<<endl;
  cout<<"n2los = "<< n2los<<endl;
  cout<<"n1l2j = "<< n1l2j<<endl;
  cout<<"n1l1j = "<< n1l1j<<endl;
  cout<<"n1l0j = "<< n1l0j<<endl;
    
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

    triggerRes         = true; //default, always true for MC
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
    
    //Checking trigger flags before:
    if(muon_trigger)       h.count[0]->Fill(0); else h.count[0]->Fill(1);
    if(electron_trigger)   h.count[0]->Fill(2); else h.count[0]->Fill(3);
    if(overlapping_events) h.count[0]->Fill(4);

    //Investigaring the electron trigger:
    if(*HLT_Ele27_WPTight_Gsf==1) h.count[2]->Fill(0);    else h.count[2]->Fill(1);
    if(*HLT_Ele32_WPTight_Gsf==1) h.count[2]->Fill(2);    else h.count[2]->Fill(3);
    if(*HLT_Ele27_WPTight_Gsf==1 && *HLT_Ele32_WPTight_Gsf==1) h.count[2]->Fill(4); //bothpass
    if(*HLT_Ele27_WPTight_Gsf==0 && *HLT_Ele32_WPTight_Gsf==0) h.count[2]->Fill(5); //bothfail
    if(*HLT_Ele27_WPTight_Gsf==1 && *HLT_Ele32_WPTight_Gsf==0) h.count[2]->Fill(6); //only 27 pass
    if(*HLT_Ele27_WPTight_Gsf==0 && *HLT_Ele32_WPTight_Gsf==1) h.count[2]->Fill(7); //only 32 pass

    //Update triggerRes only in case of data.
    if(_data==1){
      triggerRes = (muon_trigger || electron_trigger);
      //This is the union of both datasets (may overlap).
      //Removing the overlapping events from the EGamma dataset as follows:
      if(_flag == "egamma" && overlapping_events) triggerRes = false;
    }
    
    if(triggerRes){
      nEvtTrigger++; //only triggered events
      h.nevt->Fill(2);

      //Checking trigger flags after:
      if(muon_trigger)       h.count[1]->Fill(0); else h.count[1]->Fill(1);
      if(electron_trigger)   h.count[1]->Fill(2); else h.count[1]->Fill(3);
      if(overlapping_events) h.count[1]->Fill(4);

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
      MediumbJet.clear();
      LooseLepton.clear();
      //For Uttsavi:
      ForwardJet.clear();
      ForwardMediumbJet.clear();
      
      createLightLeptons();
      createPhotons();
      createTaus();
      createJets();

      SortRecoObjects();

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
      /*
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
	}*/
      //Jets
      h.jet[0]->Fill((int)Jet.size());
      for(int i=0; i<(int)Jet.size(); i++){
	h.jet[1]->Fill(Jet.at(i).v.Pt(), evt_wt);
	h.jet[2]->Fill(Jet.at(i).v.Eta(), evt_wt);
	h.jet[3]->Fill(Jet.at(i).v.Phi(), evt_wt);
      }
      //bJets
      h.bjet[0]->Fill((int)MediumbJet.size());
      for(int i=0; i<(int)MediumbJet.size(); i++){
	h.bjet[1]->Fill(MediumbJet.at(i).v.Pt(), evt_wt);
	h.bjet[2]->Fill(MediumbJet.at(i).v.Eta(), evt_wt);
	h.bjet[3]->Fill(MediumbJet.at(i).v.Phi(), evt_wt);
      }
      MakeBasicEvtPlots();

      //----------------------------------------------------------------
      //Event-selection is done right after creating the object arrays.
      //evt_wt is also calculated alongwith.
      //This is done before any plotting.

      EventSelection(); //This is where trigger is applied.
      //if(_data==0) evt_wt = getEventWeight(); //Event weight is set for MC only.
      //else evt_wt = 1.0;
      //---------------------------------------------------------------- 
            
     
      //_______________________________________________________________________________________________________
      
      //                         Analysis block
      //_______________________________________________________________________________________________________

      //Investigating 2muSS:
      //Make2LSSPlots();
      //MakebJetSFPlots();
      if(!bad_event) MakeSignalPlots(1.0);

      /*
      //lumiscaling:
      //if(_data == 2018)	evt_wt = evt_wt*(59800/_lumi);
      double lumiscale = (59800/_lumi);
      h.evtweight[3]->Fill(lumiscale);

      //Make_evt2LSS_plots(evt_wt);
      if(evt_2LSS && evt_trigger){
	//nEvtPass++;
	h.nevt->Fill(3);
	Make_evt2LSS_plots(evt_wt); //This function is updating evt_wt for QCD
	if(_data==0) Make_gen2LSS_plots(evt_wt);
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

	}*/
      
      
    }//TriggeredEvts
  }//GoodEvt
  return kTRUE;
}
