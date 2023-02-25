
// Kind of digitization; may want to comment out (or set very fine) for debugging;
#define _SENSOR_ACTIVE_AREA_PIXELLATION_DEFAULT_       (32)

// Timing digitization in [ns];
#define _SINGLE_PHOTON_TIMING_RESOLUTION_DEFAULT_  (0.050)

// Hits with chi^2 built in a 2D space (theta mismatch and timing mismatch) will 
// be accepted as "good" ones if the respective CCDF is more than this number;
#define _SINGLE_HIT_CCDF_CUT_DEFAULT_              (0.001)

// Comment out if do not want to deal with the Poissonic terms in chi^2; FIXME: account 
// for yield drop at smaller momentum and yield gain when primary particle crosses aerogel 
// at a large angle;
//#define _EXPECTED_TRUE_PHOTON_COUNT_       (10.5)
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

// FIXME: yes, fix it please;
static unsigned hdim = 100;
static double hmin = -30.0, hmax = 30.0, hbin = (hmax - hmin)/hdim, hwnd = 15.0; 
static double csigma;

// -------------------------------------------------------------------------------------

ReconstructionFactory::ReconstructionFactory(const char *dfname, const char *cfname, 
					     const char *dname):
  m_Tree(0),
  // Means: shut down pad by pad, without summing up (which is kind of not ideal);
  m_BlackoutBlowupValue(0),
  m_Event(0),
  m_RICH(0),
  m_CurrentEvent(0), 
  m_UseTimingInChiSquare(true),
  m_SingleHitCCDFcut(_SINGLE_HIT_CCDF_CUT_DEFAULT_),
  m_ResolveHitOwnership(true), 
  m_SinglePhotonTimingResolution(_SINGLE_PHOTON_TIMING_RESOLUTION_DEFAULT_),
  m_SensorActiveAreaPixellation(_SENSOR_ACTIVE_AREA_PIXELLATION_DEFAULT_),
  m_UseAutomaticCalibration(false)
{
  if (!dfname) return;

  // FIXME: exception handling;
  auto fcfg = new TFile(cfname ? cfname : dfname);
  if (!fcfg) return;
  auto geometry = dynamic_cast<CherenkovDetectorCollection*>(fcfg->Get("CherenkovDetectorCollection"));
  auto fdata = new TFile(dfname);
  m_Tree = dynamic_cast<TTree*>(fdata->Get("t")); 
  m_Event = new CherenkovEvent();
  m_Tree->SetBranchAddress("e", &m_Event);

  m_hthph   = new TH1D("thph",   "Cherenkov angle (SPE)",             100,  -30,     30);
  m_hccdfph = new TH1D("ccdfph", "Single photon chi^2 CCDF",           50,    0,      1);
  m_hccdftr = new TH1D("ccdftr", "Track-level chi^2 CCDF",             50,    0,      1);
  m_hccdfev = new TH1D("ccdfev", "Event-level chi^2 CCDF",             50,    0,      1);
  m_hnpetr  = new TH1D("npetr",  "Photon count per track",             50,    0,     50);
  m_hdtph   = new TH1D("dtph",   "Cherenkov timing (SPE)",            100, -500,    500);
  m_hmatch  = new TH1D("match",  "PID evaluation correctness",          2,    0,      2);

  m_RICH = geometry->GetDetector(dname);

  m_DatabasePDG = new TDatabasePDG();
} // ReconstructionFactory::ReconstructionFactory() 

// -------------------------------------------------------------------------------------

void ReconstructionFactory::PerformCalibration(unsigned stat)
{
  unsigned nEvents = m_Tree->GetEntries();
  unsigned evmax = stat ? (stat < nEvents ? stat : nEvents) : nEvents;

  // First calculate average refractive index for all detected photons for every radiator;
  for(unsigned ev=0; ev<evmax; ev++) {
    m_Tree->GetEntry(ev);
    
    for(auto particle: m_Event->ChargedParticles()) {
      for(auto rhistory: particle->GetRadiatorHistory()) {
	auto radiator = particle->GetRadiator(rhistory);
	auto history  = particle->GetHistory (rhistory);
	
	for(auto photon: history->Photons()) {
	  // Choose only (unused) photons, which follow the same QE(lambda) distribution as the 
	  // detected ones, but did not make to be detected because of the efficiency cut;
	  if (!photon->IsUsefulForCalibration()) continue;
	  
	  radiator->m_Stat++;
	  radiator->m_AverageRefractiveIndex += photon->GetVertexRefractiveIndex();

	  //radiator->m_AverageTime     += photon->GetVertexTime();
	  //radiator->m_AverageVertex   += photon->GetVertexPosition();
	  //radiator->m_AverageMomentum += photon->GetVertexParentMomentum();
	} //for photon
      } //for radiator
    } //for particle   
  } //for ev
  
  for(auto rptr: m_RICH->Radiators()) {
    auto radiator = rptr.second;
    
    if (radiator->m_Stat) {
      radiator->m_AverageRefractiveIndex /=    radiator->m_Stat;

      //radiator->m_AverageTime            /=    radiator->m_Stat;
      //radiator->m_AverageVertex          *= 1./radiator->m_Stat;
      //radiator->m_AverageMomentum        *= 1./radiator->m_Stat;
    } //if
  } //for dt..radiator

  if (m_UseAutomaticCalibration) {
    m_hcalib  = new TH1D("calib",  "Cherenkov calibration angle (SPE)",hdim, hmin,   hmax);

    for(unsigned ev=0; ev<evmax; ev++) 
      GetEvent(ev, true);

    int max = m_hcalib->GetMaximumBin();
    double mean = hmin + max*hbin;
    TF1 fq("fq", "gaus(0)", mean - hwnd/2, mean + hwnd/2);
    fq.SetParameters(1000, mean, 4.0);
    m_hcalib->Fit("fq");
    double par[3];
    fq.GetParameters(par);
    csigma = par[2] / 1000;
    
    // Reset event counter;
    m_CurrentEvent = 0;
  }
} // ReconstructionFactory::PerformCalibration()

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

CherenkovEvent *ReconstructionFactory::GetNextEvent(bool calibration)
{
  if (m_CurrentEvent < m_Tree->GetEntries()) 
    return GetEvent(m_CurrentEvent++, calibration);
  else
    return 0;
} // ReconstructionFactory::GetNextEvent()

// -------------------------------------------------------------------------------------

CherenkovEvent *ReconstructionFactory::GetEvent(unsigned ev, bool calibration)
{
  m_BlackoutCells.clear();
  m_Hits.clear();

  m_Tree->GetEntry(ev);

  for(auto particle: m_Event->ChargedParticles()) {
    for(auto rhistory: particle->GetRadiatorHistory()) {
      auto history  = particle->GetHistory (rhistory);
      
      unsigned stat = 0;
      for(auto photon: history->Photons()) {
	// Choose only (unused) photons, which follow the same QE(lambda) distribution as the 
	// detected ones, but did not make to be detected because of the efficiency cut;
	if (!photon->IsUsefulForCalibration()) continue;
	
	stat++;
	history->m_AverageTime += photon->GetVertexTime();
	history->m_AverageVertex += photon->GetVertexPosition();
	history->m_AverageMomentum += photon->GetVertexParentMomentum();
      } //for photon
      
      if (stat) {
	history->m_AverageTime     /=    stat;
	history->m_AverageVertex   *= 1./stat;
	history->m_AverageMomentum *= 1./stat;
      } //if
    } //for radiator
  } //for particle
    
  // First mix all photons from all particles and "digitize" them; FIXME: add stray background;
  // NB: first iteration makes use of the "calibration" photons, for various purposes;
  for(unsigned itr=0; itr<2; itr++) {
    for(auto mcparticle: m_Event->ChargedParticles())
      for(auto rhistory: mcparticle->GetRadiatorHistory()) 
	for(auto photon: mcparticle->GetHistory(rhistory)->Photons()) {
	  auto pd = photon->GetPhotonDetector();
	  auto radiator = mcparticle->GetRadiator(rhistory);

	  //if (!photon->WasDetected() /*|| photon->m_ReflectionPoints.size() != 1*/) continue;
	  if (!itr && !photon->IsUsefulForCalibration())                 continue;
	  if ( itr && !photon->WasDetected()            && !calibration) continue;
	  if ( itr && !photon->IsUsefulForCalibration() &&  calibration) continue;
	  
	  // Here the trick comes: "calibration" (undetected) photons will be passed through 
	  // exactly the same digitization procedure as the "detected" ones, and used to either
	  // evaluate the expected reconstruction procedure performance (say, SPE Cherenkov 
	  // angle resolution) or mark "exclusion area" where window and gas photons hit the the 
	  // photocathode, or such;
	  {
	    DigitizedHit hit, *duplicate = 0;
	    
	    hit.m_PhotonDetector = pd;
	    hit.m_Copy = photon->GetVolumeCopy();
	    
	    hit.m_IRTs = pd->GetIRTs(photon->GetVolumeCopy());
	    if (!hit.m_IRTs) {
	      printf("No photosensor with this cellID found!\n");
	      continue;
	    } //if
	    
	    TVector3 phx = photon->GetDetectionPosition();
	    if (m_SensorActiveAreaPixellation) {
	      double size = pd->GetActiveAreaSize(), half = size/2;
	      // All IRTs are terminated at the photosensor; use the first one; extract 3D parameterization;
	      auto sensor = dynamic_cast<const FlatSurface*>((*hit.m_IRTs)[0]->tail()->GetSurface());
	      double lx = sensor->GetLocalX(phx) + half, ly = sensor->GetLocalY(phx) + half;
	      
	      // Smear the coordinate measurement (call it digitization :-); 
	      double pitch = size / m_SensorActiveAreaPixellation, x0 = -half, y0 = -half;
	      hit.m_iX = (int)floor(lx/pitch);
	      hit.m_iY = (int)floor(ly/pitch);
	      double dx = pitch*(hit.m_iX + 0.5), dy = pitch*(hit.m_iY + 0.5);
	      phx = sensor->GetSpacePoint(x0 + dx, y0 + dy);
	      
	      // FIXME: very inefficient;
	      for(auto &ptr: m_Hits)
		if (ptr.m_PhotonDetector == pd && ptr.m_Copy == hit.m_Copy && 
		    ptr.m_iX == hit.m_iX && ptr.m_iY == hit.m_iY) {
		  duplicate = &ptr;
		  break;
		} //for ptr .. if
	    } //if

	    {
	      unsigned div = 0x1 << m_BlackoutBlowupValue;

	      switch (itr) {
	      case 0:
		if (m_BlackoutRadiators.find(radiator) != m_BlackoutRadiators.end())
		  // NB: this lookup table is global for a given event, namely it is built on the 
		  // event-level (for all tracks at once rather than on a track-by-track basis);
		  m_BlackoutCells.insert(BlackoutCell(pd, hit.m_Copy, hit.m_iX/div, hit.m_iY/div));
		break;
	      case 1:
		// FIXME: this comparison is inefficient;
		if (m_BlackoutCells.find(BlackoutCell(pd, hit.m_Copy, hit.m_iX/div, hit.m_iY/div)) == m_BlackoutCells.end()) {
		  (duplicate ? duplicate : &hit)->m_DirectionSeeds.push_back(photon->GetVertexMomentum().Unit());
		  // Convert to [ps];
		  double timing = photon->GetDetectionTime() + 
		    (m_SinglePhotonTimingResolution ? m_rndm.Gaus(0.0, m_SinglePhotonTimingResolution) : 0.0);
		  (duplicate ? duplicate : &hit)->m_DetectionTimes.push_back(timing);
		  
		  if (!duplicate) {
		    // NB: in case of digitization, for duplicate hits this setting will be identical;
		    hit.m_DetectionPosition = phx;
		    
		    m_Hits.push_back(hit);
		  } //if
		} //if
		else
		  // Would not hurt to set individual blackout flag as well for those photons which 
		  // were "too close" on the sensor matrix to the ones produce in the window or in the 
		  // gas radiator; what is important though is that such photons will not produce ***hits*** 
		  // for ring imaging purposes;
		  photon->SetBlackoutFlag();
		break;
	      } //switch
	    }
	  }
	} //for particle..photon    
  } //for itr

  //
  //   There are no optical photons beyond this point; their information was merged into 
  // the DigitizedHit array; the only cheating left are direction seeds, but this is rather a
  // procedural issue than using MC truth information illegally; and t0 resolution is supposed to 
  // be ideal (but it will anyway be much better than anticipated HRPPD/SiPM single photon TTS);
  //
  
  unsigned const hdim = m_Hypotheses.size();
  
  unsigned cdim = 1, pdim = m_Event->ChargedParticles().size(), icbest = 0;
  // FIXME: come on!;
  for(unsigned iq=0; iq<m_Event->ChargedParticles().size(); iq++)
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
      
      // Clear hit attachment table;
      for(auto &hit: m_Hits) {
	hit.m_BackgroundCandidate = false;
	hit.m_Selected.clear();
      } //for hit      
      
      for(auto mcparticle: m_Event->ChargedParticles()) {
	unsigned hypo = id%hdim;
	auto hyparticle = m_Hypotheses[hypo];
	
	// Calculate average momenta value in the radiators;
	std::map<CherenkovRadiator*, double> momenta;
	for(auto rhistory: mcparticle->GetRadiatorHistory()) {
	  auto radiator = mcparticle->GetRadiator(rhistory);
	  
	  momenta[radiator] = mcparticle->GetHistory(rhistory)->m_AverageMomentum.Mag();
	} //for rhistory
	
	// FIXME: memory leak; looks like nothing else needs to be cleaned up here?;
	for(auto &hit: m_Hits) 
	  hit.m_Solutions[mcparticle].m_All.clear();
	
	// FIXME: 'true': use 3D direction seeds (MC truth); otherwise conical mirror case 
	// is problematic; do it better later;
	mcparticle->ProcessHits(m_Hits, true);
	
	//
	// By this moment every detected hit is evaluated with respect to this particle 
	// trajectory as originated from every radiator via every possible optical path; 
	//
	
	// Loop through all hits; 
	for(auto &hit: m_Hits) {
	  // Will never have means to know from which radiator a given photon originated, and 
	  // via which optical path it arrived to the sensor; just choose "the best" [radiator,irt] 
	  // combination, which this particular particle PID hypothesis can find; the criterion 
	  // is "closest in theta and closest in timing" as evaluated via a chi^2 estimate with ndf=2, 
	  // or just "closest in theta" in case of ndf=1;
	  hit.m_Solutions[mcparticle].m_Best = 0;
	  double thp0 = 0.0, hsigma0 = 0.0, ccdfmax = 0.0;//, hsigma = m_Radiator->GetSmearing();
	  
	  for(auto &tag: hit.m_Solutions[mcparticle].m_All) {
	    auto radiator = tag.first.first;
	    auto &solution = tag.second;
	    
	    // FIXME: handle this correctly;
	    //if (radiator != m_Radiator) continue;
	    if (m_Radiators.find(radiator) == m_Radiators.end()) continue;
	    
	    //double hsigma = radiator->GetSmearing();
	    double hsigma = (calibration || !m_UseAutomaticCalibration) ? 
	      radiator->GetSmearing() : csigma;
	    
	    double pp = momenta[radiator];
	    
	    for(unsigned ih=0; ih<hdim; ih++) {
	      double m = m_Hypotheses[ih]->Mass();
	      
	      // FIXME: exception;
	      double thp = acos(sqrt(pp*pp + m*m)/(radiator->m_AverageRefractiveIndex*pp));
	      double thdiff = solution.GetTheta() - thp;
	      double tmdiff = solution.m_Time - hit.GetAverageDetectionTime();
	      
	      unsigned ndf = 1;
	      double chi2 = pow(thdiff, 2)/pow(hsigma, 2);
	      if (m_UseTimingInChiSquare && m_SinglePhotonTimingResolution) {
		ndf++;
		chi2 += pow(tmdiff, 2)/pow(m_SinglePhotonTimingResolution, 2);
	      } //if
	      double ccdf = ROOT::Math::chisquared_cdf_c(chi2, ndf);
	      
	      if (ih == hypo) {
		if (!hit.m_Solutions[mcparticle].m_Best || ccdf > ccdfmax) {
		  hit.m_Solutions[mcparticle].m_Best = &solution;
		  // FIXME: do it better later;
		  thp0 = thp;
		  hsigma0 = hsigma;
		  ccdfmax = ccdf;
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
	    auto *best = hit.m_Solutions[mcparticle].m_Best;
	    
	    if (best) {
	      unsigned ndf = 1;
	      double thdiff = best->GetTheta() - thp0;
	      double chi2 = pow(thdiff, 2)/pow(hsigma0, 2);
	      double tmdiff = best->m_Time - hit.GetAverageDetectionTime();
	      if (m_UseTimingInChiSquare && m_SinglePhotonTimingResolution) {
		ndf++;
		chi2 += pow(tmdiff, 2)/pow(m_SinglePhotonTimingResolution, 2);
	      } //if
	      double ccdf = ROOT::Math::chisquared_cdf_c(chi2, ndf);
	      
	      // FIXME: should be radiator-specific;
	      if (ccdf > m_SingleHitCCDFcut) {
		hit.m_Selected.insert(std::make_pair(mcparticle, chi2));
		
		// Sure of interest is only to see a distribution for the right hypothesis; 
		if (mcparticle->GetPDG() == hyparticle->PdgCode()) {
		  if (itr && !calibration) m_hthph->Fill(1000*thdiff);
		  if (itr &&  calibration) m_hcalib->Fill(1000*thdiff);
		  if (itr && !calibration) m_hdtph->Fill(1000*tmdiff);
		  
		  if (itr && !calibration) m_hccdfph->Fill(ccdf);
		} //if
	      } //if
	    } //if
	  }
	} //for hits
	
	id /= m_Event->ChargedParticles().size();
      } //for mcparticle
      
      // Clean up background candidate list; obviously if at least one particle selected this 
      // hit for a hypothesis considered in this iteration, the hit is useful;
      for(auto &hit: m_Hits) 
	if (hit.m_Selected.size())
	  hit.m_BackgroundCandidate = false;
      
      // Now either resolve ambiguous assignments, based on the hit-to-track chi^2 match for this 
      // PID hypothesis, or eliminate these hits everywhere; FIXME: any better idea?;
      {
	for(auto &hit: m_Hits) 
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
	//#else
	//for(auto &hit: hits) 
	//	if (hit.m_Selected.size() != 1)
	//  hit.m_Selected.clear();
      }
      
      // And eventually calculate the overall event chi^2;
      {
	unsigned nbg = 0;
	for(auto &hit: m_Hits) 
	  if (hit.m_BackgroundCandidate) nbg++;
	
	unsigned ndfev = 0, idx = 0;
	double chi2ev = 0;
	for(auto mcparticle: m_Event->ChargedParticles()) {
	  unsigned npe = 0;
	  double chi2tr = 0.0;
	  
	  for(auto &hit: m_Hits) 
	    for(auto selected: hit.m_Selected)
	      if (selected.first == mcparticle) {
		npe++;
		double chi2ph = selected.second;
		chi2tr += chi2ph;
		
		if (itr) mcparticle->AddHit(&hit);//m_Hits.push_back(&hit);
		
		// FIXME: there is only one entry selected here?;
		break;
	      } //for..if
	  
	  unsigned ndftr = m_UseTimingInChiSquare && m_SinglePhotonTimingResolution ? 2*npe : 1*npe;
#ifdef _EXPECTED_TRUE_PHOTON_COUNT_
	  double texp = _EXPECTED_TRUE_PHOTON_COUNT_;
	  chi2tr += npe ? 2*(texp - npe + npe*log(npe/texp)) : 0.0;
	  ndftr++;
#endif
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
    } //for ic
  } //for itr
  
  if (calibration) {
  } else {
    // Now check how the PID procedure performed;
    // Sure of interest is only to see a flat CCDF distribution for the right combination 
    // of event-level hypotheses; 
    m_hccdfev->Fill(ccdfevsave[icbest]);
    
    for(unsigned ip=0; ip<m_Event->ChargedParticles().size(); ip++) {
      m_hccdftr->Fill(ccdftrsave[icbest][ip]);
      m_hnpetr ->Fill(npetrsave [icbest][ip]);
    } //for ip
    
    {
      // A running variable, to simplify hypothesis calculation;
      unsigned id = icbest;
      
      for(auto mcparticle: m_Event->ChargedParticles()) {
	unsigned hypo = id%hdim;
	auto rcparticle = m_Hypotheses[hypo];
	
	if (abs(mcparticle->GetPDG()) == abs(rcparticle->PdgCode())) {
	  m_hmatch->Fill(0.5);
	} else {
	  m_hmatch->Fill(1.5);
	  if (m_VerboseMode) printf("Failure!\n");
	} //if	  

	// Make it the same sign as for the MC particle (assume tracker can 
	// determine the charge sign);
	int sign = mcparticle->GetPDG() < 0 ? -1 : 1;
	mcparticle->SetRecoPdgCode(sign*abs(rcparticle->PdgCode()));
	
	id /= m_Event->ChargedParticles().size();
      } //for ip
    }
  }
  
  if (m_VerboseMode && !(ev%100)) printf("Event %5d ...\n", ev);

  return m_Event;
} // ReconstructionFactory::GetEvent()

// -------------------------------------------------------------------------------------
