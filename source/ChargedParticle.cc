
//
//  This code is independent of the details of how the trajectory points were 
//  evaluated; it just expects m_Locations[] vector to be populated for every radiator;
//
//  It can also take a pre-mixed vector of all photons rather than using MC truth 
// information about which photn was produced in which radiator;
//

#include <ChargedParticle.h>

// -------------------------------------------------------------------------------------

void ChargedParticle::PIDReconstruction(CherenkovPID &pid, std::vector<OpticalPhoton*> *photons)
{
  // Reset individual photon "selected" flags;
  if (photons) 
    for(auto photon: *photons)
      photon->m_Selected = false;
  else
    for(auto rhistory: GetRadiatorHistory()) 
      for(auto photon: GetHistory(rhistory)->Photons())
	photon->m_Selected = false;

  // Loop through all of the photons recorded in all radiators; apply IRT on a fixed grid 
  // of emission vertex locations and build kind of a PDF out of that to calculate weights;
  // this approach may not be dramatically efficient, but it would 1) work in case of the 
  // magnetic field bending, 2) allow easy extention to a dRICH case where each mass 
  // hypothesis would evaluate each of the detected photons on the aerogel and gas grids
  // independently and build a sum of their PDFs without much thinking about outlier photon
  // rejection, average theta calculation and such;
  for(auto rhistory: GetRadiatorHistory()) {
    auto radiator = GetRadiator(rhistory);
    // FIXME: error message;
    if (radiator->m_Locations.size() < 2) return;

    const unsigned zdim = radiator->m_Locations.size()-1;//radiator->GetTrajectoryBinCount();

    if (radiator->m_Locations.size()) {
      for(auto photon: (photons ? *photons : GetHistory(rhistory)->Photons())) {
	if (!photon->WasDetected()) continue;
	
	auto pd = photon->GetPhotonDetector();
	if (!pd->GetIRT(photon->GetVolumeCopy())) {
	  printf("No photosensor with this cellID found!\n");
	  continue;
	} //if

	TVector3 phx = photon->GetDetectionPosition();

	{
	  bool all_converged = true;
	  IRTSolution solutions[zdim+1];
	  
	  for(unsigned iq=0; iq<zdim+1; iq++) {
	    //printf("--> %ld %ld\n", photon->GetVolumeCopy(), (unsigned long)(pd->GetIRT(photon->GetVolumeCopy())));
	    //printf("--> %ld %ld\n", photon->GetVolumeCopy(), (photon->GetVolumeCopy() >> 8) & 0x7);
	    auto &solution = solutions[iq] = 
	      pd->GetIRT(photon->GetVolumeCopy())->Solve(radiator->m_Locations[iq].first,
							 radiator->m_Locations[iq].second.Unit(), 
							 // FIXME: give beam line as a parameter;
							 phx, TVector3(0,0,1), false);
	    if (!solution.Converged()) {
	      all_converged = false;
	      break;
	    } //if
	  } //for iq
	  
	  if (!all_converged) continue;

	  for(unsigned iq=0; iq<zdim; iq++) {
	    auto &s0 = solutions[iq], &s1 = solutions[iq+1];
	    
	    //printf("  converged: %d %d\n", s0.Converged(), s1.Converged());
	    // NB: y0 & y1 values do not matter; what matters is that they were equidistant 
	    // in the previous loop; FIXME: add some smearing later;
	    photon->_m_PDF[radiator].AddMember(new UniformPDF(s0.GetTheta(), s1.GetTheta(), 1.0));
	  } //for iq
	}
      } //for photon
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
      //const unsigned zdim = radiator->GetTrajectoryBinCount();
      const unsigned zdim = radiator->m_Locations.size()-1;//radiator->GetTrajectoryBinCount();
      
      if (!radiator->m_Locations.size()) continue;

      TVector3 p = 0.5*(radiator->m_Locations[0].second + radiator->m_Locations[radiator->m_Locations.size()-1].second);
	
      double pp = p.Mag(), arg = sqrt(pp*pp + m*m)/(radiator->m_AverageRefractiveIndex*pp);
      // Threshold check; FIXME: do it better?;
      if (fabs(arg) > 1.0) continue;
      
      {
	double theta = acos(arg), dth = radiator->GetSmearing();
	bool gaussian = radiator->UseGaussianSmearing();
	// +/-3 sigma; FIXME: do it better later;
	if (gaussian) dth *= 3;

	for(auto photon: (photons ? *photons : GetHistory(rhistory)->Photons())) {
	  if (!photon->WasDetected()) continue;
	  
	  auto pdf = &photon->_m_PDF[radiator];

	  // FIXME: unreadable;
	  //printf("-> %7.2f\n", pdf->GetRangeIntegral(theta - dth, theta + dth));
	  auto within_range = float(pdf->GetWithinRangeCount(theta, dth));
	  if (within_range) photon->m_Selected = true;

	  hypothesis->IncrementWeight(radiator, within_range/zdim, 
				      (dth ? (gaussian ? pdf->GetGaussianIntegral(theta,dth) :
					      pdf->GetRangeIntegral(theta - dth, theta + dth)) : 
				       pdf->GetValue(theta))/zdim);
	  //printf("@W@ %2d -> %7.2f %7.2f\n", ih, m, 1000*theta);
	} //for photon
      }
    } //for rhistory
  } //for ih
} // ChargedParticle::PIDReconstruction()

// -------------------------------------------------------------------------------------
