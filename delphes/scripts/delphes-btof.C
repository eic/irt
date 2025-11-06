
//
// Units are [mm], [GeV], [T], [ps];
//
// root -l delphes_btof.C
//

#define _E_PI_SEPARATION_MODE_

void delphes_btof( void )
{
  //printf("%f\n", (1.0 - erf(1.5/(sqrt(2.)*1.0)))/2);

  auto btof = new DelphesConfigTOF("BTOF");
  btof->UsePtMode();

  // Define particle mass hypotheses in ascending mass order; yes, there is no 
  // reason to overcomplicate things;
#ifdef _E_PI_SEPARATION_MODE_
  btof->AddMassHypothesis(-11);
#endif
  btof->AddMassHypothesis("pi+");
#ifndef _E_PI_SEPARATION_MODE_
  btof->AddMassHypothesis("K+");
  btof->AddMassHypothesis("proton");
#endif

  // Define t0 and detector time resolution is [ps];
  btof->SetT0Resolution        (20.00);
  btof->SetDetectorResolution  (30.00);
  // dp/p ~ 0.02% * p + 0.5%; take the proposal draft TEMPLATE data; assume holds for Pt;
  btof->SetMomentumResolution  (0.020, 0.500);
  // Units are [mm] throughout the code;
  btof->SetPathLengthResolution(1.000);

  // Installation radius in [mm]; constant magnetic field in [T];
  btof->SetInstallationRadius  (500.0);
  btof->SetMagneticField       (3.000);

  // eta and momentum range and binning; 
  btof->SetEtaRange     (-1.05, 1.05,  10);
  // Do not mind to use Pt rather than 1/Pt bins; [GeV/c];
#ifdef _E_PI_SEPARATION_MODE_
  btof->SetMomentumRange( 0.22, 0.45,  10);
#else
  btof->SetMomentumRange( 0.22, 2.20,  10);
#endif

  // This input is sufficient to allocate the internal tables and calculate 
  // time of flight for various mass hypotheses;
  btof->DoSigmaCalculations();
  
  // This is again some generic stuff;
  //btof->Print();
  btof->WriteTcl();
  {
    auto fout = new TFile("BTOF.root", "RECREATE");
    btof->Write();
    fout->Close();
  }
  exit(0);
} // delphes_btof()
