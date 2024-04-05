#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
void ana_test(int sample=0)
{
  const char *hstfilename, *sumfilename; 
  TChain *chain = new TChain("Events");
  AnaScript m_selec;//declared an instance of our class.
  
  //--------------------------------------------------------------------------//
  //               Choose the sample you want to run over                     //
  //--------------------------------------------------------------------------//

  if(sample==-1){//EGamma data
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/UL2018Data/EGamma/EGamma_A/EGamma_2018A_116.root");
    hstfilename = "test_outputs/hst_data_egamma.root";
    sumfilename = "test_outputs/sum_data_egamma.txt";
    m_selec.SetData(1); //0 - running over MC, 1 - running over Data
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(0); //0-electron dataset, 1-muon dataset
    m_selec.SetFlag(""); //flag == "" by default. It does nothing.
    m_selec.SetSampleName("EGamma_A");
    m_selec.SetLumi(59800);
  }

  if(sample==0){//SingleMuon Data
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/UL2018Data/SingleMuon/SingleMuon_A/SingleMuon_2018A_6.root");
    hstfilename = "test_outputs/hst_data_singlemuon.root";
    sumfilename = "test_outputs/sum_data_singlemuon.txt";
    m_selec.SetData(1); //0 - running over MC, 1 - running over Data
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1); //0-electron dataset, 1-muon dataset
    m_selec.SetFlag("");
    m_selec.SetSampleName("SingleMuon_A");
    m_selec.SetLumi(59800);
  }
  
  else if(sample==1){//Drell-Yan
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/DYJetsToLL/M50/VLL_DYJetsToLL_M50_98.root");
    hstfilename = "test_outputs/hst_dy.root";
    sumfilename = "test_outputs/sum_dy.txt";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag("");
    m_selec.SetSampleName("DYJetsToLL_M50");
    m_selec.SetLumi(59800);
  }

  else if(sample==2){//QCD
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/QCD_MuEnriched/170to300/VLL_QCDPt170to300MuEnrichedPt5_5.root");
    hstfilename = "test_outputs/hst_qcd.root";
    sumfilename = "test_outputs/sum_qcd.txt";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag("");
    m_selec.SetSampleName("QCD_MuEnriched_170to300");
    m_selec.SetLumi(59800);
  }

  else if(sample==3){//TTbarSemileptonic
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/TTBar/TTToSemiLeptonic/NanoPost_TTToSemiLeptonic_TuneCP5_97.root");
    hstfilename = "test_outputs/hst_ttbarsemi.root";
    sumfilename = "test_outputs/sum_ttbarsemi.txt";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag("");
    m_selec.SetSampleName("TTToSemiLeptonic");
    m_selec.SetLumi(59800);
  }

  else if(sample==4){//TTbarDilep
    chain->Add("/home/work/alaha1/public/RunII_ULSamples/2018/TTBar/TTTo2L2Nu/NanoPost_TTTo2L2Nu_TuneCP5_9.root");
    hstfilename = "test_outputs/hst_ttbardilep.root";
    sumfilename = "test_outputs/sum_ttbardilep.txt";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag("");
    m_selec.SetSampleName("TTTo2L2Nu");
    m_selec.SetLumi(59800);
  }

  else if(sample==800){//Signal
    chain->Add("/home/work/ykumar1/Work/VLLAnalysis_e-muLike/Samples/Signal/2018/VLLD/ele/VLLD_ele_M800/*.root");
    hstfilename = "test_outputs/hst_vlld_ele_800.root";
    sumfilename = "test_outputs/sum_vlld_ele_800.txt";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
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
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("VLLS_ele_M500");
    m_selec.SetLumi(59800);
  }

  else if(sample==100){//Signal
    chain->Add("/home/work/ykumar1/Work/VLLAnalysis_e-muLike/Samples/Signal/2018/VLLS/ele/VLLS_ele_M100/*.root");
    hstfilename = "test_outputs/hst_vlls_ele_100.root";
    sumfilename = "test_outputs/sum_vlls_ele_100.txt";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("VLLS_ele_M100");
    m_selec.SetLumi(59800);
  }

  else if(sample==101){//Signal
    chain->Add("/home/work/ykumar1/Work/VLLAnalysis_e-muLike/Samples/Signal/2018/VLLS/mu/VLLS_mu_M100/*.root");
    hstfilename = "test_outputs/hst_vlls_mu_100.root";
    sumfilename = "test_outputs/sum_vlls_mu_100.txt";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("VLLS_mu_M100");
    m_selec.SetLumi(59800);
  }
  
  //Testing on skim:
  else if(sample==999){//Testing skimmed version.
    chain->Add("/home/work/phazarik1/work/CondorDump/output/skim_2muSS_Mar05_Baseline/DYJetsToLL_M50_2024-03-05/*.root");
    hstfilename = "test_outputs/hst_DYJetsToLL_M50_skimmed.root";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(2);
    m_selec.SetLep(1);
    m_selec.SetFlag("dy"); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("DYJetsToLL_M50");
    m_selec.SetLumi(30321.155);
  }
  else if(sample==998){//Testing skimmed version.
    chain->Add("/home/work/phazarik1/work/CondorDump/output/skim_2muSS_Mar05_Baseline/QCD_MuEnriched_170to300_2024-03-05/*.root");
    hstfilename = "test_outputs/hst_qcd_skimmed.root";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag("qcd"); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("QCD_MuEnriched_170to300");
    m_selec.SetLumi(10265.815);
  }
  else if(sample==-999){//Testing skimmed version.
    chain->Add("/home/work/phazarik1/work/CondorDump/output/skim_2muSS_Mar05_Baseline/EGamma_EGamma_A_2024-03-05/*.root");
    hstfilename = "test_outputs/hst_EGammaA_skimmed.root";
    m_selec.SetData(1);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(0);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("EGamma_A");
    m_selec.SetLumi(59800);
  }
  else if(sample==666){//Testing skimmed version.
    chain->Add("/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/Skimmer/test_outputs/*.root");
    hstfilename = "test_outputs/hst_local_skimmed.root";
    m_selec.SetData(0);
    m_selec.SetYear(2018);
    m_selec.SetMCwt(1);
    m_selec.SetLep(1);
    m_selec.SetFlag(""); //flag=="doublet" removes invalid decay modes from VLLD files.
    m_selec.SetSampleName("DYJetsToLL_M50");
    m_selec.SetLumi(59800);
 }
  else{
    cout<<"Invalid argument!"<<endl;
  }

  std::cout<<"Output : "<<hstfilename<<std::endl;
  m_selec.SetHstFileName(hstfilename);
  //m_selec.SetSumFileName(sumfilename);
  m_selec.SetVerbose(0);//set verbosity level for output.
  
  //this calls the Process function for each event in the chain
  //and runs the m_selec code over it.
  chain->Process(&m_selec);
    
}
