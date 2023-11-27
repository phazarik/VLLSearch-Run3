//##############################################################################################
// This picks the appropriate libraries automatically and runs the anaCond.C script using gROOT.
// The inputs to runana can be given from the condor submission scripts.
// While anaCond.C is used for only one root file (locally), runana.C is used to run on loops,
// without having to generate the libabary each time by doing .L *so.
// While anaCond.C sits at the directory where the library is, runana.C can sit anywhere.
//##############################################################################################

//test command:
//.x runana.C("/home/work/alaha1/public/RunII_ULSamples/2018/DYJetsToLL/M50/VLL_DYJetsToLL_M50_98.root", "/home/work/phazarik1/work/General-Analysis-template/AnaCodes/BasicEvtSelection/test_outputs/hst_dy_runana.root", "0", "2018", "1", "flag")

//.x runana.C("/home/work/ykumar1/Work/VLLAnalysis_e-muLike/Samples/Signal/2018/VLLD/ele/VLLD_ele_M800/*.root", " /home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/BasicEvtSelection/test_outputs/test_runana.root", "0", "2018", "1", "doublet")

// I will add more parameters, if needed later.

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>

void runana(TString ifname="input_file" , TString ofname="outputfile" , TString data="data", TString year="year", TString lep="lep", TString flag="flag"){

  TString path_to_anaCond ="/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/BasicEvtSelection";
  TString path_to_library =path_to_anaCond;

  //TString anastring =".x "+path_to_anaCond+"/anaCond.C(\""+ifname+"\",\""+ofname+"\",\""+data+"\",\""+year+"\",\""+lep+"\")";
  TString anastring =".x "+path_to_anaCond+"/anaCond.C(\""+ifname+"\",\""+ofname+"\",\""+data+"\",\""+year+"\",\""+lep+"\",\""+flag+"\")"; //This is the line that is being executed. 

  //cout<<"executing the following command:"<<endl;
  //cout<<anastring<<endl;
  
  gSystem->Load(path_to_library+"/AnaScript_C.so");
  gROOT->ProcessLine(anastring);
}
