
//
// root -l dconfig-jaydeep.C
//

void dconfig_jaydeep( void )
{
  auto delphes = new DelphesConfigRICH("pfRICH");

  // Define particle mass hypotheses in ascending mass order; yes, there is no 
  // reason to overcomplicate things;
  delphes->AddMassHypothesis("e-");
  delphes->AddMassHypothesis("pi+");

  // Define radiator refractive index;
  delphes->SetRefractiveIndex(1.04549);
  // Tracker angular resolution as provided by Matt; FIXME: momentum resolution ignored;
  //delphes->ImportTrackingSmearing("../database/dth_parameters_22.11.2.txt", 
  //				  "../database/dph_parameters_22.11.2.txt");
  delphes->ImportTrackingSmearing("../database/par_th.modified.txt", 
  				  "../database/par_ph.modified.txt");

  // Define eta ranges in ascending order;
  {
    auto eta = delphes->AddEtaRange(-3.5, -2.8);

    // Arguments [0..1]: momentum ranges for this eta in [GeV] in ascending order without gaps, 
    // Arguments [2..N]: sigma values in [mrad] matching the mass hypotheses above, in the same 
    // ascending order used to call delphes->AddMassHypothesis(); 
    eta->AddMomentumRange( 0.05, 0.10, 0.00,  0.00);
    eta->AddMomentumRange( 0.10, 0.15, 0.00,  0.00);
    eta->AddMomentumRange( 0.15, 0.20, 2.48,  0.00);
    eta->AddMomentumRange( 0.20, 0.25, 2.00,  0.00);
    eta->AddMomentumRange( 0.25, 0.30, 1.89,  0.00);
    eta->AddMomentumRange( 0.30, 0.35, 1.78,  0.00);
    eta->AddMomentumRange( 0.35, 0.40, 1.73,  0.00);
    eta->AddMomentumRange( 0.40, 0.45, 1.69,  0.00);
    eta->AddMomentumRange( 0.45, 0.50, 1.68, 10.56);
    eta->AddMomentumRange( 0.50, 0.55, 1.64,  4.59);
    eta->AddMomentumRange( 0.55, 0.60, 1.63,  3.18);
    eta->AddMomentumRange( 0.60, 0.70, 1.60,  2.41);
    eta->AddMomentumRange( 0.70, 0.80, 1.60,  2.05);
    eta->AddMomentumRange( 0.80, 0.90, 1.65,  1.86);
    eta->AddMomentumRange( 0.90, 1.00, 1.58,  1.80);
    eta->AddMomentumRange( 1.00, 1.50, 1.59,  1.68);
    eta->AddMomentumRange( 1.50, 2.00, 1.57,  1.63);
    eta->AddMomentumRange( 2.00, 2.50, 1.59,  1.59);
    eta->AddMomentumRange( 2.50, 3.00, 1.58,  1.62);
    eta->AddMomentumRange( 3.00, 3.50, 1.57,  1.63);
    eta->AddMomentumRange( 3.50, 4.00, 1.59,  1.60);
    eta->AddMomentumRange( 4.00, 4.50, 1.53,  1.58);
    eta->AddMomentumRange( 4.50, 5.00, 1.61,  1.58);
    eta->AddMomentumRange( 5.00, 5.50, 1.59,  1.57);
  }
  {
    auto eta = delphes->AddEtaRange(-2.8, -1.9);

    // Well, average bin momentum is used to calculate Cherenkov theta, so the bins
    // should not be too wide in the areas where dtheta/dp is high for at least one 
    // of the hypotheses;
    eta->AddMomentumRange( 0.05, 0.10, 3.67,  0.00);
    eta->AddMomentumRange( 0.10, 0.15, 3.62,  0.00);
    eta->AddMomentumRange( 0.15, 0.20, 2.24,  0.00);
    eta->AddMomentumRange( 0.20, 0.25, 1.82,  0.00);
    eta->AddMomentumRange( 0.25, 0.30, 1.76,  0.00);
    eta->AddMomentumRange( 0.30, 0.35, 1.69,  0.00);
    eta->AddMomentumRange( 0.35, 0.40, 1.66,  0.00);
    eta->AddMomentumRange( 0.40, 0.45, 1.69,  0.00);
    eta->AddMomentumRange( 0.45, 0.50, 1.66, 10.76);
    eta->AddMomentumRange( 0.50, 0.55, 1.64,  4.62);
    eta->AddMomentumRange( 0.55, 0.60, 1.61,  3.07);
    eta->AddMomentumRange( 0.60, 0.70, 1.60,  2.40);
    eta->AddMomentumRange( 0.70, 0.80, 1.57,  2.05);
    eta->AddMomentumRange( 0.80, 0.90, 1.60,  1.93);
    eta->AddMomentumRange( 0.90, 1.00, 1.59,  1.78);
    eta->AddMomentumRange( 1.00, 1.50, 1.59,  1.68);
    eta->AddMomentumRange( 1.50, 2.00, 1.55,  1.60);
    eta->AddMomentumRange( 2.00, 2.50, 1.57,  1.60);
    eta->AddMomentumRange( 2.50, 3.00, 1.56,  1.58);
    eta->AddMomentumRange( 3.00, 3.50, 1.53,  1.59);
    eta->AddMomentumRange( 3.50, 4.00, 1.55,  1.55);
    eta->AddMomentumRange( 4.00, 4.50, 1.55,  1.54);
    eta->AddMomentumRange( 4.50, 5.00, 1.57,  1.56);
    eta->AddMomentumRange( 5.00, 5.50, 1.57,  1.55);
    
  }

  {
    auto eta = delphes->AddEtaRange(-1.9, -1.5);

    eta->AddMomentumRange( 0.05, 0.10, 3.78,  0.00);
    eta->AddMomentumRange( 0.10, 0.15, 3.52,  0.00);
    eta->AddMomentumRange( 0.15, 0.20, 2.31,  0.00);
    eta->AddMomentumRange( 0.20, 0.25, 2.04,  0.00);
    eta->AddMomentumRange( 0.25, 0.30, 1.87,  0.00);
    eta->AddMomentumRange( 0.30, 0.35, 1.79,  0.00);
    eta->AddMomentumRange( 0.35, 0.40, 1.76,  0.00);
    eta->AddMomentumRange( 0.40, 0.45, 1.71,  0.00);
    eta->AddMomentumRange( 0.45, 0.50, 1.71, 10.67);
    eta->AddMomentumRange( 0.50, 0.55, 1.71,  4.99);
    eta->AddMomentumRange( 0.55, 0.60, 1.62,  3.38);
    eta->AddMomentumRange( 0.60, 0.70, 1.59,  2.52);
    eta->AddMomentumRange( 0.70, 0.80, 1.58,  2.11);
    eta->AddMomentumRange( 0.80, 0.90, 1.56,  1.88);
    eta->AddMomentumRange( 0.90, 1.00, 1.55,  1.78);
    eta->AddMomentumRange( 1.00, 1.50, 1.53,  1.65);
    eta->AddMomentumRange( 1.50, 2.00, 1.56,  1.60);
    eta->AddMomentumRange( 2.00, 2.50, 1.56,  1.53);
    eta->AddMomentumRange( 2.50, 3.00, 1.57,  1.54);
    eta->AddMomentumRange( 3.00, 3.50, 1.54,  1.56);
    eta->AddMomentumRange( 3.50, 4.00, 1.53,  1.52);
    eta->AddMomentumRange( 4.00, 4.50, 1.52,  1.54);
    eta->AddMomentumRange( 4.50, 5.00, 1.51,  1.53);
    eta->AddMomentumRange( 5.00, 5.50, 1.49,  1.51); } 

  // This call also makes a guess on the kaon and proton thresholds;
  delphes->AddZeroSigmaEntries();
  //delphes->Print();
  delphes->WriteTcl();
  {
    auto fout = new TFile("pfRICH.root", "RECREATE");
    delphes->Write();
    fout->Close();
  }
  exit(0);
} // dconfig()
