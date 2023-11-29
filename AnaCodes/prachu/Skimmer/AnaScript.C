#define AnaScript_cxx
#include "AnaScript.h"
#include <TH2.h>
#include <TStyle.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

//Including the header files:
#include "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/Skimmer/skimmerHelper.h"

//DON'T CHANGE THE FOLLOWING:
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/CustomFunctions.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/EventSelection.h"
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
  nEvtSkim=0;//for skimmer

  //_HstFile = new TFile(_HstFileName,"recreate");
  //Call the function to book the histograms we declared in Hists.
  //BookHistograms();

  //For skimmer:
  //The following keep only the branches mentioned in skimmerHelper.h 
  //tree->SetBranchStatus("*",0);
  //ActivateBranch(tree);
}

void AnaScript::SlaveTerminate()
{
  //_HstFile->Write();
  //_HstFile->Close();

  //for skimmer
  skimTree->Write();
  _SkimFile->Close();

  float goodevtfrac = ((float)nEvtRan)/((float)nEvtTotal);
  float trigevtfrac = ((float)nEvtTrigger)/((float)nEvtTotal);
  float skimevtfrac = ((float)nEvtSkim)/((float)nEvtTotal);//for skimmer

  cout<<"---------------------------------------------"<<endl;
  cout<<"Summary:"<<endl;
  cout<<"Total events = "<<nEvtTotal<<endl;
  cout<<"Total events ran = "<<nEvtRan<<" ("<<goodevtfrac*100<<" %)"<<endl;
  cout<<"Total Triggered events = "<<nEvtTrigger<<" ("<<trigevtfrac*100<<" %)"<<endl;
  cout<<"nEvents left after skimming = "<<nEvtSkim<<" ("<<skimevtfrac*100<<" %)"<<endl;//for skimmer
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

  ReadBranch();//for skimmer

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

      /*
      if(_data==0){
	createGenLightLeptons();
	SortPt(genMuon);
	SortPt(genElectron);
	SortPt(genLightLepton);
	//Make gen-level plots here.
	}*/

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

      SortPt(Muon);
      SortPt(Electron);
      SortPt(LightLepton);
      SortPt(Photon);
      SortPt(Tau);
      SortPt(Jet);
      SortPt(bJet);

      /*
      //Basic object-level plots:
      //ELectrons
      h.ele[0]->Fill((int)Electron.size());
      for(int i=0; i<(int)Electron.size(); i++){
	h.ele[1]->Fill(Electron.at(i).v.Pt());
	h.ele[2]->Fill(Electron.at(i).v.Eta());
	h.ele[3]->Fill(Electron.at(i).v.Phi());
      }
      //Muons
      h.mu[0]->Fill((int)Muon.size());
      for(int i=0; i<(int)Muon.size(); i++){
	h.mu[1]->Fill(Muon.at(i).v.Pt());
	h.mu[2]->Fill(Muon.at(i).v.Eta());
	h.mu[3]->Fill(Muon.at(i).v.Phi());
	h.mu[4]->Fill(Muon.at(i).reliso04);
      }
      //Photons
      h.pho[0]->Fill((int)Photon.size());
      for(int i=0; i<(int)Photon.size(); i++){
	h.pho[1]->Fill(Photon.at(i).v.Pt());
	h.pho[2]->Fill(Photon.at(i).v.Eta());
	h.pho[3]->Fill(Photon.at(i).v.Phi());
	h.pho[4]->Fill(Photon.at(i).reliso04);
      }
      //Taus
      h.tau[0]->Fill((int)Tau.size());
      for(int i=0; i<(int)Tau.size(); i++){
	h.tau[1]->Fill(Tau.at(i).v.Pt());
	h.tau[2]->Fill(Tau.at(i).v.Eta());
	h.tau[3]->Fill(Tau.at(i).v.Phi());
      }
      //Jets
      h.jet[0]->Fill((int)Jet.size());
      for(int i=0; i<(int)Jet.size(); i++){
	h.jet[1]->Fill(Jet.at(i).v.Pt());
	h.jet[2]->Fill(Jet.at(i).v.Eta());
	h.jet[3]->Fill(Jet.at(i).v.Phi());
      }
      //bJets
      h.bjet[0]->Fill((int)bJet.size());
      for(int i=0; i<(int)bJet.size(); i++){
	h.bjet[1]->Fill(bJet.at(i).v.Pt());
	h.bjet[2]->Fill(bJet.at(i).v.Eta());
	h.bjet[3]->Fill(bJet.at(i).v.Phi());
	}*/

      //_______________________________________________________________________________________________________
      
      //Skimmer
      //_______________________________________________________________________________________________________

      //Event selection:
      if(true){ //Any addional cuts, if needed, can be applied here.

	EventSelection();

	//For a particular final state, fillup the tree.
	//Edit the funtion while changing the final state,
	//otherwise it will give a segmentation error.

	if(evt_2LSS){
	  nEvtSkim++;
	  skimTree->Fill();
	}

      }
      
    }//TriggeredEvts
  }//GoodEvt
  return kTRUE;
}
