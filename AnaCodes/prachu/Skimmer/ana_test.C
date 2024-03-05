#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
void ana_test(int sample=1)
{
  const char *hstfilename, *sumfilename;
  const char *skimfilename; //for skimmer
  
  TChain *chain = new TChain("Events");
  AnaScript m_selec;//declared an instance of our class.
  
  //--------------------------------------------------------------------------//
  //               Choose the sample you want to run over                     //
  //--------------------------------------------------------------------------//

  if(sample==0){//Data
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/UL2018Data/SingleMuon/SingleMuon_A/SingleMuon_2018A_6.root");
    hstfilename = "test_outputs/hst_data.root";
    sumfilename = "test_outputs/sum_data.txt";
    skimfilename = "test_outputs/skimmed_data.root";
    m_selec.SetData(1); //0 - running over MC, 1 - running over Data
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1); //0-electron dataset, 1-muon dataset
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
  }
  
  else if(sample==1){//Drell-Yan
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/DYJetsToLL/M50/VLL_DYJetsToLL_M50_98.root");
    hstfilename = "test_outputs/hst_dy.root";
    sumfilename = "test_outputs/sum_dy.txt";
    skimfilename = "test_outputs/skimmed_dy.root";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
  }

  else if(sample==2){//Drell-Yan
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/QCD_MuEnriched/170to300/VLL_QCDPt170to300MuEnrichedPt5_40.root");
    hstfilename = "test_outputs/hst_qcd.root";
    sumfilename = "test_outputs/sum_qcd.txt";
    skimfilename = "test_outputs/skimmed_qcd.root";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag("qcd"); //flag=="doublet" removes invalid decay modes from VLLD files.
  }

  else if(sample==800){//Signal
    chain->Add("/home/work/ykumar1/Work/VLLAnalysis_e-muLike/Samples/Signal/2018/VLLD/ele/VLLD_ele_M800/*.root");
    hstfilename = "test_outputs/hst_vlld_ele_800.root";
    sumfilename = "test_outputs/sum_vlld_ele_800.txt";
    skimfilename = "test_outputs/skimmed_vlld_ele_800.root";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag("doublet"); //flag=="doublet" removes invalid decay modes from VLLD files.
  }
  
  else if(sample==500){//Signal
    chain->Add("/home/work/ykumar1/Work/VLLAnalysis_e-muLike/Samples/Signal/2018/VLLS/ele/VLLS_ele_M500/*.root");
    hstfilename = "test_outputs/hst_vlls_ele_500.root";
    sumfilename = "test_outputs/sum_vlls_ele_500.txt";
    skimfilename = "test_outputs/skimmed_vlls_ele_500.root";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
  }
  
  else{
    cout<<"Invalid argument!"<<endl;
  }
  std::cout<<"Output file : "<<skimfilename<<std::endl;
  m_selec.SetHstFileName(hstfilename);
  m_selec.SetSumFileName(sumfilename);
  m_selec.SetSkimFileName(skimfilename);
  m_selec.SetVerbose(0);//set verbosity level for output.
  
  //this calls the Process function for each event in the chain
  //and runs the m_selec code over it.
  chain->Process(&m_selec);
    
}
