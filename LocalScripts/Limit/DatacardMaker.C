#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <string>
#include <iomanip>
#include <math.h>
#include <cmath>

using namespace std;

void DatacardMaker(string signame, string bkgname, string outname, int nbins)
{
  // input file
  ifstream sigfile; ifstream bkgfile;
  sigfile.open(signame);
  bkgfile.open(bkgname);
  ofstream fout(outname);

  //variables used
  vector<vector<float>> sigregs; //Signal Regions
  sigregs.clear();
  vector<float> sigreg;
  int bin;
  float sig, dsig, data, bkg, dbkg, stob;

  // Reading the input files, and storing them in arrays.
  // Each row contains these numbers: sig, dsig, data, bkg, dbkg, stob
  // The number of rows = number of bins.
  for (int iBin = 0; iBin <= nbins; iBin++) {    
    sigfile >> bin >> sig >> dsig;    
    bkgfile >> bin >> data >> bkg >> dbkg;
    //Signal scaled by 1/10000
    //sig *= 0.000138; // L = 138 fb-1 = 138*1000 pb-1;   (Sig*B)th = 1 pb;   (AE) = Nsig / 100000
    if(sig == 0) sig = 0.0000001;
    if(bkg == 0) continue; //Reject bins with zero background.
    
    stob = (float) sig/sqrt(bkg);
    
    sigreg = {(float)bin, sig, data, bkg, dbkg, stob};
    sigregs.push_back(sigreg);
  }

  //Sorting the rows as descending orders of S/sqrt{B}
  for(int i=0; i<(int)sigregs.size()-1; i++){
    for(int j=i+1; j<(int)sigregs.size(); j++){
      if( sigregs.at(i).at(5) < sigregs.at(j).at(5) ) swap(sigregs.at(i),sigregs.at(j));
    }
  }

  //--------------------
  //Making the datacard
  //--------------------
  fout << "imax " << nbins << " \t\t\t # number of channels" << endl;
  fout << "jmax 1 \t\t\t # number of backgrounds" << endl;
  fout << "kmax " << nbins << " \t\t\t # number of nuisance parameters" << endl;
  fout << "------------" << endl;

  //Block Data
  fout << setw(15) << left << "bin";
  for(int iBin=0; iBin < nbins; iBin++){
    fout << "\t" << right << "bin" << iBin+1;
  }
  
  fout << endl << setw(15) << left << "observation";
  for(int iBin=0; iBin < nbins; iBin++){
    fout << "\t" << right << sigregs.at(iBin).at(2);
  }
  fout << endl << "------------" << endl;

  //Block Signal and Bkg
  fout << setw(15) << left << "bin";
  for(int iBin=0; iBin < nbins; iBin++){
    fout << "\t" << right << "bin" << iBin+1 << "\t" << right << "bin" << iBin+1;
  }
  
  fout << endl << setw(15) << left << "process";
  for(int iBin=0; iBin < nbins; iBin++){
    fout << "\t" << right << "sig" << "\t" << right << "bkg";
  }
  
  fout << endl << setw(15) << left << "process";
  for(int iBin=0; iBin < nbins; iBin++){
    fout << "\t" << right << (-1)*(iBin+1) << "\t" << right << iBin+1;
  }
  
  fout << endl << setw(15) << left << "rate";
  for(int iBin=0; iBin < nbins; iBin++){
    fout << "\t" << right << sigregs.at(iBin).at(1) << "\t" << right << sigregs.at(iBin).at(3);
  }
  fout << endl << "------------" << endl;

  //Block dBkg
  for(int iBin=0; iBin < nbins; iBin++){
    fout << "xs" << setw(6) << left << iBin+1 << setw(7) << left << "lnN";
    for(int jBin=0; jBin<nbins; jBin++){
      if(jBin == iBin) fout << "\t-\t" << (float)1 + (float)sigregs.at(iBin).at(4)/sigregs.at(iBin).at(3);
      else fout << "\t-\t-";
    }
    fout << endl;
  } 

  fout.close();
}
