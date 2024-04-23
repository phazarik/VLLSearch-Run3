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

//Corrections:
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ApplyCorrections.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ScaleFactors/ScaleFactors_2016UL_preVFP.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ScaleFactors/ScaleFactors_2016UL_postVFP.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ScaleFactors/ScaleFactors_2017UL.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/ScaleFactors/ScaleFactors_2018UL.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/TriggerEfficiency.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/GetEventWeight.h"
#include "/home/work/phazarik1/work/Analysis-Run3/Setup/Corrections/bJetCorrections/JetEff_DeepJet_MediumWP_UL2018.h"

void AnaScript::Begin(TTree * /*tree*/)
{
  TString option = GetOption();
}

void AnaScript::SlaveBegin(TTree *tree /*tree*/)
{
  time(&start);
  cout<<"\nn-events \t time_taken (sec)"<<endl;

  TString option = GetOption();
  nEvtTotal = 0;
  nEvtRan = 0;
  nEvtTrigger=0;
  nEvtPass=0;
  nEvtBad=0; //Events flagged out because of invalid decay

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

  cout<<"---------------------------------------------"<<endl;
  cout<<"Summary:"<<endl;
  cout<<"nEvtTotal = "<<nEvtTotal<<endl;
  cout<<"nEvtRan = "<<nEvtRan<<" ("<<goodevtfrac*100<<" %)"<<endl;
  cout<<"nEvtTrigger = "<<nEvtTrigger<<" ("<<trigevtfrac*100<<" %)"<<endl;
  cout<<"nEvtPass = "<<nEvtPass<<" ("<<passevtfrac*100<<" %)"<<endl;
  cout<<"nEvtBad = "<<nEvtBad<<" ("<<badevtfrac*100<<" %)"<<endl;
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

  fReader.SetLocalEntry(entry);
  if(_data == 0)
    fReader_MC  .SetLocalEntry(entry);
  if(_data == 1)
    fReader_Data.SetLocalEntry(entry);

  //Setting verbosity:
  time(&buffer);
  double time_taken_so_far = double(buffer-start);
  if(_verbosity==0 && nEvtTotal%1000==0)     cout<<nEvtTotal<<" \t "<<time_taken_so_far<<endl;
  else if(_verbosity>0 && nEvtTotal%1000==0) cout<<nEvtTotal<<" \t "<<time_taken_so_far<<endl;

  nEvtTotal++;
  
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
      if(_flag == "egamma" && muon_trigger) triggerRes = false; //To stop overcounting in the EGamma dataset.
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
      vllep.clear();
      vlnu.clear();

      bad_event = false;
      if(_data==0){
	//createGenLightLeptons();
	//SortGenObjects();
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
      
      createLightLeptons();
      createPhotons();
      createTaus();
      createJets();

      SortRecoObjects();
     
      //_______________________________________________________________________________________________________
      
      //                         Analysis block
      //_______________________________________________________________________________________________________

      //Selecting 2muSS events to fill:
      bool basic_evt_selection = false;
  
      if((int)Muon.size()==2 && (int)Electron.size()==0){
	if(Muon.at(0).v.Pt()>26 && Muon.at(0).charge == Muon.at(1).charge)
	  basic_evt_selection = true;
      }

      bool isolated_leptons = false;
      bool excluding_low_stats = false;
      bool baseline = false;
      bool highST = false;

      if(basic_evt_selection){

	//Defining local variables for event selection. (Not for filling the tree)
	float dR_ = Muon.at(0).v.DeltaR(Muon.at(1).v);
	TLorentzVector dilep_ = Muon.at(0).v + Muon.at(1).v;
	float dilepeta_ = dilep_.Eta();
	float HT_=0; for(int i=0; i<(int)Jet.size(); i++)  HT_ = HT_ +  Jet.at(i).v.Pt();
	float LT_= dilep_.Pt();//Muon.at(0).v.Pt() + Muon.at(1).v.Pt();
	float ST_ = HT_+LT_+metpt;
	float STfrac_ = LT_/ST_;
      
	isolated_leptons = Muon.at(0).reliso03<0.15 && Muon.at(1).reliso03<0.20;
	excluding_low_stats = fabs(dilepeta_)<3 && (0.4<dR_ && dR_<4.0) && metpt>20 && STfrac_<0.8 && HT_<500;
	baseline = (Muon.at(0).v + Muon.at(1).v).M() > 20 && excluding_low_stats;
	highST = baseline && isolated_leptons && ST_>150;
		   
      }

      if(highST){
	nEvtPass++;

	//Calculating event weight:
	wt = 1.0;
	
	double scalefactor = 1.0;
	double triggereff = 1.0;
	double bjeteff = 1.0;
	if(_data==0){
	  double sf0 = Muon_2018UL_Reco(Muon.at(0).v.Pt(), Muon.at(0).v.Eta());
	  double sf1 = Muon_2018UL_Reco(Muon.at(1).v.Pt(), Muon.at(1).v.Eta());
	  scalefactor = sf0*sf1;
	  double ef0 = TrigEff_2018_IsoMu24_Data(Muon.at(0).v.Pt(), Muon.at(0).v.Eta());
	  double ef1 = TrigEff_2018_IsoMu24_Data(Muon.at(1).v.Pt(), Muon.at(1).v.Eta());
	  triggereff = 1-((1-ef0)*(1-ef1));
	  wt = scalefactor*triggereff;
	  //Corrections for Jets:
	  double jeteff = 1.0;
	  bjeteff = bTagEff_UL2018(Jet, 0); //Tweaking the SF from POG by 0 %
	  wt = wt*bjeteff;
	}
	
	//Calculating other variables:
	FillTree(mytree);

	//Filling the tree with all these values:
	mytree->Fill();
      }
      
      
    }//TriggeredEvts
  }//GoodEvt
  return kTRUE;
}
