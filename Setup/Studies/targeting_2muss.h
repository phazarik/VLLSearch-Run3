void AnaScript::Make2muSSPlots(){

  //Picking events with exactly 2muons with same-sign, where the leading muon pT>26 GeV.
  bool basic_evt_selection = false;
  if((int)Muon.size()==2 && (int)Electron.size()==0){
    if(Muon.at(0).v.Pt()>26 && Muon.at(0).charge == Muon.at(1).charge)
      basic_evt_selection = true;
  }
  if((int)Muon.size()==1 && (int)Electron.size()==1){
    if(Muon.at(0).v.Pt()>26 || Electron.at(0).v.Pt()>32){
      if(Muon.at(0).charge == Electron.at(0).charge)
	nemuss++;
    }
  }
  if((int)Muon.size()==0 && (int)Electron.size()==2){
    if(Electron.at(0).v.Pt()>32 && Electron.at(0).charge == Electron.at(1).charge)
      n2ess++;
  }
  
  if(basic_evt_selection){
    n2muss++;

    //Calculating event weight:
    double sf0 = Muon_2018UL_Reco(Muon.at(0).v.Pt(), Muon.at(0).v.Eta());
    double sf1 = Muon_2018UL_Reco(Muon.at(1).v.Pt(), Muon.at(1).v.Eta());
    double scalefactor = sf0*sf1;
    double ef0 = TrigEff_2018_IsoMu24_Data(Muon.at(0).v.Pt(), Muon.at(0).v.Eta());
    double ef1 = TrigEff_2018_IsoMu24_Data(Muon.at(1).v.Pt(), Muon.at(1).v.Eta());
    double triggereff = 1-((1-ef0)*(1-ef1));
    double wt = scalefactor*triggereff;

    //cout<<"Testing 2mu events"<<endl;

    //Event level variabls:
    //Counts:
    UInt_t nlep  = (UInt_t)Muon.size();
    UInt_t njet  = (UInt_t)Jet.size();
    UInt_t nbjet = (UInt_t)bJet.size();

    //Object-level variables: First lepton
    Float_t lep0_pt  = (Float_t)Muon.at(0).v.Pt();
    Float_t lep0_eta = (Float_t)Muon.at(0).v.Eta();
    Float_t lep0_phi = (Float_t)Muon.at(0).v.Phi();
    Float_t lep0_iso = (Float_t)Muon.at(0).reliso03;
    Float_t lep0_mt  = (Float_t)transv_mass(Muon.at(0).v.E(), Muon.at(0).v.Phi(), metpt, metphi);
    //Second lepton
    Float_t lep1_pt  = (Float_t)Muon.at(1).v.Pt();
    Float_t lep1_eta = (Float_t)Muon.at(1).v.Eta();
    Float_t lep1_phi = (Float_t)Muon.at(1).v.Phi();
    Float_t lep1_iso = (Float_t)Muon.at(1).reliso03;
    Float_t lep1_mt  = (Float_t)transv_mass(Muon.at(1).v.E(), Muon.at(1).v.Phi(), metpt, metphi);

    //Dilepton system:
    TLorentzVector dilep = Muon.at(0).v + Muon.at(1).v;
    Float_t dilep_pt   = (Float_t)dilep.Pt();
    Float_t dilep_eta  = (Float_t)dilep.Eta();
    Float_t dilep_phi  = (Float_t)dilep.Phi();
    Float_t dilep_mass = (Float_t)dilep.M();
    Float_t dilep_mt   = (Float_t)transv_mass(dilep.E(), dilep.Phi(), metpt, metphi);
    Float_t dilep_deta    = (Float_t)fabs(Muon.at(0).v.Eta() - Muon.at(1).v.Eta());
    Float_t dilep_dphi    = (Float_t)delta_phi(Muon.at(0).v.Phi(), Muon.at(1).v.Phi());
    Float_t dilep_dR      = (Float_t)Muon.at(0).v.DeltaR(Muon.at(1).v);
    Float_t dilep_ptratio = (Float_t)lep1_pt/lep0_pt;

    //Event level:
    Float_t HT=0; for(Int_t i=0; i<(Int_t)Jet.size(); i++) HT = HT + Jet.at(i).v.Pt();
    Float_t ST = HT + dilep_pt;
    Float_t STfrac = 0; if(ST>0) STfrac = dilep_pt/ST;
    Float_t dphi_metlep0 = (Float_t)delta_phi(Muon.at(0).v.Phi(), metphi);
    Float_t dphi_metlep1 = (Float_t)delta_phi(Muon.at(1).v.Phi(), metphi);
    Float_t dphi_metdilep = (Float_t)delta_phi(dilep.Phi(), metphi);
    Float_t dphi_metlep_max = (Float_t)max(dphi_metlep0, dphi_metlep1);
    Float_t dphi_metlep_min = (Float_t)min(dphi_metlep0, dphi_metlep1);

    //bool basic_filtering = dilep_pt>30 && fabs(dilep_eta)<4 && HT<500;  
    bool event_selection = HT<500;

    //------------------------
    // QCD scaling in HT bins:
    //------------------------
    double qcdscale = 1.0; //default

    if(HT < 50)                 qcdscale = 0.0212517;
    else if (50<=HT  && HT<100) qcdscale = 0.0485609;
    else if (100<=HT && HT<150) qcdscale = 0.0745748;
    else if (150<=HT && HT<200) qcdscale = 0.0303177;
    else if (200<=HT && HT<250) qcdscale = 0.0225880;
    else if (250<=HT && HT<300) qcdscale = 0.0179411;
    else if (300<=HT && HT<350) qcdscale = 0.0118273;
    else if (350<=HT && HT<400) qcdscale = 0.0151473;
    else if (400<=HT && HT<450) qcdscale = 0.0103587;
    else                        qcdscale = 0.0105359;
  
    if(_flag == "qcd") wt = wt*qcdscale;

    if(event_selection){
      nEvtPass++;
    
      h.evt2LSS[0] ->Fill(nlep, wt);
      h.evt2LSS[1] ->Fill(njet, wt);
      h.evt2LSS[2] ->Fill(nbjet, wt);

      h.evt2LSS[3] ->Fill(lep0_pt, wt);
      h.evt2LSS[4] ->Fill(lep0_eta, wt);
      h.evt2LSS[5] ->Fill(lep0_phi, wt);
      h.evt2LSS[6] ->Fill(lep0_iso, wt);
      h.evt2LSS[7] ->Fill(lep0_mt, wt);

      h.evt2LSS[8] ->Fill(lep1_pt, wt);
      h.evt2LSS[9] ->Fill(lep1_eta, wt);
      h.evt2LSS[10]->Fill(lep1_phi, wt);
      h.evt2LSS[11]->Fill(lep1_iso, wt);
      h.evt2LSS[12]->Fill(lep1_mt, wt);

      h.evt2LSS[13]->Fill(dilep_pt, wt);
      h.evt2LSS[14]->Fill(dilep_eta, wt);
      h.evt2LSS[15]->Fill(dilep_phi, wt);
      h.evt2LSS[16]->Fill(dilep_mass, wt);
      h.evt2LSS[17]->Fill(dilep_mt, wt);
      h.evt2LSS[18]->Fill(dilep_deta, wt);
      h.evt2LSS[19]->Fill(dilep_dphi, wt);
      h.evt2LSS[20]->Fill(dilep_dR, wt);
      h.evt2LSS[21]->Fill(dilep_ptratio, wt);

      h.evt2LSS[22]->Fill(HT, wt);
      h.evt2LSS[23]->Fill(ST, wt);
      h.evt2LSS[24]->Fill(STfrac, wt);

      h.evt2LSS[25]->Fill(dphi_metlep0, wt);
      h.evt2LSS[26]->Fill(dphi_metlep1, wt);
      h.evt2LSS[27]->Fill(dphi_metdilep, wt);
      h.evt2LSS[28]->Fill(dphi_metlep_max, wt);
      h.evt2LSS[29]->Fill(dphi_metlep_min, wt);

      h.evt2LSS[30]->Fill(metpt, wt);
      h.evt2LSS[31]->Fill(metphi, wt);
    
    }

  }
  
}