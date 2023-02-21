
// Kind of digitization; may want to comment out (or set very fine) for debugging;
#define _HRPPD_ACTIVE_AREA_PIXELLATION_DEFAULT_       (32)

// Timing digitization in [ns];
#define _SINGLE_PHOTON_TIMING_RESOLUTION_DEFAULT_  (0.050)

// Hits with chi^2 built in a 2D space (theta mismatch and timing mismatch) will 
// be accepted as "good" ones if the respective CCDF is more than this number;
#define _SINGLE_HIT_CCDF_CUT_DEFAULT_              (0.001)

// [rad] & [mm]; smearing portion, excluding emission point uncertainty 
// (so to first order only dn/dl and sensor plane resolution);
#define _THETA_GAUSSIAN_SMEARING_DEFAULT_         (0.0040)

// [mm]; FIXME: should use fixed.h value;
//#define _VESSEL_FRONT_SIDE_OFFSET_       (1185.5)

// Comment out if do not want to deal with the Poissonic terms in chi^2; FIXME: account 
// for yield drop at smaller momentum and yield gain when primary particle crosses aerogel 
// at a large angle;
//#define _EXPECTED_TRUE_PHOTON_COUNT_       (10.5)
// Especially this one is problematic since it should account for the size of radial bands;
//#define _EXPECTED_BG_PHOTON_COUNT_         ( 0.5)

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TDatabasePDG.h>
#include <Math/ProbFunc.h>

#include <CherenkovDetectorCollection.h>
#include <CherenkovEvent.h>
#include <ReconstructionFactory.h>

// -------------------------------------------------------------------------------------

ReconstructionFactory::ReconstructionFactory(const char *dfname, const char *cfname, 
					     const char *dname, const char *rname):
  m_Tree(0),
  m_Radiator(0),
  m_Event(0),
  m_RICH(0),
  m_CurrentEvent(0), 
  m_UseTimingInChiSquare(true),
  m_SingleHitCCDFcut(_SINGLE_HIT_CCDF_CUT_DEFAULT_),
  m_ResolveHitOwnership(true), 
  m_SinglePhotonTimingResolution(_SINGLE_PHOTON_TIMING_RESOLUTION_DEFAULT_),
  m_SensorActiveAreaPixellation(_HRPPD_ACTIVE_AREA_PIXELLATION_DEFAULT_)
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

  m_hthph   = new TH1D("thph",   "Cherenkov angle (SPE)",          100,  -30,     30);
  //hthtr   = new TH1D("thtr",   "Cherenkov angle (track)",        100,  -30,     30);
  m_hccdfph = new TH1D("ccdfph", "Single photon angle chi^2 CCDF",  50,    0,      1);
  m_hccdftr = new TH1D("ccdftr", "Track-level chi^2 CCDF",          50,    0,      1);
  m_hccdfev = new TH1D("ccdfev", "Event-level chi^2 CCDF",          50,    0,      1);
  m_hnpetr  = new TH1D("npetr",  "Photon count per track",          50,    0,     50);
  m_hdtph   = new TH1D("dtph",   "Cherenkov timing (SPE)",         100, -500,    500);
  m_hmatch  = new TH1D("match",  "PID evaluation correctness",       2,    0,      2);

  m_RICH = geometry->GetDetector(dname);
  m_Radiator = m_RICH->GetRadiator(rname);

  m_DatabasePDG = new TDatabasePDG();

  m_Radiator->SetGaussianSmearing(_THETA_GAUSSIAN_SMEARING_DEFAULT_);
} // ReconstructionFactory::ReconstructionFactory() 

// -------------------------------------------------------------------------------------

void ReconstructionFactory::PerformCalibration( void )
{
  unsigned nEvents = m_Tree->GetEntries();

  // First calculate average refractive index for all detected photons for every radiator;
  for(unsigned ev=0; ev<nEvents; ev++) {
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
	} //for photon
      } //for radiator
    } //for particle
  } //for ev
  
  for(auto rptr: m_RICH->Radiators()) {
    auto radiator = rptr.second;
    
    if (radiator->m_Stat) radiator->m_AverageRefractiveIndex /= radiator->m_Stat;
  } //for dt..radiator
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

CherenkovEvent *ReconstructionFactory::GetNextEvent( void )
{
  if (m_CurrentEvent < m_Tree->GetEntries()) 
    return GetEvent(m_CurrentEvent++);
  else
    return 0;
} // ReconstructionFactory::GetNextEvent()

// -------------------------------------------------------------------------------------

CherenkovEvent *ReconstructionFactory::GetEvent(unsigned ev)
{
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
	history->m_AverageTime /= stat;
	history->m_AverageVertex *= 1./stat;
	history->m_AverageMomentum *= 1./stat;
      } //if
    } //for radiator
  } //for particle
  
  //std::vector<DigitizedHit> hits;
  
  // First mix all photons from all particles and "digitize" them; FIXME: add stray background;
  for(auto particle: m_Event->ChargedParticles())
    for(auto rhistory: particle->GetRadiatorHistory()) 
      for(auto photon: particle->GetHistory(rhistory)->Photons()) {
	if (!photon->WasDetected() /*|| photon->m_ReflectionPoints.size() != 1*/) continue;
	
	auto pd = photon->GetPhotonDetector();
	
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
	    //printf("%3d %3d\n", hit.m_iX, hit.m_iY);
	    double dx = pitch*(hit.m_iX + 0.5), dy = pitch*(hit.m_iY + 0.5);
	    phx = sensor->GetSpacePoint(x0 + dx, y0 + dy);
	    
	    for(auto &ptr: m_Hits)
	      if (ptr.m_PhotonDetector == pd && ptr.m_Copy == hit.m_Copy && 
		  ptr.m_iX == hit.m_iX && ptr.m_iY == hit.m_iY) {
		duplicate = &ptr;
		break;
	      } //for ptr .. if
	  } //if
	  (duplicate ? duplicate : &hit)->m_DirectionSeeds.push_back(photon->GetVertexMomentum().Unit());
	  // Convert to [ps];
	  double timing = photon->GetDetectionTime() + 
	    (m_SinglePhotonTimingResolution ? m_rndm.Gaus(0.0, m_SinglePhotonTimingResolution) : 0.0);
	  (duplicate ? duplicate : &hit)->m_DetectionTimes.push_back(timing);
	  
	  if (!duplicate) {
	    // For duplicate hits this setting will be identical;
	    hit.m_DetectionPosition = phx;
	    
	    m_Hits.push_back(hit);
	  } //if
	}
      } //for particle..photon
  
  //
  //   There are no optical photons beyond this point; their information was merged into 
  // the DigitizedHit array; the only cheating left are direction seeds, but this is rather a
  // procedural issue than using MC truth information illegally; and t0 resolution is supposed to 
  // be ideal (but it will anyway be much better than anticipated HRPPD single photon TTS);
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
	
	momenta[radiator] = 
	  mcparticle->GetHistory(rhistory)->m_AverageMomentum.Mag();
      } 
      
      // FIXME: 'true': use 3D direction seeds (MC truth); otherwise conical mirror case 
      // is problematic; do it better later;
      mcparticle->ProcessHits(m_Hits, true);
      
      //
      // By this moment every detected hit is evaluated with respect to this particle 
      // trajectory as originated from every radiator via every possible optical path; 
      //
      
      // Loop through all hits; 
      for(auto &hit: m_Hits) {
	// Will never have means to know from which radiator 
	// a given photon originated, and via which optical path it arrived to the sensor;
	// just choose "the best" [radiator,irt] combination, which this particular 
	// particle PID hypothesis can find; the criterion is "closest in theta and closest 
	// in timing" as evaluated via a chi^2 estimate with ndf=2;
	//IRTSolution *best = 0;
	hit.m_Best = 0;
	double thp0 = 0.0, /*tsigma = _SINGLE_PHOTON_TIMING_RESOLUTION_,*/ ccdfmax = 0.0;
	double hsigma = m_Radiator->GetSmearing();
	
	for(auto &tag: hit.m_Solutions) {
	  auto radiator = tag.first.first;
	  auto &solution = tag.second;
	  
	  // FIXME: handle this correctly;
	  if (radiator != m_Radiator) continue;
	  
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
	      if (!hit.m_Best || ccdf > ccdfmax) {
		hit.m_Best = &solution;
		// FIXME: do it better later;
		thp0 = thp;
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

	if (hit.m_Best) {
	  unsigned ndf = 1;
	  double thdiff = hit.m_Best->GetTheta() - thp0;
	  double chi2 = pow(thdiff, 2)/pow(hsigma, 2);
	  double tmdiff = hit.m_Best->m_Time - hit.GetAverageDetectionTime();
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
	      if (itr) m_hthph->Fill(1000*thdiff);
	      if (itr) m_hdtph->Fill(1000*tmdiff);
	      
	      if (itr) m_hccdfph->Fill(ccdf);
	    } //if
	  } //if
	} //if
      } //for hits
      
      id /= m_Event->ChargedParticles().size();

      // FIXME: memory leak; looks like nothing else needs to be cleaned up here?;
      for(auto &hit: m_Hits) 
	hit.m_Solutions.clear();
    } //for mcparticle
    
      // Clean up background candidate list; obviously if at least one particle selected this 
      // hit for a hypothesis considered in this iteration, the hit is useful;
    for(auto &hit: m_Hits) 
      if (hit.m_Selected.size())
	hit.m_BackgroundCandidate = false;
    
    // Now either resolve ambiguous assignments, based on the hit-to-track chi^2 match for this 
    // PID hypothesis, or eliminate these hits everywhere; FIXME: any better idea?;
    {
      //#ifdef _RESOLVE_HIT_OWNERSHIP_ 
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
      //#endif
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
	      
	      if (itr) mcparticle->m_Hits.push_back(&hit);

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
  
    // Now check how the PID procedure performed;
  {
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

	mcparticle->SetRecoPdgCode(abs(rcparticle->PdgCode()));
	
	id /= m_Event->ChargedParticles().size();
      } //for ip
    }
  }
  
  if (m_VerboseMode && !(ev%100)) printf("Event %5d ...\n", ev);

  return m_Event;
} // ReconstructionFactory::GetEvent()

// -------------------------------------------------------------------------------------
