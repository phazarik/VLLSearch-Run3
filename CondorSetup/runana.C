//##############################################################################################
// This picks the appropriate libraries automatically and runs the anaCond.C script using gROOT.
// The inputs to runana can be given from the condor submission scripts.
// While anaCond.C is used for only one root file (locally), runana.C is used to run on loops,
// without having to generate the libabary each time by doing .L *so.
// While anaCond.C sits at the directory where the library is, runana.C can sit anywhere.
//##############################################################################################

//.x runana.C("/home/work/alaha1/public/RunII_ULSamples/2018/DYJetsToLL/M50/VLL_DYJetsToLL_M50_98.root", "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/HistMaker/test_outputs/test_runana.root", "0", "2018_UL", "1", "dy", "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/HistMaker", "30321.155", "DYJetsToLL_M50")
//OR
//.x runana.C("/home/work/alaha1/public/RunII_ULSamples/2018/DYJetsToLL/M50/VLL_DYJetsToLL_M50_98.root", "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/Skimmer/test_outputs/test_runana.root", "0", "2018_UL", "1", "dy", "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/Skimmer", "30321.155", "DYJetsToLL_M50")
//OR
//.x runana.C("/home/work/alaha1/public/RunII_ULSamples/2018/DYJetsToLL/M50/VLL_DYJetsToLL_M50_98.root", "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/TreeMaker/test_outputs/test_runana.root", "0", "2018_UL", "1", "dy", "/home/work/phazarik1/work/Analysis-Run3/AnaCodes/prachu/TreeMaker", "30321.155", "DYJetsToLL_M50")

// I will add more parameters, if needed later.

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>

void runana(TString ifname, TString ofname, TString data, TString campaign, TString lep, TString flag, TString codedir, TString lumi, TString samplename){

  //#######################
  // DO NOT TOUCH ANYTHING
  //#######################

  //Loading correctionlib:
  gInterpreter->AddIncludePath("/home/work/phazarik1/.local/lib/python3.9/site-packages/correctionlib/include");
  gSystem->Load("/home/work/phazarik1/.local/lib/python3.9/site-packages/correctionlib/lib/libcorrectionlib.so");
  
  TString path_to_anaCond = codedir;
  TString path_to_library =path_to_anaCond;
  //TString anastring =".x "+path_to_anaCond+"/anaCond.C(\""+ifname+"\",\""+ofname+"\",\""+data+"\",\""+year+"\",\""+lep+"\",\""+flag+"\")";
  TString anastring = ".x "+path_to_anaCond+"/anaCond.C(\""+ifname+"\",\""+ofname+"\",\""+data+"\",\""+campaign+"\",\""+lep+"\",\""+flag+"\",\""+lumi+"\",\""+samplename+"\")"; //This is the line that is being executed.

  gSystem->Load(path_to_library+"/AnaScript_C.so");
  gROOT->ProcessLine(anastring);

  std::cout<<"Input file  = "<<ifname<<endl;
  std::cout<<"Output file = "<<ofname<<"\n"<<endl;

}
