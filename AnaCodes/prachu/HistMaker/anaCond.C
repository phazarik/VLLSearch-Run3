//#########################################################################################################
// Instead of the regular ana.C, this anaCond.C is sent to the condor job.
// It still requires the library to be loaded by doing .L *.so beforehand.
// It takes input and output filenames and some other parameters as arguments.
// This driver script is run using runana.C, which uses gROOT to run this from outside the root terminal.
//#########################################################################################################

//Execution line :
// .x anaCond.C("/home/work/phazarik1/work/CondorDump/output/skim_2LSS_Dec11/DYJetsToLL_M50_2023-12-11/*.root", "test_outputs/test_anaCond.root", "0", "2018", "mu", "doublet", "", 30321.155)

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>
#include <string>
//#include <boost/lexical_cast.hpp>// for lexical_cast()

void anaCond( TString ifname , TString ofname, TString data, TString year, TString lep, TString flag, TString lumi)
{
  //Description of the parameters:
  //1. ifname : Input file name with full path.
  //2. ofname : Output file name with full path.
  //3. data   : 0, or 1 depedning on whether 'ifname' is MC or data.
  //4. year   : Which year of data taking. (affects trigger paths and efficiencies)
  //5. lep    : If muon dataset, lep=1, if electron dataset, lep=0

  gROOT->Time();
  const char *hstfilename;
  TChain *chain = new TChain("Events");
  AnaScript m_selec;

  //Setting parameters from the input arguments:
  TString input = ifname;
  bool manual = input.EndsWith(".root");
  bool single=false;
  if(!manual) input += "/*.root"; //This makes sure that the input filenames always end with .root
  chain->Add(input);
  
  hstfilename = ofname;  

  //SetHstFileName:
  m_selec.SetHstFileName(hstfilename);

  //SetVerbose:
  m_selec.SetVerbose(1);

  //SetData:
  if(data=="0") m_selec.SetData(0);
  if(data=="1") m_selec.SetData(1);
 
  //SetYear:
  if(year=="2016") m_selec.SetYear(2016);
  if(year=="2017") m_selec.SetYear(2017);
  if(year=="2018") m_selec.SetYear(2018);

  //SetEra
  //m_selec.SetEra(1);

  //SetLep:
  //This parameter decides whether we are working with muon or electron dataset:
  m_selec.SetLep(1);
  if(lep=="el") m_selec.SetLep(0);
  if(lep=="mu") m_selec.SetLep(1);

  //Set the additional flag:
  m_selec.SetFlag(flag);

  //Set lumi:
  //double lumival = lumi.Atof();
  std::string lumistring(lumi.Data()); 
  double lumival = std::stod(lumistring);
  m_selec.SetLumi(lumival);
  
  chain->Process(&m_selec);
  gROOT->Time();
}
