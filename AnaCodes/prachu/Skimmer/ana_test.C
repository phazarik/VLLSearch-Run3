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
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1); //0-electron dataset, 1-muon dataset
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("SingleMuon_A");
    m_selec.SetLumi(59800);
  }
  
  else if(sample==1){//Drell-Yan
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/DYJetsToLL/M50/VLL_DYJetsToLL_M50_98.root");
    hstfilename = "test_outputs/hst_dy.root";
    sumfilename = "test_outputs/sum_dy.txt";
    skimfilename = "test_outputs/skimmed_dy.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("DYJetsToLL_M50");
    m_selec.SetLumi(59800);
  }

  else if(sample==2){//Drell-Yan
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/QCD_MuEnriched/170to300/VLL_QCDPt170to300MuEnrichedPt5_40.root");
    hstfilename = "test_outputs/hst_qcd.root";
    sumfilename = "test_outputs/sum_qcd.txt";
    skimfilename = "test_outputs/skimmed_qcd.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag("qcd"); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("QCD_MuEnriched_170to300");
    m_selec.SetLumi(59800);
  }

  else if(sample==800){//Signal
    chain->Add("/home/work/ykumar1/Work/VLLAnalysis_e-muLike/Samples/Signal/2018/VLLD/ele/VLLD_ele_M800/*.root");
    hstfilename = "test_outputs/hst_vlld_ele_800.root";
    sumfilename = "test_outputs/sum_vlld_ele_800.txt";
    skimfilename = "test_outputs/skimmed_vlld_ele_800.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag("doublet"); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("VLLD_ele_M800");
    m_selec.SetLumi(59800);
  }
  
  else if(sample==500){//Signal
    chain->Add("/home/work/ykumar1/Work/VLLAnalysis_e-muLike/Samples/Signal/2018/VLLS/ele/VLLS_ele_M500/*.root");
    hstfilename = "test_outputs/hst_vlls_ele_500.root";
    sumfilename = "test_outputs/sum_vlls_ele_500.txt";
    skimfilename = "test_outputs/skimmed_vlls_ele_500.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("VLLS_ele_M500");
    m_selec.SetLumi(59800);
  }
  //------------------------------------------------------------------------------------------------------------
  //Testing on samples from other campaigns
  else if(sample==2017){
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2017/DYJetsToLL/M50/VLL_DYJetsToLL_M50_148.root");
    skimfilename = "test_outputs/skim_DYJetsToLL_M50_2017_UL.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2017_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("DYJetsToLL_M50");
    m_selec.SetLumi(59800);
  }
  else if(sample==20161){
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2016/DYJetsToLL/preVFP/M50/VLL_DYJetsToLLM50_preVFP_2.root");
    skimfilename = "test_outputs/skim_DYJetsToLL_M50_2016preVFP_UL.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2016preVFP_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("DYJetsToLL_M50");
    m_selec.SetLumi(59800);
  }
  else if(sample==20162){
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2016/DYJetsToLL/postVFP/M50/VLL_DYJetsToLLM50_postVFP_18.root");
    skimfilename = "test_outputs/skim_DYJetsToLL_M50_2016postVFP_UL.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2016postVFP_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("DYJetsToLL_M50");
    m_selec.SetLumi(59800);
  }
  else{
    cout<<"Invalid argument!"<<endl;
  }

  //For testing purposesL
  //Extract Number of events:
  TFile *tfile = new TFile("/home/work/alaha1/public/RunII_ULSamples/2018/DYJetsToLL/M50/VLL_DYJetsToLL_M50_98.root", "READ");
  TTree *ttree = (TTree *)tfile->Get("Events");
  TBranch *br = ttree->GetBranch("run");
  int nevt = br->GetEntries();
  std::cout<<"Total no of events : (test) "<<nevt<<std::endl;
  
  std::cout<<"Output file : "<<skimfilename<<std::endl;
  m_selec.SetHstFileName(hstfilename);
  m_selec.SetSumFileName(sumfilename);
  m_selec.SetSkimFileName(skimfilename);
  m_selec.SetVerbose(0);//set verbosity level for output.
  
  //this calls the Process function for each event in the chain
  //and runs the m_selec code over it.
  chain->Process(&m_selec);
    
}
