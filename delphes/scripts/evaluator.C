
// 
// ! Make sure libDELPHES.so is in LD_LIBRARY_PATH
//
// cd delphes/scripts
//
// root -l 'evaluator.C("<delphes-file.root>")'
//
// root -l 'evaluator.C("e_pi_pid.root")'
// 

void evaluator(const char *fname)
{
  auto ff = new TFile(fname);
  auto dconfig = dynamic_cast<DelphesConfigRICH*>(ff->Get("DelphesConfigRICH"));
  ff->Close();

  {
    // Use the same momentum, not to overcomplicate logic;
    TVector3 p(0.0, 0.4, -3.0);
    printf("%f\n", p.Eta());

    // Electron hypothesis;
    auto electron = dconfig->GetMassHypothesis(11), pion = dconfig->GetMassHypothesis(211);

    // In gaussian sigma units "to the left" (towards the pion peak location); 
    double cutoff = -1.0;
    unsigned inefficiency = 0, misidentification = 0, contamination = 0;

    // Generate few random electron & pion events, in a 50:50 proportion;
    for(unsigned iq=0; iq<50; iq++) {
      auto truth = iq%2 ? electron : pion;

      // Using built-in configuration (average Cherenkov angle <theta> for this
      // particle hypothesis defined by momentum and encoded medium refractive index, 
      // as well as expected Cherenkov angle smearing for this combination of {pdg,p,eta})
      // simulate a "measured" Cherenkov angle for this event using rndm.Gaus() call;
      double theta = dconfig->GenerateMeasurement(truth->PdgCode(), p);

      // Check which particle hypothesis wins (gives smaller chi^2 for this 'theta');
      auto best = dconfig->FindBestHypothesis(p, theta);

      char signature = ' ';
      // Evaluate the outcome;
      if (truth == electron) {
	// Electron identified as pion; so be it;
	if (best != electron) {
	  misidentification++;
	  signature = 'm';
	} //if

	// Electron measured "too close to pion" and rejected; sigh!;
	if (best == electron && electron->GetMeasurementOffset() < cutoff) {
	  inefficiency++;
	  signature = 'i';
	} //if
      } else {
	// Pion identified as electron, and accepted as such; bad!;
	if (best == electron && electron->GetMeasurementOffset() > cutoff) {
	  contamination++;
	  signature = 'c';
	} //if
      } //if

      printf("%3d %3d -> %7.2f [mrad] (%3d %c, %7.2f [sigma])\n", iq, truth->PdgCode(), theta,
	     best->PdgCode(), signature, electron->GetMeasurementOffset());
    } //for iq
  }
} // evaluator()
