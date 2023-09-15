//########################
// Gen Light Leptons:
//########################

void AnaScript::createGenLightLeptons(){

  int grandmomid=-1;

  for(unsigned int i=0; i< (*nGenPart); i++){
    Particle temp;
    temp.v.SetPtEtaPhiM(GenPart_pt[i],GenPart_eta[i],GenPart_phi[i],GenPart_mass[i]);
    temp.status = GenPart_status[i];
    temp.ind = i;
    temp.pdgid = GenPart_pdgId[i];
    temp.momid= MotherID(i,GenPart_genPartIdxMother[i]);

    int lepcharge=0;
    if(GenPart_pdgId[i]>0) lepcharge = -1; else lepcharge = 1;
    temp.charge = lepcharge;

    //General flags:
    bool promptdecay= abs(temp.momid)==15 ||abs(temp.momid)==23||abs(temp.momid)==24||abs(temp.momid)==25||(abs(temp.momid)>0 && abs(temp.momid)<7);
    //Setting very specific decay modes:
    bool WZbosondecay=false;
    bool ttzdecay=false;
    if(promptdecay){
      if(abs(temp.momid)==24 || abs(temp.momid)==23|| abs(temp.momid)==15){
	ttzdecay=true;
	int grandmomid=MotherID(GenPart_genPartIdxMother[i],GenPart_genPartIdxMother[GenPart_genPartIdxMother[i]]);
	//cout<<"GrandMom ID="<<grandmomid<<endl;
	if((abs(grandmomid)>0 && abs(grandmomid)<7))
	  WZbosondecay=true;    //Interested in very specific decay: W->l nu, Z->ll, mother is W or Z and grandmother should be a quark
      }
    }
    
    //##############
    //genMuon block:
    //##############
    bool passcutmu= abs(temp.pdgid)==13 && temp.status==1 && temp.v.Pt()>5 && fabs(temp.v.Eta())<2.4;
    bool motherisnotmu= abs(temp.momid)!=13 && GenPart_pdgId[GenPart_genPartIdxMother[i]]!=22;
    passcutmu = passcutmu && motherisnotmu && promptdecay; 
    if(passcutmu){     	
      genMuon.push_back(temp);
      genLightLepton.push_back(temp);
    }

    //##################
    //genElectron block:
    //##################
    bool passcutele= abs(temp.pdgid)==11 && temp.status==1 && temp.v.Pt()>5 && fabs(temp.v.Eta())<2.4;
    bool motherisnotele= abs(temp.momid)!=11 && GenPart_pdgId[GenPart_genPartIdxMother[i]]!=22;
    passcutele = passcutele && motherisnotele && promptdecay; 
    if(passcutele){
      genElectron.push_back(temp);
      genLightLepton.push_back(temp);
    }
     
  }// for genpart
  
}
