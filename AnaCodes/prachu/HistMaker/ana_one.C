//----------------------------
// Test-run on a single file
//----------------------------

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
void ana_one(TString filepath)
{
  const char *hstfilename; 
  TChain *chain = new TChain("Events");
  AnaScript m_selec;//declared an instance of our class.
  
  chain->Add(filepath);
  hstfilename = "test_outputs/hst_test.root";
  m_selec.SetData(1);
  m_selec.SetCampaign("2016preVFP_UL");
  m_selec.SetMCwt(1);
  m_selec.SetLep(0);
  m_selec.SetFlag("");
  m_selec.SetSampleName("test");
  m_selec.SetLumi(59800);
  
  std::cout<<"\nOutput : "<<hstfilename<<std::endl;
  m_selec.SetHstFileName(hstfilename);
  m_selec.SetVerbose(0);
  
  chain->Process(&m_selec);
    
}
