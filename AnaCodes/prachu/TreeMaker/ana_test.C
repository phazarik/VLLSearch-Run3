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
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(0); //0-electron dataset, 1-muon dataset
    m_selec.SetFlag(""); //flag == "" by default. It does nothing.
    m_selec.SetSampleName("EGamma_A");
    m_selec.SetLumi(59800);
  }

  if(sample==0){//SingleMuon Data
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/UL2018Data/SingleMuon/SingleMuon_A/SingleMuon_2018A_6.root");
    treefilename = "test_outputs/tree_singlemuon.root";
    m_selec.SetData(1); //0 - running over MC, 1 - running over Data
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1); //0-electron dataset, 1-muon dataset
    m_selec.SetFlag("");
    m_selec.SetSampleName("SingleMuon_A");
    m_selec.SetLumi(59800);
  }
  
  else if(sample==1){//Drell-Yan
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/DYJetsToLL/M50/VLL_DYJetsToLL_M50_98.root");
    treefilename = "test_outputs/tree_dy.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag("dy");
    m_selec.SetSampleName("DYJetsToLL_M50");
    m_selec.SetLumi(59800);
  }

  else if(sample==2){//ZZ
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/ZZ/ZZTo4L/VLL_ZZTo4L_29.root");
    treefilename = "test_outputs/tree_zz.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag("");
    m_selec.SetSampleName("ZZTo4L");
    m_selec.SetLumi(59800);
  }

  else if(sample==800){//Signal
    chain->Add("/home/work/ykumar1/Work/VLLAnalysis_e-muLike/Samples/Signal/2018/VLLD/ele/VLLD_ele_M800/*.root");
    //hstfilename = "test_outputs/hst_vlld_ele_800.root";
    //sumfilename = "test_outputs/sum_vlld_ele_800.txt";
    treefilename = "test_outputs/tree_vlld_ele_800.root";
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
    treefilename = "test_outputs/tree_vlls_ele_500.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("VLLS_ele_M500");
    m_selec.SetLumi(59800);
  }

  else if(sample==-2){//Testing skimmed version.
    chain->Add("../Skimmer/test_outputs/*.root");
    treefilename = "test_outputs/tree_from_skim.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("blah");
    m_selec.SetLumi(59800);
  }

  //Testing on skim:
  else if(sample==999){
    chain->Add("/home/work/phazarik1/work/CondorDump/hadded/skim_2LSS_2018UL_Oct15/skim_DYJetsToLL_M50.root");
    treefilename = "test_outputs/tree_DYJetsToLL_M50_skimmed.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(2);
    m_selec.SetLep(1);
    m_selec.SetFlag("dy"); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("DYJetsToLL_M50");
    m_selec.SetLumi(30321.155);
  }
  else if(sample==998){
    chain->Add("/home/work/phazarik1/work/CondorDump/hadded/skim_2LSS_2018UL_Oct15/skim_QCD_MuEnriched_170to300.root");
    treefilename = "test_outputs/tree_QCD_MuEnriched_170to300.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(2);
    m_selec.SetLep(1);
    m_selec.SetFlag("qcd"); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("QCD_MuEnriched_170to300");
    m_selec.SetLumi(30321.155);
  }
  else if(sample==-999){
    chain->Add("/home/work/phazarik1/work/CondorDump/hadded/skim_2LSS_2018UL_Oct15/skim_EGamma_EGamma_*.root");
    treefilename = "test_outputs/tree_EGamma_skimmed.root";
    m_selec.SetData(1);
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(0);
    m_selec.SetFlag("egamma"); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("EGamma_A");
    m_selec.SetLumi(59800);
  }
  else if(sample==-998){
    chain->Add("/home/work/phazarik1/work/CondorDump/hadded/skim_2LSS_2018UL_Oct15/skim_SingleMuon_*.root");
    treefilename = "test_outputs/tree_SingleMuon_skimmed.root";
    m_selec.SetData(1);
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(0);
    m_selec.SetFlag("singlemuon"); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("SingleMuon_A");
    m_selec.SetLumi(59800);
  }
  //------------------------------------------------------------------------------------------------------------
  //Testing on samples from other campaigns
  else if(sample==2017){
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2017/DYJetsToLL/M50/VLL_DYJetsToLL_M50_148.root");
    treefilename = "test_outputs/tree_DYJetsToLL_M50_2017_UL.root";
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
    treefilename = "test_outputs/tree_DYJetsToLL_M50_2016preVFP_UL.root";
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
    treefilename = "test_outputs/tree_DYJetsToLL_M50_2016postVFP_UL.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2016postVFP_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("DYJetsToLL_M50");
    m_selec.SetLumi(59800);
  }
  //--------------------------
  // Legacy sample (nanoAODv7)
  //--------------------------
  else if(sample==700){
    chain->Add("/home/work/phazarik1/public/RunII_LegacySamples/2016/VLLS/M100/F5557D8A-DA95-5D4C-91C6-40DBEFDA79E3.root");
    treefilename = "test_outputs/tree_VLLS_tau_M-100_legacy.root";
    m_selec.SetData(0);
    m_selec.SetCampaign("2018_UL");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag("vlls_tau"); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("VLLS_tau_M-100");
    m_selec.SetLumi(59800);
  }
  //--------------------------
  // Run3-2022 (nanoAODv12)
  //--------------------------
  //dataset=/SingleMuon/Run2022B-22Sep2023-v1/NANOAOD
  else if(sample==2022){
    chain->Add("/home/work/phazarik1/work/Run3TestSamples/SingleMuon/Run2022B-22Sep2023-v1/6fffbfce-4abe-4efb-992b-242e383bb328.root");
    treefilename = "test_outputs/tree_Data_2022.root";
    m_selec.SetData(1);
    m_selec.SetCampaign("Summer22");
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("SingleMuon");
    m_selec.SetLumi(59800);
  }
  else{
    cout<<"Invalid argument!"<<endl;
  }

  std::cout<<"\nOutput file is "<<treefilename<<std::endl;
  m_selec.SetTreeFileName(treefilename);
  m_selec.SetVerbose(0);//set verbosity level for output.
  
  //this calls the Process function for each event in the chain
  //and runs the m_selec code over it.
  chain->Process(&m_selec);
    
}
