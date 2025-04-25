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
		     TString infile  = "/mnt/d/work/skimmed_2LSS_Run3Summer22EE/DYto2L/50toInf/*.root",
		     TString outfile = "test_outputs/tree_testcompile.root",
		     TString campaign = "Run3Summer22EE",
		     TString sample = "DYJetsToLL_M50",
		     TString flag = "dy"
		     )
{

  TString sourceCodeDir = ".";  //Set code directory here.
  
  TString arguments = "\""+infile+"\",\""+outfile+"\",\""+campaign+"\",\""+sample+"\",\""+flag+"\"";
  TString processline = ".x "+sourceCodeDir+"/run.C(" + arguments + ")"; //testing purposes

  //Compiling and loading the source code:
  time_t compile_start, compile_end;
  cout<<"Compiling the source code ..."<<endl;
  time(&compile_start);
  gSystem->CompileMacro(sourceCodeDir+"/AnaScript.C", "k");
  time(&compile_end);
  cout<<"Compilation success!"<<endl;
  auto dt = compile_end-compile_start;
  cout<<"\033[34mTime taken to compile = "<<(int)dt<<" seconds.\033[0m"<<endl;

  gROOT->ProcessLine(processline);
}
