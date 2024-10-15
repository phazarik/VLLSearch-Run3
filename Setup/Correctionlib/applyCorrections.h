#include "muon.h"
#include "egamma.h"
#include "jetmet.h"
#include "btv_deepjet.h"
#include "pileup.h"

double AnaScript::correctinlib_leptonSF(Particle lepton, string mode){
  int id = fabs(lepton.id);
  double sf = 1.0;
  
  if(id == 11)       sf = correctionlib_egmIDSF(lepton, mode);
  else if (id == 13) sf = correctionlib_muonIDSF(lepton, mode)*correctionlib_muonIsoSF(lepton, mode);

  return sf;
}

