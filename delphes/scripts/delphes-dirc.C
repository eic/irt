
//
// Units are [mrad], [mm], [T];
//
// root -l delphes-dirc.C
//

//#define _E_PI_SEPARATION_MODE_

void delphes_dirc( void )
{
  //printf("%f\n", (1.0 - erf(1.75/(sqrt(2.)*1.0)))/2);

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

  // "d(theta) ~ a/p + b" parameterization as taken from Wenqing's 11/23/21 slides, 
  // roughly averaged over eta;
  dirc->SetTrackerAngularResolution(0.9, 0.1);

  // eta and momentum range and binning; 
  dirc->SetEtaRange     (-1.64, 1.25,  10);
  //dirc->SetEtaRange     ( 1.0,  1.01,   1);
  // Do not mind to use Pt rather than 1/Pt bins; [GeV/c];
#ifdef _E_PI_SEPARATION_MODE_
  dirc->SetMomentumRange( 0.44, 3.00,  10);
  //dirc->SetMomentumRange( 1.20, 1.21,   1);
#else
  dirc->SetMomentumRange( 0.44,10.00,  10);
#endif

  // Installation radius in [mm]; constant magnetic field in [T];
  dirc->SetInstallationRadius  (1060.0);
  dirc->SetMagneticField       ( 3.000);

  dirc->SetParameterizationMap("./ctr_map_p1_0.95.root");

  dirc->DoSigmaCalculations();
  // This is again some generic stuff;
  dirc->Write(false);
  exit(0);
} // delphes_dirc()
