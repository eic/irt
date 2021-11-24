
//
// Units are [mrad], [mm], [T];
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

  // d(theta) parameterization as taken from Wenqing's 11/23/21 slides, roughly 
  // averaged over eta;
  dirc->SetTrackerAngularResolution(0.9, 0.1);

  // eta and momentum range and binning; 
  dirc->SetEtaRange     (-1.64, 1.25,  10);
  // Do not mind to use Pt rather than 1/Pt bins; [GeV/c];
#ifdef _E_PI_SEPARATION_MODE_
  dirc->SetMomentumRange( 0.44, 1.50,  10);
#else
  dirc->SetMomentumRange( 0.44, 7.00,  10);
#endif

  // Installation radius in [mm]; constant magnetic field in [T];
  dirc->SetInstallationRadius  (1000.0);
  dirc->SetMagneticField       ( 3.000);

  dirc->SetParameterizationMap("./ctr_map_p1_0.95.root");

  // This is again some generic stuff;
  dirc->Write();
  exit(0);
} // delphes_dirc()
