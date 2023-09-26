void AnaScript::SortPt(vector<Particle> part){
  for(int i=0; i<(int)part.size()-1; i++){
    for(int j=i+1; j<(int)part.size(); j++){
      if( part[i].v.Pt() < part[j].v.Pt() ) swap(part[i],part[j]); }}
}

float AnaScript::delta_phi(float phi1, float phi2){
  phi1 = TVector2::Phi_0_2pi(phi1);
  phi2 = TVector2::Phi_0_2pi(phi2);
  float dphi = fabs(phi1 - phi2);
  if(dphi>TMath::Pi()) dphi = 2*TMath::Pi() - dphi;
  return dphi;
}

int AnaScript::MotherID(int partindex, int momindex){
  int parid =GenPart_pdgId[partindex];
  int momid = GenPart_pdgId[momindex];
  while(parid==momid){
    partindex=momindex;
    momindex=GenPart_genPartIdxMother[momindex];
    parid =GenPart_pdgId[partindex];
    momid = GenPart_pdgId[momindex];
  }
  return momid;
}

float AnaScript::transv_mass(float lepE, float lepphi, float met, float metphi){
  float dphi = fabs(lepphi - metphi);
  if(dphi>TMath::Pi()) dphi = 2*TMath::Pi() - dphi;
  float mT = sqrt(2* lepE * met *(1-cos(dphi)));
  return mT;
}

//Cleaning funtions:
bool AnaScript::clean_from_array(Particle target, vector<Particle> array, float dRcut){
  bool result = true; //cleaned
  float dRmin = 1000;
  for(int i=0; i<(int)array.size(); i++){
    float dR = target.v.DeltaR(array.at(i).v);
    if(dR < dRmin) dRmin = dR;
  }
  //If the minimum dR of any Particle from the array with the target Particle is less than the dRcut,
  //then the target is not isolated.
  if(dRmin < dRcut) result = false;
  return result;
}

TString AnaScript::ParticleName(int id){
  TString name = "--";
  bool leptons = 11<=fabs(id) && fabs(id)<=18;
  bool neutrinos = leptons && id%2 ==0;
  bool quarks = fabs(id) <= 8;

  if(id == 0) name = "--";
  else if(quarks)       name = "q";
  else if(fabs(id)==11 || fabs(id)==12) name = "e";
  else if(fabs(id)==13 || fabs(id)==14) name = "mu";
  else if(fabs(id)==15 || fabs(id)==16) name = "tau";
  else if(fabs(id)==17 || fabs(id)==18) name = "vll";
  //bosons
  else if(id==21) name = "pho";
  else if(id==22) name = "glu";
  else if(id==23) name = "Z";
  else if(id==24) name = "W+";
  else if(id==-24) name = "W-";
  else if(id==25) name = "H";
  else if(id > 100) name = "had";
  
  //Additional:
  if(neutrinos) name = "nu_"+name;
  if(quarks && id>0) name = name+"+";
  if(quarks && id<0) name = name+"-";
  if(leptons && id>0 && fabs(id%2) == 1) name = name+"-";
  if(leptons && id<0 && fabs(id%2) == 1) name = name+"+";

  if(fabs(id) > 10000) name = "--";

  return name;
}
