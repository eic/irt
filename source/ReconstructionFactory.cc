
// Hits with chi^2 built in a 2D space (theta mismatch and timing mismatch) will 
// be accepted as "good" ones if the respective CCDF is more than this number;
#define _SINGLE_HIT_CCDF_CUT_DEFAULT_              (0.001)

// Especially this one is problematic since it should account for the size of radial bands;
//#define _EXPECTED_BG_PHOTON_COUNT_         ( 0.5)

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TF1.h>
#include <TDatabasePDG.h>
#include <Math/ProbFunc.h>

#include <CherenkovDetectorCollection.h>
#include <CherenkovEvent.h>
#include <ReconstructionFactory.h>

// -------------------------------------------------------------------------------------

ReconstructionFactory::ReconstructionFactory(const char *dfname, const char *cfname, 
					     const char *dname):
  GeantImport(dfname, cfname, dname),
  m_UseTimingInChiSquare(true),
  m_SingleHitCCDFcut(_SINGLE_HIT_CCDF_CUT_DEFAULT_),
  m_ResolveHitOwnership(true),
  m_UsePoissonTermInChiSquare(true),
  m_ExperimentalMode(false), 
  // Help conical mirrors a bit;
  m_UseMcTruthPhotonDirectionSeed(true)
{
} // ReconstructionFactory::ReconstructionFactory() 

// -------------------------------------------------------------------------------------

int ReconstructionFactory::AddHypothesis(int pdg)
{
  auto ptr = m_DatabasePDG->GetParticle(pdg);

  // Some sanity check; efficiency is of no concern here;
  for(auto hypo: m_Hypotheses)
    if (hypo == ptr)
      return -1;

  m_Hypotheses.push_back(ptr);

  return 0;
} // ReconstructionFactory::AddHypothesis()

// -------------------------------------------------------------------------------------

int ReconstructionFactory::AddHypothesis(const char *pdg)
{
  auto ptr = m_DatabasePDG->GetParticle(pdg);
  
  // Avoid code duplication;
  return (ptr ? AddHypothesis(ptr->PdgCode()) : -1);
} // ReconstructionFactory::AddHypothesis()

// -------------------------------------------------------------------------------------

void ReconstructionFactory::LaunchRingFinder(bool calibration)
{
  unsigned const hdim = m_Hypotheses.size();
  
  unsigned cdim = 1, pdim = Event()->ChargedParticles().size(), icbest = 0;
  // FIXME: come on!;
  for(unsigned iq=0; iq<Event()->ChargedParticles().size(); iq++)
    cdim *= hdim;
  // Save intermediate chi^2 estimates;
  double  ccdfevsave[cdim];       memset(ccdfevsave, 0x00, sizeof(ccdfevsave));
  double  ccdftrsave[cdim][pdim]; memset(ccdftrsave, 0x00, sizeof(ccdftrsave));
  unsigned npetrsave[cdim][pdim]; memset(npetrsave,  0x00, sizeof(npetrsave));
  
  // Now loop through all [particle,hypothesis] combinations and evaluate their chi^2;
  for(unsigned itr=0; itr<2; itr++) {
    for(unsigned ic=0; ic<cdim; ic++) {
      if (itr && ic != icbest) continue;

      // A running variable, to simplify hypothesis calculation;
      unsigned id = ic;  

      // FIXME: may want this check to be a bit more efficient; indeed for calibration pass
      // we better use MC truth as a PID hypothesis for all particles to calculate <theta>
      // as well as the width of this distribution; so skip all other combinations;
      if (calibration) {
	// A local running variable, to simplify hypothesis calculation;
	unsigned qid = ic;      
	
	for(auto mcparticle: Event()->ChargedParticles()) {
	  unsigned hypo = qid%hdim;
      
	  if (abs(mcparticle->GetPDG()) != abs(m_Hypotheses[hypo]->PdgCode())) 
	    goto _next_combination;

	  qid /= Event()->ChargedParticles().size();
	} //for mcparticle
      } //if    

      // Clear the hit attachment table;
      for(auto &hit: Hits()) {
	hit.m_BackgroundCandidate = false;
	hit.m_Selected.clear();
      } //for hit      

      for(auto mcparticle: Event()->ChargedParticles()) {
	unsigned hypo = id%hdim;
	auto hyparticle = m_Hypotheses[hypo];
	
	// Re-tune average refractive indices, depending on the incident theta angle; 
	// FIXME: in fact should also adjust them in ALL radiators, since it is not only the 
	// average emission angle (say for aerogel), but also IRT algorithm which (slightly) 
	// depends on this;
	double theta = mcparticle->GetVertexMomentum().Theta();
	unsigned ibin = (unsigned)floor(theta / _THETA_BIN_WIDTH_);
 	{	  
	  // NB: here the loop is over selected radiators only;
	  for(auto radiator: m_SelectedRadiators) 
	    // This one will be used in IRT tracing and time calculation;
	    radiator->SetReferenceRefractiveIndex(radiator->m_Calibrations[ibin].m_AverageRefractiveIndex);
	}

	// FIXME: why do we need these intermediate arrays?!;
	std::map<CherenkovRadiator*, double> momenta;
	std::map<CherenkovRadiator*, double> paths;
	for(auto rhptr: mcparticle->GetRadiatorHistory()) {
	  auto radiator = mcparticle->GetRadiator(rhptr);
	  
	  momenta[radiator] = mcparticle->GetHistory(rhptr)->m_AverageParentMomentum.Mag();
	  paths  [radiator] = mcparticle->GetHistory(rhptr)->m_EstimatedPath;
	} //for rhptr
	
	// FIXME: memory leak; looks like nothing else needs to be cleaned up here?;
	for(auto &hit: Hits()) 
	  hit.m_Solutions[mcparticle].m_All.clear();
	
	// FIXME: 'true': use 3D direction seeds (MC truth); otherwise conical mirror case 
	// is problematic; do it better later;
	mcparticle->ProcessHits(Hits(), m_UseMcTruthPhotonDirectionSeed);//true);
	
	//
	// By this moment every detected hit is evaluated with respect to this particle 
	// trajectory as originated from every radiator via every possible optical path; 
	//
	
	// Loop through all hits; 
	for(auto &hit: Hits()) {
	  auto sptr = &hit.m_Solutions[mcparticle];

	  // Will never have means to know from which radiator a given photon originated, and 
	  // via which optical path it arrived to the sensor; just choose "the best" [radiator,irt] 
	  // combination, which this particular particle PID hypothesis can find; the criterion 
	  // is "closest in theta and closest in timing" as evaluated via a chi^2 estimate with ndf=2, 
	  // or just "closest in theta" in case of ndf=1;
	  sptr->m_Best = 0;
	  
	  for(auto &tag: hit.m_Solutions[mcparticle].m_All) {
	    auto radiator = tag.first.first;
	    auto &solution = tag.second;

	    if (!IsSelected(radiator)) continue;
	    
	    double hsigma = (calibration || !AutomaticCalibrationRequired()) ?
	      //radiator->GetSmearing() : radiator->m_Calibrations[ibin].m_Csigma;
	      GetDefaultSinglePhotonThetaResolution() : radiator->m_Calibrations[ibin].m_Csigma;
	    
	    double pp = momenta[radiator], ll = paths[radiator];
	    
	    for(unsigned ih=0; ih<hdim; ih++) {
	      double m = m_Hypotheses[ih]->Mass();
	      
	      double beta = 1./sqrt(1. + pow(m/pp, 2)), tt = ll/(beta*300);

	      // FIXME: exception;
	      double thp = acos(sqrt(pp*pp + m*m)/(radiator->n()*pp));
	      double thdiff = solution.GetTheta() - thp - radiator->m_Calibrations[ibin].m_Coffset;
	      double tmdiff = (tt + solution.m_Time) - hit.GetAverageDetectionTime();
	      
	      unsigned ndf = 1;
	      double chi2 = pow(thdiff, 2)/pow(hsigma, 2);
	      if (m_UseTimingInChiSquare && GetSinglePhotonTimingResolution()) {
		ndf++;
		chi2 += pow(tmdiff, 2)/pow(GetSinglePhotonTimingResolution(), 2);
	      } //if
	      double ccdf = ROOT::Math::chisquared_cdf_c(chi2, ndf);
	      
	      if (ih == hypo) {
		if (!sptr->m_Best || ccdf > sptr->m_ccdf) {
		  sptr->AssignVariables(&solution, thp, tt, radiator, ccdf);
		} //if
	      } else {
		// The logic behind this is that all hits within +/-N*sigma window
		// with respect to at least one hypothesis in at least one radiator
		// for at least one optical path for every particle belong to the 
		// club "which needs to be explained" in terms of Poisson statistics;
		if (ccdf > m_SingleHitCCDFcut) hit.m_BackgroundCandidate = true;
	      } //if
	    } //for ih
	  } //for tag 
	  
	  {
	    auto *best = sptr->m_Best;
	    
	    if (best) {
	      unsigned ndf = 1;
	      double thdiff = best->GetTheta() - sptr->m_thp - sptr->m_rbest->m_Calibrations[ibin].m_Coffset;
	      double hsigma = (calibration || !AutomaticCalibrationRequired()) ?
		//sptr->m_rbest->GetSmearing() : sptr->m_rbest->m_Calibrations[ibin].m_Csigma;
		GetDefaultSinglePhotonThetaResolution() : sptr->m_rbest->m_Calibrations[ibin].m_Csigma;
	      double chi2 = pow(thdiff, 2)/pow(hsigma, 2);
	      double tmdiff = (sptr->m_tt + best->m_Time) - hit.GetAverageDetectionTime();
	      if (m_UseTimingInChiSquare && GetSinglePhotonTimingResolution()) {
		ndf++;
		chi2 += pow(tmdiff, 2)/pow(GetSinglePhotonTimingResolution(), 2);
	      } //if
	      double ccdf = ROOT::Math::chisquared_cdf_c(chi2, ndf);
	      
	      // FIXME: should be radiator-specific;
	      if (ccdf > m_SingleHitCCDFcut) {
		hit.m_Selected.insert(std::make_pair(mcparticle, chi2));
		
		// Sure of interest is only to see a distribution for the right hypothesis; 
		if (mcparticle->GetPDG() == hyparticle->PdgCode()) {
		  if (itr && !calibration) m_hthph->Fill(1000*thdiff);
		  if (itr &&  calibration) 		    
		    sptr->m_rbest->m_Calibrations[ibin].m_hcalib->Fill(1000*thdiff);
		  if (itr && !calibration) m_hdtph->Fill(1000*tmdiff);		  
		  if (itr && !calibration) m_hccdfph->Fill(ccdf);
		} //if
	      } //if
	    } //if
	  }
	} //for hits
	
	id /= Event()->ChargedParticles().size();
      } //for mcparticle
      
      // Clean up background candidate list; obviously if at least one particle selected this 
      // hit for a hypothesis considered in this iteration, the hit is useful;
      for(auto &hit: Hits()) 
	if (hit.m_Selected.size())
	  hit.m_BackgroundCandidate = false;
      
      // Now either resolve ambiguous assignments, based on the hit-to-track chi^2 match for this 
      // PID hypothesis, or eliminate these hits everywhere; FIXME: any better idea?;
      for(auto &hit: Hits()) 
	if (hit.m_Selected.size() >= 1) {
	  if (m_ResolveHitOwnership) {
	    double chi2min = 0.0;
	    // FIXME: perhaps order them at the creation time?;
	    for(auto it = hit.m_Selected.begin(); it != hit.m_Selected.end(); it++)
	      if (it == hit.m_Selected.begin() || it->second < chi2min)
		chi2min = it->second;
	    for(auto it = hit.m_Selected.begin(); it != hit.m_Selected.end(); )
	      if (it->second != chi2min)
		it = hit.m_Selected.erase(it);
	      else
		it++;
	  }
	  else
	    hit.m_Selected.clear();
	} //for .. if
      
      // And eventually calculate the overall event chi^2;
      {
	unsigned nbg = 0;
	for(auto &hit: Hits()) 
	  if (hit.m_BackgroundCandidate) nbg++;
	
	unsigned ndfev = 0, idx = 0;
	double chi2ev = 0;
	for(auto mcparticle: Event()->ChargedParticles()) {
	  unsigned npe = 0;
	  double chi2tr = 0.0;
	  
	  for(auto &hit: Hits()) 
	    for(auto selected: hit.m_Selected)
	      if (selected.first == mcparticle) {
		npe++;
		double chi2ph = selected.second;
		chi2tr += chi2ph;
		
		if (itr) mcparticle->AddHit(&hit);
		
		// FIXME: there is only one entry selected here?;
		break;
	      } //for..if
	  
	  unsigned ndftr = m_UseTimingInChiSquare && GetSinglePhotonTimingResolution() ? 2*npe : 1*npe;
	  // Poissonic term is optional;
	  if (!calibration && itr && m_UsePoissonTermInChiSquare) {
	    for(auto rhptr: mcparticle->GetRadiatorHistory()) {
	      auto radiator = mcparticle->GetRadiator(rhptr);

	      if (!IsSelected(radiator)) continue;

	      double theta = mcparticle->GetRecoCherenkovAverageTheta(radiator);
	      //printf("%7.3f -> %3d ... expect %7.1f\n", theta, npe, 347.*0.36*pow(sin(theta), 2));
	      if (theta) {
		//printf("%f %f\n", radiator->m_YieldCff, radiator->m_DetectedToCalibrationPhotonRatio);//m_Calibrations[ibin]);
		//double texp = 347.*0.36*pow(sin(theta), 2);
		double texp = radiator->m_YieldCff*radiator->m_DetectedToCalibrationPhotonRatio*pow(sin(theta), 2);
		chi2tr += npe ? 2*(texp - npe + npe*log(npe/texp)) : 0.0;
		ndftr++;
	      }
	    }
	  } //if
	  ccdftrsave[ic][idx] = ROOT::Math::chisquared_cdf_c(chi2tr, ndftr);
	  npetrsave [ic][idx] = npe;
	  
	  ndfev  += ndftr;
	  chi2ev += chi2tr;
	  // This is dumb of course, but do not want to change std::set to std::vector;
	  idx++;
	} //for mcparticle
	
#ifdef _EXPECTED_BG_PHOTON_COUNT_
	if (hdim >= 2) {
	  ndfev++;
	  double bexp = _EXPECTED_BG_PHOTON_COUNT_;
	  chi2ev += nbg ? 2*(bexp - nbg + nbg*log(nbg/bexp)) : 0.0;
	} //if
#endif
	double ccdfev = ROOT::Math::chisquared_cdf_c(chi2ev, ndfev);
	ccdfevsave[ic] = ccdfev;
	
	if (!ic || ccdfev > ccdfevsave[icbest]) icbest = ic;
      }

    _next_combination:
      ;
    } //for ic
  } //for itr
  
  if (calibration) 
    UpdateYields();
  else {
    // Now check how the PID procedure performed and fill out histograms;
    // Sure of interest is only to see a flat CCDF distribution for the right combination 
    // of event-level hypotheses; 
    m_hccdfev->Fill(ccdfevsave[icbest]);
    
    for(unsigned ip=0; ip<Event()->ChargedParticles().size(); ip++) {
      //if (npetrsave [icbest][ip])
      m_hccdftr->Fill(ccdftrsave[icbest][ip]);
      m_hnpetr ->Fill(npetrsave [icbest][ip]);
    } //for ip
    
    {
      // A running variable, to simplify hypothesis calculation;
      unsigned id = icbest;
      
      for(auto mcparticle: Event()->ChargedParticles()) {
      //for(unsigned ip=0; ip<Event()->ChargedParticles().size(); ip++) {
      //auto mcparticle = *Event()->ChargedParticles()[ip];

	unsigned hypo = id%hdim;
	auto rcparticle = m_Hypotheses[hypo];

	//if (mcparticle->m_HadronicInteractionOccured) continue;
	
	if (mcparticle->GetRecoCherenkovHitCount() == 0) {
	  m_hmatch->Fill(0.5);
	} else if (abs(mcparticle->GetPDG()) == abs(rcparticle->PdgCode())) {
	  m_hmatch->Fill(1.5);
	} else {
	  m_hmatch->Fill(2.5);
	  if (BeVerbose()) printf("PID Failure!\n");
	} //if	  

	// Make it the same sign as for the MC particle (assume tracker can 
	// determine the charge sign);
	int sign = mcparticle->GetPDG() < 0 ? -1 : 1;
	mcparticle->SetRecoPdgCode(sign*abs(rcparticle->PdgCode()));
	
	id /= Event()->ChargedParticles().size();
      } //for ip
    }
  } //if
} // ReconstructionFactory::LaunchRingFinder()

// -------------------------------------------------------------------------------------

CherenkovEvent *ReconstructionFactory::GetEvent(unsigned ev, bool calibration)
{
  // Prepair for next event;
  ClearBlackoutCells();
  ClearDigitizedHits();

  // Get it from the ROOT tree;
  GetInputTreeEntry(ev);

  // Use undetected photons (HERE DO THIS ON TRACK PER TRACK BASIS) to extract the 
  // expected average emission point 3D location, time and parent particle 3D momentum;  
  if (!m_ExperimentalMode) CalibratePhotonEmissionPoints();

  // Loop through all photons (both calibration and detected ones) of all tracks and 
  // produce event-level hit array; "calibration" (undetected) photons will be passed through 
  // exactly the same digitization procedure as the "detected" ones, and used to either
  // evaluate the expected reconstruction procedure performance (say, SPE Cherenkov 
  // angle resolution) or mark "exclusion area" where window and gas photons hit the
  // photocathode, or such;
  ProduceDigitizedHits(calibration);

  //   There are no optical photons beyond this point; their information was merged into 
  // the DigitizedHit array; the only cheating left are direction seeds, but this is rather a
  // procedural issue than using MC truth information illegally; and t0 resolution is supposed to 
  // be ideal (but it will anyway be much better than anticipated HRPPD/SiPM single photon TTS);
  //
  // Now go through the combinatorics, build chi^2 for each of the M^{N} (M - number of suggested
  // hypotheses, N - number of charged tracks) combinations, by fishing out digitized hits which fit 
  // a current combination of the particle hypothesis best, and choose the best one; performance is
  // a non-issue here at the moment, let AI/ML do the job better later; at present complexity seems
  // to scale with H*M^{N}, where H is the number of digitized hits;
  if (!m_ExperimentalMode) LaunchRingFinder(calibration);
  
  if (BeVerbose() && !(ev%100)) printf("Event %5d ...\n", ev);

  return Event();
} // ReconstructionFactory::GetEvent()

// -------------------------------------------------------------------------------------
