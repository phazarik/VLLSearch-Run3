#define AnaScript_cxx
#include "AnaScript.h"
#include <TH2.h>
#include <TStyle.h>

//Additional headers:
#include "../includeHeaders.h"
#include "../veto3L4L.h"
#include "BookHistograms.h"
#include "make_bJetSFPlots.h"
#include "make_signalPlots.h"

void AnaScript::Begin(TTree * /*tree*/)
{
  TString option = GetOption();
}
void AnaScript::SlaveBegin(TTree *tree)
{
  TString option = GetOption();
  time(&start);

  //Set year and btagWP:
  if      (_campaign =="2016postVFP_UL")    {_year=2016; _btagWP=0.2489;}
  else if (_campaign =="2016preVFP_UL")     {_year=2016; _btagWP=0.2598;}
  else if (_campaign =="2017_UL")           {_year=2017; _btagWP=0.3040;}
  else if (_campaign =="2018_UL")           {_year=2018; _btagWP=0.2783;}
  else if (_campaign == "Run3Summer22")     {_year=2022; _btagWP=0.3086;}
  else if (_campaign == "Run3Summer22EE")   {_year=2022; _btagWP=0.3196;}
  else if (_campaign == "Run3Summer23")     {_year=2023; _btagWP=0.2431;}
  else if (_campaign == "Run3Summer23BPix") {_year=2023; _btagWP=0.2435;}
  else cout<<"main: Provide correct campaign name!"<<endl;
  //Source: https://btv-wiki.docs.cern.ch/ScaleFactors/
  
  cout<<"\nParameters:"<<endl;
  cout<<"Campaign = "<< _campaign <<endl;
  cout<<"Year   = "  << _year <<endl;
  cout<<"Sample = "  << _samplename <<endl;
  cout<<"Flag   = "  << _flag <<endl;
  cout<<"btagWP = "  << _btagWP <<endl;
  cout<<"Data   = "  << _data <<"\n"<<endl;

  //Initializing counters:
  nEvtGen=tree->GetEntries();
  nEvtTotal=0; nEvtRan=0;  nEvtTrigger=0;
  nEvtPass=0;  nEvtBad=0;  nThrown=0; nEvtVeto=0;
  
  //Loading offline data (json, text):
  jsondata = loadJson();
  LoadCorrectionsFromPOG();
  avggenweight =  LoadAvgGenWeights(_campaign, _samplename);
  lumiweight = LoadLumiWeights(_campaign, _samplename);

  bad_event = false;
  evt_trigger = false;

  _HstFile = new TFile(_HstFileName,"recreate");
  BookHistograms();

  cout << "\n"
     << right << setw(8) << "Progress"
     << right << setw(12) << "nEvents"
     << right << setw(8) << "Time" << endl;
}
void AnaScript::SlaveTerminate()
{
  _HstFile->Write();
  _HstFile->Close();

  float goodevtfrac = ((float)nEvtRan)/((float)nEvtTotal);
  float trigevtfrac = ((float)nEvtTrigger)/((float)nEvtTotal);
  float passevtfrac = ((float)nEvtPass)/((float)nEvtTotal);
  float badevtfrac  = ((float)nEvtBad)/((float)nEvtTotal);//for VLLD
  float notgoldenevtfrac  = ((float)nThrown)/((float)nEvtTotal);

  cout<<"---------------------------------------------"<<endl;
  cout<<"Summary:"<<endl;
  cout<<"nEvtTotal = "<<nEvtTotal<<endl;
  cout<<"nEvtRan = "<<nEvtRan<<" ("<<goodevtfrac*100<<" %)"<<endl;
  cout<<"nEvtTrigger = "<<nEvtTrigger<<" ("<<trigevtfrac*100<<" %)"<<endl;
  cout<<"nEvtPass = "<<nEvtPass<<" ("<<passevtfrac*100<<" %)"<<endl;
  cout<<"nEvtBad = "<<nEvtBad<<" ("<<badevtfrac*100<<" %)"<<endl;
  cout<<"nEvtVeto = "<<nEvtVeto<<endl;
  if(_data!=0) cout<<"nEvents not in golden json = "<<nThrown<<" ("<<notgoldenevtfrac*100<<" %)"<<endl;
  cout<<"---------------------------------------------"<<endl;

  time(&end);
  double time_taken = double(end-start);
  cout<<"\033[34m\nTime taken to process = " << (int)time_taken << " seconds.\033[0m"<< endl;
  cout<<" sec \n"<<endl;
}
void AnaScript::Terminate()
{

}

Bool_t AnaScript::Process(Long64_t entry)
{
  nEvtTotal++;

  //Initializing fReaders:
  fReader                .SetLocalEntry(entry);
  if (_data == 0){
    fReader_MC.SetLocalEntry(entry);
    if (_flag!="qcd") fReader_nonQCD.SetLocalEntry(entry);
  }

  //Setting verbosity:
  if (nEvtTotal % 10000 == 0) {
    time(&buffer);
    double time_buff = double(buffer-start);
    double frac = (double)nEvtTotal / nEvtGen * 100.0;
    string progress = (ostringstream() << fixed << setprecision(2) << frac << "%").str();
    cout << right << setw(8) << progress
	 << right << setw(12) << nEvtTotal
	 << right << setw(8) << fixed << setprecision(0) << time_buff <<endl;
  }

  //Filtering bad events:
  bool common  = *Flag_goodVertices && *Flag_globalSuperTightHalo2016Filter && *Flag_HBHENoiseFilter &&
    *Flag_HBHENoiseIsoFilter && *Flag_EcalDeadCellTriggerPrimitiveFilter && *Flag_BadPFMuonFilter;
  bool eeBadSc = _data ? *Flag_eeBadScFilter : 1;

  //GoodEvt2018 = (_year == 2018 ? common && eeBadSc && *Flag_BadPFMuonDzFilter && *Flag_ecalBadCalibFilter : 1);
  //GoodEvt2017 = (_year == 2017 ? common && eeBadSc && *Flag_BadPFMuonDzFilter : 1);
  GoodEvt2018 = (_year == 2018 ? common && eeBadSc && *Flag_ecalBadCalibFilter : 1);
  GoodEvt2017 = (_year == 2017 ? common && eeBadSc : 1);
  GoodEvt2016 = (_year == 2016 ? common && eeBadSc : 1);
  GoodEvt2022 = (_year == 2022 ? common && eeBadSc : 1);
  GoodEvt2023 = (_year == 2022 ? common && eeBadSc : 1);
  GoodEvt = GoodEvt2018 && GoodEvt2017 && GoodEvt2016 && GoodEvt2022 && GoodEvt2023;

  if(GoodEvt){
    nEvtRan++;

    triggerRes         = true; //default, always true for MC
    /*
    muon_trigger       = *HLT_SingleMuon;
    electron_trigger   = *HLT_SingleEle;
    overlapping_events = muon_trigger && electron_trigger;

    if(_data !=0){
      //Applying trigger on data:
      if(_flag == "muon")   triggerRes = muon_trigger; //For the SingleMuon dataset, including overlap.
      if(_flag == "egamma") triggerRes = electron_trigger && !muon_trigger; //For the EGamma dataset

      //Throw awaying bad data that are not included in the GoldenJson:
      int runno = (int)*run;
      int lumisection = (int)*luminosityBlock;
      bool golden_event = checkJson(true, runno, lumisection);
      if(!golden_event){
	nThrown++;
	triggerRes = false;
      }
    }//Applying trigger on data
    */
    if(triggerRes){
      nEvtTrigger++;

      //----------------------------------------------------------------------------------------------------------
      // OBJECT DEFINITIONS
      //----------------------------------------------------------------------------------------------------------

      //----------------
      // Reco-Particles
      //----------------
      
      metpt = *PuppiMET_pt;
      metphi = *PuppiMET_phi;
      
      Muon.clear();    Electron.clear(); LightLepton.clear();
      Photon.clear();  Tau.clear();      Jet.clear();
      bJet.clear();    MediumbJet.clear();
      LooseLepton.clear(); LooseElectron.clear(); LooseMuon.clear();
      
      createLightLeptons();
      //createTaus();
      createJets();
      SortRecoObjects();

      //-------------------------------------------------
      //Corrections to Jets: (needs to be done only once)
      //-------------------------------------------------
      double jec = 1.0;
      double jer = 1.0;
      if(_data == 0){
	for(int i=0; i<(int)Jet.size(); i++){
	  jec =  returnJetJECSF(Jet.at(i), "nom");
	  jer =  returnJetResolutionCorrection(Jet.at(i), "nom");
	  Jet.at(i).v = Jet.at(i).v * jec;
	  Jet.at(i).v = Jet.at(i).v * jer;
	}
      }

      //Gen-Particles block moved to make_signalPlots.h
      if(_data == 0) MakeSignalPlots(); //nEvtBad is here

      //----------------------------------------------------------------------------------------------------------
      // Analysis:
      //----------------------------------------------------------------------------------------------------------

      if(_data == 0) MakebJetSFPlots(); //nEvtPass is here
      
      
    }//Triggered Events
  }//GoodEvt
  return kTRUE;
}
