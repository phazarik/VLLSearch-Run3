//-----------------------------------------------------
// Feeding parameters and running the compiled library.
//-----------------------------------------------------

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>
#include <filesystem>
#include <time.h>
using namespace std;

void compile_and_run(
		     TString mode = "TreeMaker",
		     //TString infile = "nanoaod_test.root",
		     TString infile = "/mnt/d/work/MC_samples_skimmed/skimmed_2LSS_2018_UL/DYto2L/50toInf/*.root",
		     TString outfile = "tree_test.root",
		     TString campaign = "2018_UL",
		     TString sample = "dy",
		     TString flag = "dy"
		     )
{

  TString thisdir = TString(std::filesystem::path(__FILE__).parent_path().string());
  TString codedir = thisdir + "/"+mode;
  
  TString arguments = "\""+infile+"\",\""+outfile+"\",\""+campaign+"\",\""+sample+"\",\""+flag+"\"";
  TString processline = ".x "+codedir+"/run.C(" + arguments + ")"; //testing purposes

  //Compiling and loading the source code:
  time_t compile_start, compile_end;
  cout<<"Loading the library ..."<<endl;
  time(&compile_start);
  gSystem->CompileMacro(codedir+"/AnaScript.C", "k");
  //gSystem->Load(sourceCodeDir+"/AnaScript_C.so");
  time(&compile_end);
  cout<<"Loading success!"<<endl;
  auto dt = compile_end-compile_start;
  cout<<"\033[34mTime taken to load = "<<(int)dt<<" seconds.\033[0m"<<endl;

  gROOT->ProcessLine(processline);
}
