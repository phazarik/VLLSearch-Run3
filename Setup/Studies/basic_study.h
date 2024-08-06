void AnaScript::MakeBasicEvtPlots()
{

  //Momentum:
  if((int)Electron.size()> 0)   h.basic[0] ->Fill(Electron.at(0).v.Pt());
  if((int)Electron.size()> 1)   h.basic[1] ->Fill(Electron.at(1).v.Pt());
  if((int)Electron.size()> 2)   h.basic[2] ->Fill(Electron.at(2).v.Pt());

  if((int)Muon.size()> 0)       h.basic[3] ->Fill(Muon.at(0).v.Pt());
  if((int)Muon.size()> 1)       h.basic[4] ->Fill(Muon.at(1).v.Pt());
  if((int)Muon.size()> 2)       h.basic[5] ->Fill(Muon.at(2).v.Pt());

  if((int)Jet.size()> 0)        h.basic[6] ->Fill(Jet.at(0).v.Pt());
  if((int)Jet.size()> 1)        h.basic[7] ->Fill(Jet.at(1).v.Pt());
  if((int)Jet.size()> 2)        h.basic[8] ->Fill(Jet.at(2).v.Pt());

  if((int)MediumbJet.size()> 0) h.basic[9] ->Fill(MediumbJet.at(0).v.Pt());
  if((int)MediumbJet.size()> 1) h.basic[10]->Fill(MediumbJet.at(1).v.Pt());
  if((int)MediumbJet.size()> 2) h.basic[11]->Fill(MediumbJet.at(2).v.Pt());

  //Transverse masses:
  if((int)Electron.size()> 0)   h.basic[12]->Fill(transv_mass(Electron.at(0).v.E(), Electron.at(0).v.Phi(), metpt ,metphi));
  if((int)Electron.size()> 1)   h.basic[13]->Fill(transv_mass(Electron.at(1).v.E(), Electron.at(1).v.Phi(), metpt ,metphi));
  if((int)Electron.size()> 2)   h.basic[14]->Fill(transv_mass(Electron.at(2).v.E(), Electron.at(2).v.Phi(), metpt ,metphi));

  if((int)Muon.size()> 0)   h.basic[15]->Fill(transv_mass(Muon.at(0).v.E(), Muon.at(0).v.Phi(), metpt ,metphi));
  if((int)Muon.size()> 1)   h.basic[16]->Fill(transv_mass(Muon.at(1).v.E(), Muon.at(1).v.Phi(), metpt ,metphi));
  if((int)Muon.size()> 2)   h.basic[17]->Fill(transv_mass(Muon.at(2).v.E(), Muon.at(2).v.Phi(), metpt ,metphi));

  if((int)Jet.size()> 0)   h.basic[18]->Fill(transv_mass(Jet.at(0).v.E(), Jet.at(0).v.Phi(), metpt ,metphi));
  if((int)Jet.size()> 1)   h.basic[19]->Fill(transv_mass(Jet.at(1).v.E(), Jet.at(1).v.Phi(), metpt ,metphi));
  if((int)Jet.size()> 2)   h.basic[20]->Fill(transv_mass(Jet.at(2).v.E(), Jet.at(2).v.Phi(), metpt ,metphi));

  //di-objects:
  if((int)Electron.size()> 1){
    TLorentzVector vec = Electron.at(0).v + Electron.at(1).v;
    h.basic[21]->Fill(vec.Pt());
    h.basic[22]->Fill(vec.Eta());
    h.basic[23]->Fill(vec.Phi());
    h.basic[24]->Fill(vec.M());
    h.basic[25]->Fill(transv_mass(vec.E(), vec.Phi(), metpt, metphi));
  }
  if((int)Muon.size()> 1){
    TLorentzVector vec = Muon.at(0).v + Muon.at(1).v;
    h.basic[26]->Fill(vec.Pt());
    h.basic[27]->Fill(vec.Eta());
    h.basic[28]->Fill(vec.Phi());
    h.basic[29]->Fill(vec.M());
    h.basic[30]->Fill(transv_mass(vec.E(), vec.Phi(), metpt, metphi));
  }
  if((int)Jet.size()> 1){
    TLorentzVector vec = Jet.at(0).v + Jet.at(1).v;
    h.basic[31]->Fill(vec.Pt());
    h.basic[32]->Fill(vec.Eta());
    h.basic[33]->Fill(vec.Phi());
    h.basic[34]->Fill(vec.M());
    h.basic[35]->Fill(transv_mass(vec.E(), vec.Phi(), metpt, metphi));
  }
 

  //dPhi:
  if((int)Electron.size()>1)    h.basic[36]->Fill(delta_phi(Electron.at(0).v.Phi(), Electron.at(1).v.Phi()));
  if((int)Muon.size()>1)        h.basic[37]->Fill(delta_phi(Muon.at(0).v.Phi(),     Muon.at(1).v.Phi()));
  if((int)Jet.size()>1)         h.basic[38]->Fill(delta_phi(Jet.at(0).v.Phi(),      Jet.at(1).v.Phi()));

  //dR:
  if((int)Electron.size()>1)    h.basic[39]->Fill(Electron.at(0).v.DeltaR(Electron.at(1).v));
  if((int)Muon.size()>1)        h.basic[40]->Fill(Muon.at(0).v.DeltaR(Muon.at(1).v));
  if((int)Jet.size()>1)         h.basic[41]->Fill(Jet.at(0).v.DeltaR(Jet.at(1).v));

  //Others:
  h.basic[41]->Fill(*PuppiMET_pt);
  h.basic[42]->Fill(*PuppiMET_phi);
  
}
