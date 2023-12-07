float AnaScript::getEventWeight(){

  float wt = 1.0; //default

  //This correction is applied to MC only.
  //The event has to pass the trigger that we put by hand.

  if(evt_trigger && _data==0){

    float scalefactor = 1.0;
    float triggeff = 1.0;

    //4L inclusive final states (considering the first 4 into the calculation):
    if(evt_4L_incl){
      float lep0SF = LeptonIDSF(LightLepton.at(0).id, LightLepton.at(0).v.Pt(), LightLepton.at(0).v.Eta());
      float lep1SF = LeptonIDSF(LightLepton.at(1).id, LightLepton.at(1).v.Pt(), LightLepton.at(1).v.Eta());
      float lep2SF = LeptonIDSF(LightLepton.at(2).id, LightLepton.at(2).v.Pt(), LightLepton.at(2).v.Eta());
      float lep3SF = LeptonIDSF(LightLepton.at(3).id, LightLepton.at(3).v.Pt(), LightLepton.at(3).v.Eta());
      scalefactor = lep0SF * lep1SF * lep2SF * lep3SF;
      float e1=SingleLepTrigger_eff(LightLepton.at(0).id, LightLepton.at(0).v.Pt(), LightLepton.at(0).v.Eta());
      float e2=SingleLepTrigger_eff(LightLepton.at(1).id, LightLepton.at(1).v.Pt(), LightLepton.at(1).v.Eta());
      float e3=SingleLepTrigger_eff(LightLepton.at(2).id, LightLepton.at(2).v.Pt(), LightLepton.at(2).v.Eta());
      float e4=SingleLepTrigger_eff(LightLepton.at(3).id, LightLepton.at(3).v.Pt(), LightLepton.at(3).v.Eta());
      triggeff=1-((1-e1)*(1-e2)*(1-e3)*(1-e4));
    }
    //Exactly 3L final states:
    else if(evt_3L){      
      float lep0SF = LeptonIDSF(LightLepton.at(0).id, LightLepton.at(0).v.Pt(), LightLepton.at(0).v.Eta());
      float lep1SF = LeptonIDSF(LightLepton.at(1).id, LightLepton.at(1).v.Pt(), LightLepton.at(1).v.Eta());
      float lep2SF = LeptonIDSF(LightLepton.at(2).id, LightLepton.at(2).v.Pt(), LightLepton.at(2).v.Eta());
      scalefactor = lep0SF * lep1SF * lep2SF;
      float e1=SingleLepTrigger_eff(LightLepton.at(0).id, LightLepton.at(0).v.Pt(), LightLepton.at(0).v.Eta());
      float e2=SingleLepTrigger_eff(LightLepton.at(1).id, LightLepton.at(1).v.Pt(), LightLepton.at(1).v.Eta());
      float e3=SingleLepTrigger_eff(LightLepton.at(2).id, LightLepton.at(2).v.Pt(), LightLepton.at(2).v.Eta());
      triggeff=1-((1-e1)*(1-e2)*(1-e3));
    }
    //Exactly 2L final states:
    else if(evt_2LSS || evt_2LOS){      
      float lep0SF = LeptonIDSF(LightLepton.at(0).id, LightLepton.at(0).v.Pt(), LightLepton.at(0).v.Eta());
      float lep1SF = LeptonIDSF(LightLepton.at(1).id, LightLepton.at(1).v.Pt(), LightLepton.at(1).v.Eta());
      scalefactor = lep0SF * lep1SF;
      float e1=SingleLepTrigger_eff(LightLepton.at(0).id, LightLepton.at(0).v.Pt(), LightLepton.at(0).v.Eta());
      float e2=SingleLepTrigger_eff(LightLepton.at(1).id, LightLepton.at(1).v.Pt(), LightLepton.at(1).v.Eta());
      triggeff=1-((1-e1)*(1-e2));
    }
    //Exactly 1L final states:
    else if(evt_1L2J_incl || evt_1L1J || evt_1L0J){
      scalefactor = LeptonIDSF(LightLepton.at(0).id, LightLepton.at(0).v.Pt(), LightLepton.at(0).v.Eta());
      triggeff = SingleLepTrigger_eff(LightLepton.at(0).id, LightLepton.at(0).v.Pt(), LightLepton.at(0).v.Eta());
    }

    wt = scalefactor * triggeff;

    h.evtweight[0]->Fill(scalefactor);
    h.evtweight[1]->Fill(triggeff);
    h.evtweight[2]->Fill(evt_wt);
    
  }

  
  return wt;

}
