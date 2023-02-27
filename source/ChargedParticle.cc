
//
//  This code is independent of the details of how the trajectory points were 
//  evaluated; it just expects m_Locations[] vector to be populated for every radiator;
//
//  It can also take a pre-mixed vector of all photons rather than using MC truth 
// information about which photn was produced in which radiator;
//

#include <TRandom.h>

#include <ChargedParticle.h>

// -------------------------------------------------------------------------------------

void ChargedParticle::PIDReconstruction(CherenkovPID &pid, bool use_seed)
{
#if _BACK_
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
    const auto irts = pd->GetIRTs(photon->GetVolumeCopy());
    if (!irts) {
      printf("No photosensor with this cellID found!\n");
      continue;
    } //if

    // Loop through all optical paths for this photosensor;
    //    unsigned counter = 0;
    for(auto irt: *irts) {
      //if (!counter) goto _next;

      for(auto rhistory: GetRadiatorHistory()) {
	auto radiator = GetRadiator(rhistory);
	unsigned zdim = radiator->GetTrajectoryBinCount();
	if (radiator->m_Locations.size() != zdim+1) continue;
	
	TVector3 phx = photon->GetDetectionPosition();
#if 0
	phx += TVector3(gRandom->Gaus(0.0, 0.6), gRandom->Gaus(0.0, 0.6), 0.0);
#endif
	
	// Get effective attenuation length for this radiator, as well as the 
	// parameterization of its rear surface in this particular sector; this is
	// not really a clean procedure for dRICH aerogel, but should be good enough 
	// in most part of the cases;
	double attenuation = radiator->GetReferenceAttenuationLength();
	auto rear = radiator->GetRearSide(irt->GetSector());//photon->GetVolumeCopy());
	
	{
	  bool all_converged = true;
	  IRTSolution solutions[zdim+1], seed;
	  double weights[zdim+1];
	  for(unsigned iw=0; iw<zdim+1; iw++) 
	    weights[iw] = 0.0;
	  
	  if (use_seed) seed.SetSeed(photon->GetVertexMomentum().Unit());
	  //printf("ChargedParticle::PIDReconstruction(): %f %f\n", photon->GetVertexMomentum().Theta(), 
	  //photon->GetVertexMomentum().Phi());//Unit().Y(), photon->GetVertexMomentum().Unit().Z()); 
	  //printf("Here-Q! %f %f\n", seed.m_Theta, seed.m_Phi);
	  photon->m_Phi[radiator] = 0.0;
	  
	  for(unsigned iq=0; iq<zdim+1; iq++) {
	    auto &solution = solutions[iq] = 
	      irt->Solve(radiator->m_Locations[iq].first,
			 // FIXME: give beam line as a parameter;
			 radiator->m_Locations[iq].second.Unit(), phx, TVector3(0,0,1), false, use_seed ? &seed : 0);
	    //printf("%2d %d\n", iq, solution.Converged());//all_converged);
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
	    //printf("%2d -> %7.4f %7.4f \n", iq, (weights[iq] + weights[iq+1])/2, (s0.GetTheta() + s1.GetTheta())/2);
	    photon->_m_PDF[radiator].AddMember(new UniformPDF(s0.GetTheta(), s1.GetTheta(), 
							      (weights[iq] + weights[iq+1])/2));//1.0));
	    //printf("attenuation=%f  weight=%f\n",attenuation,(weights[iq] + weights[iq+1])/2);
	    //photon->_m_PDF[radiator].AddMember(new UniformPDF(s0.GetTheta(), s1.GetTheta(), fabs(cos(s0.GetPhi()))));

	    //if (!counter) continue;//goto _next;
	  } //for iq
	}
      } //for rhistory

      //_next:
      //counter++;
    } //for irt
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

	  //double qzdim = pdf->GetMemberCount();

	  // FIXME: unreadable;
	  auto within_range = float(pdf->GetWithinRangeCount(theta, dth));
	  //if (within_range) photon->m_Selected = true;
	  if (within_range) photon->_m_Selected.insert(std::make_pair(ih, radiator));// = true;

	  // FIXME: 'zdim' is a poor estimate here if >1 optical path possible;
	  hypothesis->IncrementWeight(radiator, within_range/zdim, 
				      (dth ? (gaussian ? pdf->GetGaussianIntegral(theta,dth) :
					      pdf->GetRangeIntegral(theta - dth, theta + dth)) : 
				       pdf->GetValue(theta))/zdim);
	} //for photon
      }
    } //for rhistory
  } //for ih
#endif
} // ChargedParticle::PIDReconstruction()

// -------------------------------------------------------------------------------------

void ChargedParticle::ProcessHits(std::vector<DigitizedHit> &hits, bool use_seed)
{
  // Loop through all digitized hits of a given event; apply IRT on a pre-calculated 
  // [vertex,momentum] pair for each hit-to-radiator association; no sampling any 
  // longer (assume gaussian errors);
  //
  // FIXME: add orphan (a la DCR) photons;
  for(auto &hit: hits) {
    // Loop through all optical paths for this photosensor;
    for(auto irt: *hit.m_IRTs) {
      for(auto rhistory: GetRadiatorHistory()) {
	auto history  = GetHistory (rhistory);
	auto radiator = GetRadiator(rhistory);
	auto tag = std::make_pair(radiator, irt);
	
	{
	  IRTSolution seed;

	  // Loop through all of the provided seeds until IRT converges (yes, assume 
	  // the solution is unique);
	  unsigned smax = use_seed ? hit.m_DirectionSeeds.size() : 1;
	  for(unsigned iq=0; iq<smax; iq++) {
	    if (use_seed) seed.SetSeed(hit.m_DirectionSeeds[iq]);

	    auto &solution = hit.m_Solutions[this].m_All[tag] = 
	      irt->Solve(history->m_EstimatedVertex,
			 //irt->Solve(radiator->m_AverageVertex,
			 // FIXME: give beam line as a parameter;
			 history->m_AverageParentMomentum.Unit(), hit.GetDetectionPosition(), 
			 //radiator->m_AverageMomentum.Unit(), hit.GetDetectionPosition(), 
			 TVector3(0,0,1), false, use_seed ? &seed : 0);
	    //solution.m_Time = history->m_AverageTime + solution.m_Length/300;
	    solution.m_Time = solution.m_Length/300;

	    if (solution.m_Converged) break;
	  } //for iq
	  //solution.m_Time = radiator->m_AverageTime + solution.m_Length/300;
	}
      } //for rhistory
    } //for irt
  } //for hit
} // ChargedParticle::ProcessHits()

// -------------------------------------------------------------------------------------

double ChargedParticle::GetRecoCherenkovPhotonTheta(unsigned id)
{
  if (id >= m_Hits.size()) 
    return -1.0;
  else
    return m_Hits[id]->m_Solutions[this].m_Best->GetTheta();
} // ChargedParticle::GetRecoCherenkovPhotonTheta()

// -------------------------------------------------------------------------------------

double ChargedParticle::GetRecoCherenkovPhotonPhi(unsigned id)
{
  if (id >= m_Hits.size()) 
    return -1.0;
  else
    return m_Hits[id]->m_Solutions[this].m_Best->GetPhi();
} // ChargedParticle::GetRecoCherenkovPhotonPhi()

// -------------------------------------------------------------------------------------

double ChargedParticle::GetRecoCherenkovAverageTheta(CherenkovRadiator *radiator)
{
  unsigned stat = 0;
  double sum = 0.0;

  for(auto hit: m_Hits) {
    auto solution = hit->m_Solutions[this].m_Best;
    if (radiator && hit->m_Solutions[this].GetRadiator() != radiator) continue;
    
    stat++;
    sum += solution->GetTheta();
  } //for hit
  
  if (stat) sum /= stat;

  return sum;
} // ChargedParticle::GetRecoCherenkovAverageTheta()

// -------------------------------------------------------------------------------------

double ChargedParticle::GetMocaCherenkovAverageTheta(CherenkovRadiator *radiator)
{
  unsigned stat = 0;
  double sum = 0.0;

  for(auto rhptr: GetRadiatorHistory()) {
    auto rptr = GetRadiator(rhptr);

    if (rptr != radiator) continue; 

    for(auto photon: GetHistory(rhptr)->Photons()) {    
      auto n1 = photon->GetVertexParentMomentum().Unit(), n2 = photon->GetVertexMomentum().Unit();

      stat++;
      sum += acos(n1.Dot(n2));
    } //for photon
  } //for rhptr
  
  if (stat) sum /= stat;

  return sum;
} // ChargedParticle::GetMocaCherenkovAverageTheta()

// -------------------------------------------------------------------------------------

unsigned ChargedParticle::GetRecoCherenkovPhotonCount(CherenkovRadiator *radiator)
{
  unsigned stat = 0;

  //printf("... %5d\n", m_Hits.size());
  for(auto hit: m_Hits) {
    if (radiator && hit->m_Solutions[this].GetRadiator() != radiator) continue;
    
    stat++;
  } //for hit

  //printf("... %5d\n", stat);
  return stat;
} // ChargedParticle::GetRecoCherenkovPhotonCount()

// -------------------------------------------------------------------------------------
