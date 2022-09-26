
//
//  This code is independent of the details of how the trajectory points were 
//  evaluated; it just expects m_Locations[] vector to be populated for every radiator;
//
//  It can also take a pre-mixed vector of all photons rather than using MC truth 
// information about which photn was produced in which radiator;
//

#include <ChargedParticle.h>

// -------------------------------------------------------------------------------------

void ChargedParticle::PIDReconstruction(CherenkovPID &pid)
{
  std::vector<OpticalPhoton*> photons;

  // Mix all photons together;
  for(auto rhistory: GetRadiatorHistory()) 
    for(auto photon: GetHistory(rhistory)->Photons())
      photons.push_back(photon);

  for(auto photon: photons)
    photon->_m_Selected.clear();// = false;

  // Loop through all of the photons recorded in all radiators; apply IRT on a fixed grid 
  // of emission vertex locations and build kind of a PDF out of that to calculate weights;
  // this approach may not be dramatically efficient, but it would 1) work in case of the 
  // magnetic field bending, 2) allow easy extention to a dRICH case where each mass 
  // hypothesis would evaluate each of the detected photons on the aerogel and gas grids
  // independently and build a sum of their PDFs without much thinking about outlier photon
  // rejection, average theta calculation and such;
  for(auto photon: photons) {
    if (!photon->WasDetected()) continue;

    auto pd = photon->GetPhotonDetector();
    const auto irt = pd->GetIRT(photon->GetVolumeCopy());
    if (!irt) {
      printf("No photosensor with this cellID found!\n");
      continue;
    } //if

    for(auto rhistory: GetRadiatorHistory()) {
      auto radiator = GetRadiator(rhistory);
      unsigned zdim = radiator->GetTrajectoryBinCount();
      if (radiator->m_Locations.size() != zdim+1) continue;
      
      TVector3 phx = photon->GetDetectionPosition();
      
      // Get effective attenuation length for this radiator, as well as the 
      // parameterization of its rear surface in this particular sector; this is
      // not really a clean procedure for dRICH aerogel, but should be good enough 
      // in most part of the cases;
      double attenuation = radiator->GetReferenceAttenuationLength();
      auto rear = radiator->GetRearSide(irt->GetSector());//photon->GetVolumeCopy());

      {
	bool all_converged = true;
	IRTSolution solutions[zdim+1];
	double weights[zdim+1] = {0.0};
	
	photon->m_Phi[radiator] = 0.0;

	for(unsigned iq=0; iq<zdim+1; iq++) {
	  auto &solution = solutions[iq] = irt->Solve(radiator->m_Locations[iq].first,
		       // FIXME: give beam line as a parameter;
		       radiator->m_Locations[iq].second.Unit(), phx, TVector3(0,0,1), false);
	  if (!solution.Converged()) {
	    all_converged = false;
	    break;
	  } //if

	  photon->m_Phi[radiator] += solution.GetPhi();

	  if (attenuation) {
	    TVector3 from = radiator->m_Locations[iq].first, to;
	    bool ok = rear->GetCrossing(from, solution.m_Direction, &to);
	    if (ok) {
	      double length = (to - from).Mag();
	      //printf("%02d -> %7.2f\n", iq, radiator->m_Locations[iq].first.z());
	      //weights[iq] = 1.0;
	      weights[iq] = exp(-length / attenuation);
	    } //if
	  } else
	    weights[iq] = 1.0;
	} //for iq
	
	if (!all_converged) continue;
	
	photon->m_Phi[radiator] /= (zdim+1);

	for(unsigned iq=0; iq<zdim; iq++) {
	  auto &s0 = solutions[iq], &s1 = solutions[iq+1];
	  
	  // NB: y0 & y1 values do not matter; what matters is that they were equidistant 
	  // in the previous loop; FIXME: add some smearing later;
	  //photon->_m_PDF[radiator].AddMember(new UniformPDF(s0.GetTheta(), s1.GetTheta(), 1.0));
	  //printf("%2d -> %7.3f\n", iq, weights[iq]);
	  photon->_m_PDF[radiator].AddMember(new UniformPDF(s0.GetTheta(), s1.GetTheta(), 
							    (weights[iq] + weights[iq+1])/2));//1.0));
	  //photon->_m_PDF[radiator].AddMember(new UniformPDF(s0.GetTheta(), s1.GetTheta(), fabs(cos(s0.GetPhi()))));
	} //for iq
      }
    } //for rhistory
  } //for photon

  // And now that IRT is performed on a grid of possible emission vertex locations, 
  // populate the requested mass hypothesis array;
  for(unsigned ih=0; ih<pid.GetHypothesesCount(); ih++) {
    auto hypothesis = pid.GetHypothesis(ih);
    double m = hypothesis->GetMass();

    // Reset photon count and weight;
    hypothesis->Reset();
    
    for(auto rhistory: GetRadiatorHistory()) {
      auto radiator = GetRadiator(rhistory);
      unsigned zdim = radiator->GetTrajectoryBinCount();
      if (radiator->m_Locations.size() != zdim+1) continue;
	
      // Asume this estimate is good enough;
      double pp = radiator->m_Locations[0].second.Mag();
      double arg = sqrt(pp*pp + m*m)/(radiator->m_AverageRefractiveIndex*pp);
      // Threshold check; FIXME: do it better?;
      if (fabs(arg) > 1.0) continue;
      
      {
	double theta = acos(arg), dth = radiator->GetSmearing();
	bool gaussian = radiator->UseGaussianSmearing();
	// +/-3 sigma; FIXME: do it better later;
	if (gaussian) dth *= 3;

	for(auto photon: photons) {
	  if (!photon->WasDetected()) continue;
	  
	  auto pdf = &photon->_m_PDF[radiator];

	  // FIXME: unreadable;
	  auto within_range = float(pdf->GetWithinRangeCount(theta, dth));
	  //if (within_range) photon->m_Selected = true;
	  if (within_range) photon->_m_Selected.insert(std::make_pair(ih, radiator));// = true;

	  hypothesis->IncrementWeight(radiator, within_range/zdim, 
				      (dth ? (gaussian ? pdf->GetGaussianIntegral(theta,dth) :
					      pdf->GetRangeIntegral(theta - dth, theta + dth)) : 
				       pdf->GetValue(theta))/zdim);
	} //for photon
      }
    } //for rhistory
  } //for ih
} // ChargedParticle::PIDReconstruction()

// -------------------------------------------------------------------------------------
