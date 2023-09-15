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
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/BookHistograms.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/CustomFunctions.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/ProduceGenCollection.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/ProduceRecoCollection.h"

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

  _HstFile = new TFile(_HstFileName,"recreate");
  //Call the function to book the histograms we declared in Hists.
  BookHistograms();
}

void AnaScript::SlaveTerminate()
{
  _HstFile->Write();
  _HstFile->Close();

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
  if(_verbosity==0 && nEvtTotal%100000==0)cout<<"Processed "<<nEvtTotal<<" event..."<<endl;      
  else if(_verbosity>0 && nEvtTotal%100000==0)cout<<"Processed "<<nEvtTotal<<" event..."<<endl;

  nEvtTotal++;
  h.nevt->Fill(0);

  GoodEvt2018 = (_year==2018 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);
  GoodEvt2017 = (_year==2017 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);
  GoodEvt2016 = (_year==2016 ? *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter && *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter && (_data ? *Flag_eeBadScFilter : 1) : 1);

  GoodEvt = GoodEvt2016 && GoodEvt2017 && GoodEvt2018;

  if(GoodEvt){
    nEvtRan++; //only good events
    h.nevt->Fill(1);

    triggerRes=true; //Always true for MC

    if(_data==1){
      trigger2018 = (_year==2018 ? (_lep==1 ? *HLT_IsoMu24==1 : _lep==0 && *HLT_Ele32_WPTight_Gsf) : 1);
      //trigger2017 = (_year==2017 ? (_lep==1 ? *HLT_IsoMu27==1 : _lep==0 && (*HLT_Ele32_WPTight_Gsf||*HLT_Ele32_WPTight_Gsf_L1DoubleEG)) : 1);
      trigger2017 = (_year==2017 ? (_lep==1 ? *HLT_IsoMu27==1 : _lep==0 && (*HLT_Ele32_WPTight_Gsf)) : 1);
      trigger2016 = (_year==2016 ? (_lep==1 ? (*HLT_IsoMu24==1) : _lep==0 && *HLT_Ele27_WPTight_Gsf) : 1);
           
      triggerRes = trigger2018 && trigger2017 && trigger2016;
      //triggerRes = true;
    }
    if(triggerRes){
      nEvtTrigger++; //only triggered events
      h.nevt->Fill(2);

      //------------------------------------------------------
      // Applying corrections like SF, trigger efficiency etc.
      //------------------------------------------------------
      evtwt = 1.0; //Default value
      if(_data==0){
	//Apply corrections on MC
	evtwt = 1.0;
      }

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
      
      createLightLeptons();
      createPhotons();
      createTaus();
      createJets();

      SortPt(Muon);
      SortPt(Electron);
      SortPt(LightLepton);
      SortPt(Photon);
      SortPt(Tau);
      SortPt(Jet);
      SortPt(bJet);

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
      }

      //_______________________________________________________________________________________________________      
      //       Analysis block
      //_______________________________________________________________________________________________________


      //Event selection:
      if(true){
	nEvtPass++;
	h.nevt->Fill(3);
      }  

      
    }//TriggeredEvts
  }//GoodEvt
  return kTRUE;
}
