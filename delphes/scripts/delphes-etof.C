
//
// Units are [mm], [GeV], [T], [ps];
//
// root -l delphes_etof.C
//

//#define _E_PI_SEPARATION_MODE_

void delphes_etof( void )
{
  //printf("%f\n", (1.0 - erf(1.5/(sqrt(2.)*1.0)))/2);

  auto etof = new DelphesConfigTOF("ETOF");
  //etof->UsePtMode();

  // Define particle mass hypotheses in ascending mass order; yes, there is no 
  // reason to overcomplicate things;
  //#ifdef _E_PI_SEPARATION_MODE_
  etof->AddMassHypothesis(-11);
  //#endif
  etof->AddMassHypothesis("pi+");
  //#ifndef _E_PI_SEPARATION_MODE_
  //etof->AddMassHypothesis("K+");
  //etof->AddMassHypothesis("proton");
  //#endif

  // Define t0 and detector time resolution is [ps];
  //etof->SetT0Resolution        (20.00);
  etof->SetT0Resolution        (25.00);
  //etof->SetDetectorResolution  (30.00);
  etof->SetDetectorResolution  (20.00);
  // dp/p ~ 0.02% * p + 0.5%; take the proposal draft TEMPLATE data; assume holds for Pt;
  //etof->SetMomentumResolution  (0.020, 0.500);
  // FIXME: assume ideal tracker for now;
  etof->SetMomentumResolution  (0.000, 0.000);
  // Units are [mm] throughout the code;
  etof->SetPathLengthResolution(1.000);

  // Installation radius in [mm]; constant magnetic field in [T];
  //etof->SetInstallationRadius  (500.0);
  //etof->SetMagneticField       (3.000);
  etof->SetInstallationDistance(1607.0);

  // eta and momentum range and binning; 
  etof->SetEtaRange     (-3.50, -1.60,  5);
  // Do not mind to use Pt rather than 1/Pt bins; [GeV/c];
  //#ifdef _E_PI_SEPARATION_MODE_
  etof->SetMomentumRange( 0.20,  2.00,  5);
  //#else
  //etof->SetMomentumRange( 0.22, 2.20,  10);
  //#endif

  // This input is sufficient to allocate the internal tables and calculate 
  // time of flight for various mass hypotheses;
  etof->DoSigmaCalculations();
  
  // This is again some generic stuff;
  //etof->Print();
  etof->WriteTcl();
  {
    auto fout = new TFile("ETOF.root", "RECREATE");
    etof->Write();
    fout->Close();
  }
  exit(0);
} // delphes_etof()
