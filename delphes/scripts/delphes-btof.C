
//
// root -l delphes_btof.C
//

void delphes_btof( void )
{
  auto btof = new DelphesConfigTOF("BTOF");
  btof->UsePtMode();

  // Define particle mass hypotheses in ascending mass order; yes, there is no 
  // reason to overcomplicate things;
  //btof->AddMassHypothesis(-11);
  btof->AddMassHypothesis("pi+");
  btof->AddMassHypothesis("K+");
  btof->AddMassHypothesis("proton");

  // Define t0 and detector time resolution is [ns];
  btof->SetT0Resolution        (0.030);
  btof->SetDetectorResolution  (0.020);
  btof->SetMomentumResolution  (0.000, 0.000);
  // [mm] here;
  btof->SetPathLengthResolution(1.000);

  // Installation radius in [m]; constant magnetic field in [T];
  btof->SetInstallationRadius  (0.500);
  btof->SetMagneticField       (3.000);

  // eta and momentum range and binning; 
  btof->SetEtaRange(-1.0, 1.0, 10);
  // Do not mind to use Pt rather than 1/Pt bins; [GeV/c];
  btof->SetMomentumRange(1.0, 2.0, 10);

  // This input is sufficient to allocate the internal tables and calculate 
  // time of flight for various mass hypotheses;
  btof->DoSigmaCalculations();
  
  // This is again some generic stuff;
  btof->AddZeroSigmaEntries();
  btof->Print();
  //btof->Write();
  exit(0);
} // delphes_btof()
