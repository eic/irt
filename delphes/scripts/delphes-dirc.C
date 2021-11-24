
//
// Units are [mrad];
//
// root -l delphes_dirc.C
//

#define _E_PI_SEPARATION_MODE_

void delphes_dirc( void )
{
  auto dirc = new DelphesConfigDIRC("barrelDIRC");
  dirc->UsePtMode();

  // Define particle mass hypotheses in ascending mass order; yes, there is no 
  // reason to overcomplicate things;
#ifdef _E_PI_SEPARATION_MODE_
  dirc->AddMassHypothesis(-11);
#endif
  dirc->AddMassHypothesis("pi+");
#ifndef _E_PI_SEPARATION_MODE_
  dirc->AddMassHypothesis("K+");
  dirc->AddMassHypothesis("proton");
#endif

  // Define t0 and detector time resolution is [ps];
  //dirc->SetT0Resolution        (20.00);
  //dirc->SetDetectorResolution  (30.00);
  // dp/p ~ 0.02% * p + 0.5%; take the proposal draft TEMPLATE data; assume holds for Pt;

  // Assume canonic 0.5mrad for the moment;
  dirc->SetTrackerAngularResolution(0.500);

  // Units are [mm] throughout the code;
  //dirc->SetPathLengthResolution(1.000);

  // Installation radius in [mm]; constant magnetic field in [T];
  //dirc->SetInstallationRadius  (500.0);
  //dirc->SetMagneticField       (3.000);

  // eta and momentum range and binning; 
  dirc->SetEtaRange     (-1.05, 1.05,  10);
  // Do not mind to use Pt rather than 1/Pt bins; [GeV/c];
#ifdef _E_PI_SEPARATION_MODE_
  dirc->SetMomentumRange( 0.22, 0.45,  10);
#else
  dirc->SetMomentumRange( 0.22, 2.20,  10);
#endif

  // This input is sufficient to allocate the internal tables and calculate 
  // time of flight for various mass hypotheses;
  //dirc->DoSigmaCalculations();
  
  // This is again some generic stuff;
  //dirc->Print();
  //dirc->Write();
  exit(0);
} // delphes_dirc()
