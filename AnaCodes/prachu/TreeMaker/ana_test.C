#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
void ana_test(int sample=0)
{
  const char *treefilename; 
  TChain *chain = new TChain("Events");
  AnaScript m_selec;//declared an instance of our class.
  
  //--------------------------------------------------------------------------//
  //               Choose the sample you want to run over                     //
  //--------------------------------------------------------------------------//

  if(sample==-1){//EGamma data
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/UL2018Data/EGamma/EGamma_A/EGamma_2018A_116.root");
    treefilename = "test_outputs/tree_egamma.root";
    m_selec.SetData(1); //0 - running over MC, 1 - running over Data
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(0); //0-electron dataset, 1-muon dataset
    m_selec.SetFlag(""); //flag == "" by default. It does nothing.
  }

  if(sample==0){//SingleMuon Data
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/UL2018Data/SingleMuon/SingleMuon_A/SingleMuon_2018A_6.root");
    treefilename = "test_outputs/tree_singlemuon.root";
    m_selec.SetData(1); //0 - running over MC, 1 - running over Data
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1); //0-electron dataset, 1-muon dataset
    m_selec.SetFlag("");
  }
  
  else if(sample==1){//Drell-Yan
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/DYJetsToLL/M50/VLL_DYJetsToLL_M50_98.root");
    treefilename = "test_outputs/tree_dy.root";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag("");
  }

  else if(sample==800){//Signal
    chain->Add("/home/work/ykumar1/Work/VLLAnalysis_e-muLike/Samples/Signal/2018/VLLD/ele/VLLD_ele_M800/*.root");
    //hstfilename = "test_outputs/hst_vlld_ele_800.root";
    //sumfilename = "test_outputs/sum_vlld_ele_800.txt";
    treefilename = "test_outputs/tree_vlld_ele_800.root";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag("doublet"); //flag=="doublet" removes invalid decay modes from VLLD files.
  }
  else{
    cout<<"Invalid argument!"<<endl;
  }

  std::cout<<"Output file is "<<treefilename<<std::endl;
  m_selec.SetTreeFileName(treefilename);
  m_selec.SetVerbose(0);//set verbosity level for output.
  
  //this calls the Process function for each event in the chain
  //and runs the m_selec code over it.
  chain->Process(&m_selec);
    
}