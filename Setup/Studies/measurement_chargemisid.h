void AnaScript::CalculateChargeMisID(){

  //First, pickup DY-rich events.
  //Baseline selection: Exacty two electrons with Mee in [76, 106] and pT1/pT0 > 0.70
  //The leading electron satisfies the offline pT threshold of 35 GeV
  
  if((int)Electron.size()==2){
    if(Electron.at(0).v.Pt() > 35){

      TLorentzVector recoZ = Electron.at(0).v + Electron.at(1).v;
      float dilep_mass     = recoZ.M();
      float dilep_pt       = recoZ.Pt();
      float dilep_ptratio  = Electron.at(1).v.Pt()/Electron.at(0).v.Pt();
      double wt = 1.0;

      //--------------------------------------------------
      if(_data == 0){
	//Electron scale-factors:
	double sf0 = 1.0; double sf1 = 1.0;
	sf0 = correctinlib_leptonSF(Electron.at(0), "nom");
	sf1 = correctinlib_leptonSF(Electron.at(1), "nom");
	double lepIdIsoSF = sf0*sf1;
	//Trigger efficiencies:
	double ef0 = 1.0; double ef1 = 1.0;
	ef0 = GetLeptonTriggerEfficiency(Electron.at(0));
	ef1 = GetLeptonTriggerEfficiency(Electron.at(1));
	double triggerEff = 1-((1-ef0)*(1-ef1));
	//Pileup weight:
	double pileupwt = pileupwt = (double)correctionlib_pileupWt(*Pileup_nTrueInt, "nom");
	wt = lepIdIsoSF*triggerEff*pileupwt;
      }
      //--------------------------------------------------

      //Apply Charge-misID rate in Drell-Yan:
      if(_flag == "dy"){
	double charge_misID_wt = 1.0;

	if(0 < dilep_pt && dilep_pt < 10)         charge_misID_wt = 1.2790;
	else if(10 < dilep_pt && dilep_pt < 20)   charge_misID_wt = 1.3047;
	else if(20 < dilep_pt && dilep_pt < 30)   charge_misID_wt = 1.2914;
	else if(30 < dilep_pt && dilep_pt < 50)   charge_misID_wt = 1.2739;
	else if(50 < dilep_pt && dilep_pt < 75)   charge_misID_wt = 1.2930;
	else if(75 < dilep_pt && dilep_pt < 100)  charge_misID_wt = 1.2709;
	else if(100 < dilep_pt && dilep_pt < 200) charge_misID_wt = 1.3816;
	else if(200 < dilep_pt && dilep_pt < 350) charge_misID_wt = 1.3841;
	else if(350 < dilep_pt && dilep_pt < 500) charge_misID_wt = 1.2945;
	else                                      charge_misID_wt = 1.3622;
	wt = wt*charge_misID_wt;
      }
      
      bool baseline_events = (76 < dilep_mass && dilep_mass < 106) && dilep_ptratio > 0.7;
      if(baseline_events){

	//Calculating electron charge misID rate in bins of Z pT.
	//Numerator = 2LSS events as a function of Z pT
	//Denominator = 2L events as a function of Z pT
	h.chargeflip[0]->Fill(dilep_pt, wt);
	if(Electron.at(0).charge == Electron.at(1).charge){
	  h.chargeflip[1]->Fill(dilep_pt, wt);
	}

      }//baseline
    }//offline-trigger
  }//2L
}//function end
