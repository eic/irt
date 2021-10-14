
//#include <TRandom.h>

#include <ChargedParticle.h>
//#include <CherenkovDetectorCollection.h>

// -------------------------------------------------------------------------------------

void ChargedParticle::PIDReconstruction(CherenkovPID &pid)
{
  // FIXME: 'zdim' should be configurable;
  const unsigned zdim = 10;

  // Loop through all of the photons recorded in all radiators; apply IRT on a fixed grid 
  // of emission vertex locations and build kind of a PDF out of that to calculate weights;
  // this approach may not be dramatically efficient, but it would 1) work in case of the 
  // magnetic field bending, 2) allow easy extention to a dRICH case where each mass 
  // hypothesis would evaluate each of the detected photons on the aerogel and gas grids
  // independently and build a sum of their PDFs without much thinking about outlier photon
  // rejection, average theta calculation and such;
  for(auto rhistory: GetRadiatorHistory()) {
    //+auto radiator = GetRadiator(rhistory);
    auto history  = GetHistory (rhistory);
	
    //+auto det = geometry->GetDetectorByRadiator(radiator);
    
    if (history->StepCount() >= 2) {
      // FIXME: for now consider just the first and the last points;
      auto from = history->GetStep(0), to = history->GetStep(history->StepCount()-1);
      TVector3 p = 0.5*(from->GetMomentum() + to->GetMomentum());
      {
	for(auto photon: history->Photons()) {
	  if (!photon->WasDetected()) continue;

	  auto pd = photon->GetPhotonDetector();

	  TVector3 phx = photon->GetDetectionPosition();
	  {
	    // FIXME: keep the commented out smearing code here for a while; photon detector 
	    // surface parameterization is available, so one can smear the 3D location in a 
	    // proper way if needed;
	    //+double pitch = 3.4;//, xysigma = pitch/sqrt(12.);
	    //phx += (pitch/2)*TVector3(gRandom->Uniform(-1,1), gRandom->Uniform(-1,1), 0);

	    //double x = pitch*rint(phx.x()/pitch), y = pitch*rint(phx.y()/pitch);
	    //phx = TVector3(x, y, phx.z());

	    //phx += (pitch/2)*TVector3(gRandom->Uniform(-1,1), gRandom->Uniform(-1,1), 0);
	    //phx += xysigma*TVector3(gRandom->Gaus(0,1), gRandom->Gaus(0,1), 0);
	  }
	    
	  TVector3 ptnx = (to->GetPosition() - from->GetPosition()).Unit();

	  auto vstart = from->GetPosition() + 0.001*ptnx, vend = to->GetPosition() - 0.001*ptnx;
	  double vlen = (vend - vstart).Mag(), step = vlen/zdim;
	  {
	    IRTSolution solutions[zdim+1];

	    for(unsigned iq=0; iq<zdim+1; iq++) {
	      solutions[iq] = pd->GetIRT()->Solve(vstart + iq*step*ptnx, ptnx, phx, TVector3(0,0,1), false);
	      //printf("%2d -> %7.2f\n", iq, 1000*solutions[iq].GetTheta());
	    } //for iq

	    for(unsigned iq=0; iq<zdim; iq++) {
	      auto &s0 = solutions[iq], &s1 = solutions[iq+1];
	      
	      // NB: y0 & y1 values do not matter; what matters is that they were equidistant 
	      // in the previous loop; FIXME: add some smearing later;
	      photon->m_PDF.AddMember(new UniformPDF(s0.GetTheta(), s1.GetTheta(), 1.0));
	    } //for iq
	  }
	} //for photon
      }
    } //if
  } //for rhistory

  // And now that IRT is performed on a grid of possible emission vertex locations, 
  // populate the requested mass hypothesis array;
  for(unsigned ih=0; ih<pid.GetHypothesesCount(); ih++) {
    auto hypothesis = pid.GetHypothesis(ih);
    double m = hypothesis->GetMass();

    // Reset photon count and weight;
    hypothesis->Reset();
    
    for(auto rhistory: GetRadiatorHistory()) {
      auto radiator = GetRadiator(rhistory);
      auto history  = GetHistory (rhistory);
      
      if (history->StepCount() >= 2) {
	auto from = history->GetStep(0), to = history->GetStep(history->StepCount()-1);
	TVector3 p = 0.5*(from->GetMomentum() + to->GetMomentum());
	
	double pp = p.Mag(), arg = sqrt(pp*pp + m*m)/(radiator->m_AverageRefractiveIndex*pp);
	// Threshold check; FIXME: do it better?;
	if (fabs(arg) > 1.0) continue;
	
	{
	  double theta = acos(arg);
	  
	  for(auto photon: history->Photons()) {
	    if (!photon->WasDetected()) continue;
	    
	    hypothesis->IncrementWeight(photon->m_PDF.GetWithinRangeCount(theta), 
					photon->m_PDF.GetValue           (theta)/zdim);
	  } //for photon
	}
      } //if
    } //for rhistoty
  } //for ih
} // ChargedParticle::Reconstruction()

// -------------------------------------------------------------------------------------
