
// Hits with chi^2 built in a 2D space (theta mismatch and timing mismatch) will 
// be accepted as "good" ones if the respective CCDF is more than this number;
#define _SINGLE_HIT_CCDF_CUT_DEFAULT_              (0.001)

// Especially this one is problematic since it should account for the size of radial bands;
//#define _EXPECTED_BG_PHOTON_COUNT_         ( 0.5)

#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TDatabasePDG.h>
#include <Math/ProbFunc.h>

#include "CherenkovDetectorCollection.h"
#include "CherenkovEvent.h"
#include "ReconstructionFactory.h"

#ifdef JSON_IMPORT_EXPORT
using json = nlohmann::json;
#endif

namespace IRT2 {

// -------------------------------------------------------------------------------------

ReconstructionFactory::ReconstructionFactory(const char *dfname, const char *cfname, 
					     const char *dname):
  GeantImport(dfname, cfname, dname),
  m_VerboseMode(true), 
  m_UseTimingInChiSquare(true),
  m_SingleHitCCDFcut(_SINGLE_HIT_CCDF_CUT_DEFAULT_),
  m_ResolveHitOwnership(true),
  m_UsePoissonTermInChiSquare(true),
  //m_ExperimentalMode(false), 
  // Help conical mirrors a bit;
  m_UseMcTruthPhotonDirectionSeed(true),
  m_Plots(0),
  // Require at least one associated hit per default;
  m_HitCountCutoff(1),
  m_ProcessedEventCount(0),
  m_CombinedPlotVisualizationEnabled(false),
  m_wtopx(0),
  m_wtopy(0),
  m_wx(0),
  m_wy(0)
{
} // ReconstructionFactory::ReconstructionFactory() 
// -------------------------------------------------------------------------------------

// FIXME: do it better later;
ReconstructionFactory::ReconstructionFactory(CherenkovDetectorCollection *geometry,
					     CherenkovDetector *cdet, CherenkovEvent *event):
  GeantImport(geometry, cdet, event),
  m_VerboseMode(true), 
  m_UseTimingInChiSquare(true),
  m_SingleHitCCDFcut(_SINGLE_HIT_CCDF_CUT_DEFAULT_),
  m_ResolveHitOwnership(true),
  m_UsePoissonTermInChiSquare(true),
  //m_ExperimentalMode(false), 
  // Help conical mirrors a bit;
  m_UseMcTruthPhotonDirectionSeed(true),
  m_Plots(0),
  // Require at least one associated hit per default;
  m_HitCountCutoff(1),
  m_ProcessedEventCount(0)
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
  
#ifdef JSON_IMPORT_EXPORT
void ReconstructionFactory::JsonParser(nlohmann::json jconfig)
{
  // Timing information usage in a chi^2 ansatz;
  if (jconfig.find("UseTimingInChiSquare") != jconfig.end() &&
      !strcmp(jconfig["UseTimingInChiSquare"].template get<std::string>().c_str(), "no"))
    IgnoreTimingInChiSquare();

  // Poisson term usage in a chi^2 ansatz;
  if (jconfig.find("UsePoissonTermInChiSquare") != jconfig.end() &&
      !strcmp(jconfig["UsePoissonTermInChiSquare"].template get<std::string>().c_str(), "no"))
    IgnorePoissonTermInChiSquare();

  // Outlier hit selection;
  if (jconfig.find("SingleHitCCDFcut") != jconfig.end())
    SetSingleHitCCDFcut(jconfig["SingleHitCCDFcut"].template get<double>());

  // Optional removal of hits which seem to be "shared" between >1 track;
  if (jconfig.find("RemoveAmbiguousHits") != jconfig.end() &&
      !strcmp(jconfig["RemoveAmbiguousHits"].template get<std::string>().c_str(), "yes"))
    RemoveAmbiguousHits();

  // Should be close enough to the real one; this only affects the calibration stage;
  if (jconfig.find("DefaultSinglePhotonThetaResolution") != jconfig.end())
    SetDefaultSinglePhotonThetaResolution(
        jconfig["DefaultSinglePhotonThetaResolution"].template get<double>());

  // Sensor active area will be pixellated NxN in digitization; '32': HRPPD-like "sensors";
  if (jconfig.find("SensorActiveAreaPixellation") != jconfig.end())
    SetSensorActiveAreaPixellation(
        jconfig["SensorActiveAreaPixellation"].template get<int>());

  // Single photon timing resolution; default: 0.050 (50ps);
  if (jconfig.find("SinglePhotonTimingResolution") != jconfig.end())
    SetSinglePhotonTimingResolution(
        jconfig["SinglePhotonTimingResolution"].template get<double>());

  // PID hypotheses to consider;
  if (jconfig.find("IdentifiedParticles") != jconfig.end()) {
    const auto& pconfig = jconfig["IdentifiedParticles"];

    for (auto& pdg : pconfig)
      AddHypothesis(pdg.template get<std::string>().c_str());
  } //if

  // May want to cheat a bit (feed IRT with true photon direction vectors);
  if (jconfig.find("UseMcTruthPhotonDirectionSeed") != jconfig.end() &&
      !strcmp(jconfig["UseMcTruthPhotonDirectionSeed"].template get<std::string>().c_str(), "no"))
    IgnoreMcTruthPhotonDirectionSeed();

  // Require at least that many associated hits; populate 0-th bin of a PID match
  // histogram otherwise; default: 1;
  if (jconfig.find("MinHitCountCutoff") != jconfig.end())
    SetHitCountCutoff(jconfig["MinHitCountCutoff"].template get<int>());

  // FIXME: this field should be mandatory (add a try-catch or such);
  if (jconfig.find("Calibration") != jconfig.end()) {
    std::ifstream fcalib(jconfig["Calibration"].template get<std::string>().c_str());
    if (fcalib.is_open()) {
      auto jcalib = json::parse(fcalib);

      if (jcalib.find("Radiators") != jcalib.end()) {
        const auto& rconfig = jcalib["Radiators"];

        for (auto [name, radiator] : GetMyRICH()->Radiators()) {
          // There should be an entry in JSON file; skip otherwise;
          if (rconfig.find(name.Data()) == rconfig.end())
            continue;
          const auto& rrconfig = rconfig[name.Data()];

          // Prefer to initialize in a separate loop; clear() is not really needed (?);
          radiator->m_Calibrations.clear();
          for (unsigned iq = 0; iq < _THETA_BIN_COUNT_; iq++)
            radiator->m_Calibrations.push_back(CherenkovRadiatorCalibration());

          if (rrconfig.find("theta-bins") != rrconfig.end()) {
            const auto& tconfig = rrconfig["theta-bins"];
            for (unsigned iq = 0; iq < _THETA_BIN_COUNT_; iq++) {
              TString bin;
              bin.Form("%02d", iq);

              if (tconfig.find(bin.Data()) != tconfig.end()) {
                const auto& tarray = tconfig[bin.Data()];

                auto* calib = &radiator->m_Calibrations[iq];

                unsigned rnum = tarray.size() - 4;

                if (rnum == GetMyRICH()->Radiators().size()) {
                  calib->m_Stat        = atoi(tarray[0].template get<std::string>().c_str());
                  calib->m_AverageZvtx = atof(tarray[1].template get<std::string>().c_str());
                  // Convert back to [rad];
                  calib->m_Coffset = atof(tarray[2].template get<std::string>().c_str()) / 1000.;
                  calib->m_Csigma  = atof(tarray[3].template get<std::string>().c_str()) / 1000.;
                  for (unsigned ir = 0; ir < rnum; ir++)
                    calib->m_AverageRefractiveIndices.push_back(
                        atof(tarray[4 + ir].template get<std::string>().c_str()));
                } //if
              } //if
            } //for iq
          } //if
        } //for radiator
      } //if

      fcalib.close();
    } //if
  } //if

  if (jconfig.find("Radiators") != jconfig.end()) {
    const auto& rconfig = jconfig["Radiators"];

    for (auto [name, radiator] : GetMyRICH()->Radiators()) {
      // There should be an entry in JSON file; skip otherwise;
      if (rconfig.find(name.Data()) == rconfig.end())
        continue;
      const auto& rrconfig = rconfig[name.Data()];

      if (rrconfig.find("imaging") != rrconfig.end() &&
          !strcmp(rrconfig["imaging"].template get<std::string>().c_str(), "yes"))
        radiator->UseInRingImaging();

      if (rrconfig.find("evaluation-plots") != rrconfig.end()) {
        const auto& tag = rrconfig["evaluation-plots"];

        if (!strcmp(tag.template get<std::string>().c_str(), "store"))
          radiator->InitializePlots(TString(name.Data()[0]).Data());
        else if (!strcmp(tag.template get<std::string>().c_str(), "display")) {
          radiator->InitializePlots(TString(name.Data()[0]).Data());
          radiator->m_OutputPlotVisualizationEnabled = true;
        } //if

        if (rrconfig.find("evaluation-plots-geometry") != rrconfig.end()) {
          const auto& gconfig = rrconfig["evaluation-plots-geometry"];

          if (gconfig.size() == 4) {
            radiator->m_wtopx = gconfig[0].template get<int>();
            radiator->m_wtopy = gconfig[1].template get<int>();
            radiator->m_wx    = gconfig[2].template get<int>();
            radiator->m_wy    = gconfig[3].template get<int>();
          } //if
        } //if

        {
          auto plots = radiator->Plots();

          if (rrconfig.find("refractive-index-range") != rrconfig.end())
            plots->SetRefractiveIndexRange(
                rrconfig["refractive-index-range"][0].template get<double>(),
                rrconfig["refractive-index-range"][1].template get<double>());

          if (rrconfig.find("photon-vertex-range") != rrconfig.end())
            plots->SetPhotonVertexRange(rrconfig["photon-vertex-range"][0].template get<double>(),
                                        rrconfig["photon-vertex-range"][1].template get<double>());

          if (rrconfig.find("cherenkov-angle-range") != rrconfig.end())
            plots->SetCherenkovAngleRange(
                rrconfig["cherenkov-angle-range"][0].template get<double>(),
                rrconfig["cherenkov-angle-range"][1].template get<double>());
        }
      } //if
    } //for radiator
  }

  // Initialize combined PID QA plots;
  if (jconfig.find("CombinedEvaluationPlots") != jconfig.end()) {
    const auto& tag = jconfig["CombinedEvaluationPlots"];

    if (!strcmp(tag.template get<std::string>().c_str(), "store"))
      InitializePlots();
    else if (!strcmp(tag.template get<std::string>().c_str(), "display")) {
      InitializePlots();
      m_CombinedPlotVisualizationEnabled = true;
    } //if
  } //if
  if (jconfig.find("CombinedEvaluationPlotsGeometry") != jconfig.end()) {
    const auto& gconfig = jconfig["CombinedEvaluationPlotsGeometry"];

    if (gconfig.size() == 4) {
      m_wtopx = gconfig[0].template get<int>();
      m_wtopy = gconfig[1].template get<int>();
      m_wx    = gconfig[2].template get<int>();
      m_wy    = gconfig[3].template get<int>();
    } //if
  } //if
} // ReconstructionFactory::JsonParser()
#endif
  
// -------------------------------------------------------------------------------------
  
void ReconstructionFactory::JsonParser(const char *fname)
{
#ifdef JSON_IMPORT_EXPORT
  printf("@Q@ ReconstructionFactory::JsonParser() ...\n");
  
  std::ifstream fcfg(fname);
  if (!fcfg) return;
  
  JsonParser(json::parse(fcfg));
#endif
} // ReconstructionFactory::JsonParser()

// -------------------------------------------------------------------------------------

void ReconstructionFactory::ProcessHits(ChargedParticle *mcparticle, std::vector<DigitizedHit> &hits, bool use_seed)
{ 
  double theta = mcparticle->GetVertexMomentum().Theta();
  unsigned ibin = (unsigned)floor(theta / _THETA_BIN_WIDTH_);
	
  // Loop through all digitized hits of a given event; apply IRT on a pre-calculated 
  // [vertex,momentum] pair for each hit-to-radiator association; no sampling any 
  // longer (assume gaussian errors);
  //
  // FIXME: add orphan (a la DCR) photons;
  for(auto &hit: hits) {
    // Loop through all optical paths for this photosensor;
    for(auto irt: *hit.m_IRTs) {
      for(auto rhistory: mcparticle->GetRadiatorHistory()) {
	auto history  = mcparticle->GetHistory (rhistory);
	
	auto radiator = mcparticle->GetRadiator(rhistory);
	if (!radiator->UsedInRingImaging()) continue;
	
	auto tag = std::make_pair(radiator, irt);

	{
	  unsigned ir = 0;
	  auto *calib = &radiator->m_Calibrations[ibin];
	  
	  for(auto [name,rad] : GetMyRICH()->Radiators()) 
	    rad->SetReferenceRefractiveIndex(calib->m_AverageRefractiveIndices[ir++]);
	} 
	
	{
	  IRTSolution seed;

	  // Loop through all of the provided seeds until IRT converges (yes, assume 
	  // the solution is unique);
	  unsigned smax = use_seed ? hit.m_DirectionSeeds.size() : 1;
	  for(unsigned iq=0; iq<smax; iq++) {
	    if (use_seed) seed.SetSeed(hit.m_DirectionSeeds[iq]);

	    auto &solution = hit.m_Solutions[mcparticle].m_All[tag] = 
	      irt->Solve(history->m_EstimatedVertex,
			 // FIXME: give beam line as a parameter;
			 history->m_AverageParentMomentum.Unit(), hit.GetDetectionPosition(), 
			 TVector3(0,0,1), false, use_seed ? &seed : 0);
	    
	    solution.m_Time = solution.m_Length/300;

	    if (solution.m_Converged) break;
	  } //for iq
	  //solution.m_Time = radiator->m_AverageTime + solution.m_Length/300;
	}
      } //for rhistory
    } //for irt
  } //for hit
} // ReconstructionFactory::ProcessHits()

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
	ProcessHits(mcparticle, Hits(), m_UseMcTruthPhotonDirectionSeed);
	
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

	    //if (!_IsSelected(radiator)) continue;
	    if (!radiator->UsedInRingImaging()) continue;
	    
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
		  if (itr && !calibration && sptr->m_rbest->Plots())
		    sptr->m_rbest->Plots()->hthph()->Fill(1000*thdiff);
		  
		  if (itr &&  calibration) 		    
		    sptr->m_rbest->m_Calibrations[ibin].m_hcalib->Fill(1000*thdiff);
		  
		  if (itr && !calibration && Plots()) Plots()->hdtph()->Fill(1000*tmdiff);
		  
		  if (itr && !calibration && sptr->m_rbest->Plots())
		    sptr->m_rbest->Plots()->hccdfph()->Fill(ccdf);
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
#ifdef _EXPECTED_BG_PHOTON_COUNT_
	unsigned nbg = 0;
	for(auto &hit: Hits()) 
	  if (hit.m_BackgroundCandidate) nbg++;
#endif
	
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

	      //if (!_IsSelected(radiator)) continue;
	      if (!radiator->UsedInRingImaging()) continue;

	      double theta = mcparticle->GetRecoCherenkovAverageTheta(radiator);
	      if (theta) {
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
    if (Plots()) {
      Plots()->hccdfev()->Fill(ccdfevsave[icbest]);
    
      for(unsigned ip=0; ip<Event()->ChargedParticles().size(); ip++) 
	Plots()->hccdftr()->Fill(ccdftrsave[icbest][ip]);
    } //if
    
    {
      // A running variable, to simplify hypothesis calculation;
      unsigned id = icbest;
      
      for(auto mcparticle: Event()->ChargedParticles()) {
	unsigned hypo = id%hdim;
	auto rcparticle = m_Hypotheses[hypo];

	//if (mcparticle->m_HadronicInteractionOccured) continue;

	if (Plots()) {
	  unsigned nhits = mcparticle->GetRecoCherenkovHitCount();
	  
	  if (nhits < m_HitCountCutoff) {
	    Plots()->hmatch()->Fill(0.5);
	    if (BeVerbose()) printf("Only %2d hits attached (min required %2d)!\n",
				    nhits, m_HitCountCutoff);
	  } else if (abs(mcparticle->GetPDG()) == abs(rcparticle->PdgCode())) {
	    Plots()->hmatch()->Fill(1.5);
	  } else {
	    Plots()->hmatch()->Fill(2.5);
	    if (BeVerbose()) printf("PID Failure!\n");
	  } //if	  
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

int ReconstructionFactory::VerifyEventStructure( void )
{
  bool ret = 0;
  
  // FIXME: duplicate code;
  for(auto mcparticle: Event()->ChargedParticles()) {
    unsigned npe_per_track = 0;

    for(auto rhptr: mcparticle->GetRadiatorHistory()) {
      auto radiator = mcparticle->GetRadiator(rhptr);
      if (!radiator->UsedInRingImaging()) continue;
		  
      for(auto photon: mcparticle->GetHistory(rhptr)->Photons()) 
	if (photon->WasDetected()) 
	  npe_per_track++;
    } // for rhistory

    if (!npe_per_track) {
      mcparticle->m_GoodForReconstruction = false;

      // If at least one MC particle had no detected photons associated with it,
      // skip the event all together; FIXME: once the debugging phase is over,
      // just exclude particular particles; 
      ret = -1;
    } //if
  } //for mcparticle

  return ret;
} // ReconstructionFactory::VerifyEventStructure()

// -------------------------------------------------------------------------------------

CherenkovEvent *ReconstructionFactory::GetEvent(unsigned ev, bool calibration)
{
  // Prepair for next event;
  ClearBlackoutCells();
  ClearDigitizedHits();

  // Get it from the ROOT tree; otherwise assume it is an EICrecon mode where event structure
  // has been populated by the IrtInterface already;
  if (m_Tree) GetInputTreeEntry(ev);

  if (VerifyEventStructure()) return Event();
  //return 0;
  
  // Use undetected photons (HERE DO THIS ON TRACK PER TRACK BASIS) to extract the 
  // expected average emission point 3D location, time and parent particle 3D momentum;  
  /*if (!m_ExperimentalMode)*/ CalibratePhotonEmissionPoints();
  //return 0;

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
  /*if (!m_ExperimentalMode)*/ LaunchRingFinder(calibration);

  // Build plots if needed;
  if (!calibration)
    // FIXME: duplicate code;
    for(auto mcparticle: Event()->ChargedParticles()) {
      unsigned npe_per_track = 0, nhits_per_track = 0;

      for(auto rhptr: mcparticle->GetRadiatorHistory()) {
	auto radiator = mcparticle->GetRadiator(rhptr);
	if (!radiator->UsedInRingImaging()) continue;
	
	unsigned npe_per_radiator = 0, nhits_per_radiator = 0;
	
	nhits_per_radiator = mcparticle->GetRecoCherenkovPhotonCount(radiator);
	nhits_per_track += nhits_per_radiator;
	
	auto *plots = radiator->Plots();
	  
	for(auto photon: mcparticle->GetHistory(rhptr)->Photons()) 
	  if (photon->WasDetected()) {
	    npe_per_radiator++;

	    if (plots) {
	      plots->hwl()->Fill(1239.8/(photon->GetVertexMomentum().Mag()));
	      if (plots->hri())  plots->hri() ->Fill(photon->GetVertexRefractiveIndex() - 1.0);
	      if (plots->hvtx()) plots->hvtx()->Fill(photon->GetVertexPosition().Z());
	    } //if
	  } //for photon .. if
	
	if (plots) {
	  plots->hnpe()  ->Fill(npe_per_radiator);
	  
	  plots->hnhits()->Fill(nhits_per_radiator);
	  plots->hthtr() ->Fill(1000*mcparticle->GetRecoCherenkovAverageTheta(radiator));
	}
		
	npe_per_track += npe_per_radiator;
      } //for rhistory
            
      if (Plots()) {
	Plots()->hnhits()->Fill(nhits_per_track);
	Plots()->hnpe()  ->Fill(npe_per_track);
      } //if
    } //for mcparticle
  
  if (BeVerbose() && !(ev%100)) printf("Event %5d (%5d processed) ...\n", ev, m_ProcessedEventCount);

  m_ProcessedEventCount++;
  
  return Event();
} // ReconstructionFactory::GetEvent()
  
// -------------------------------------------------------------------------------------
  
void ReconstructionFactory::BareEventLoop(unsigned stat)
{
  unsigned evCounter = 0;

  // Event loop embedding ReconstructionFactory calls;
  while(GetNextEvent())
    if (stat && ++evCounter == stat)
      break;
} // ReconstructionFactory::BareEventLoop()

// -------------------------------------------------------------------------------------

ReconstructionFactoryPlots::ReconstructionFactoryPlots( void )
{
  m_hnpe    = new TH1D("npe",    "Detected MC photons per track",      60,   0.0,   60.0);
  m_hnhits  = new TH1D("nhits",  "Used digitized hits per track",      60,   0.0,   60.0);
  
  m_hdtph   = new TH1D("dtph",   "Timing offset (photons)",           100,  -500,    500);
  m_hmatch  = new TH1D("match",  "PID evaluation correctness",          3,     0,      3);
  
  m_hccdftr = new TH1D("ccdftr", "Track-level chi^2 CCDF",             50,   0.0,    1.0);
  m_hccdftr->SetMinimum(0);
  m_hccdfev = new TH1D("ccdfev", "Event-level chi^2 CCDF",             50,   0.0,    1.0);
  m_hccdfev->SetMinimum(0);
} // ReconstructionFactoryPlots::ReconstructionFactoryPlots()

// -------------------------------------------------------------------------------------

TCanvas *ReconstructionFactory::DisplayStandardPlots(const char *cname, int wtopx,
						     unsigned wtopy, unsigned wx, unsigned wy) const
{
  if (!Plots()) return 0;
	  
  auto cv = new TCanvas("cx", cname, wtopx, wtopy, wx, wy);
  cv->Divide(2, 3);
  
  cv->cd(1);                      Plots()->hnpe()   ->Draw();
  cv->cd(2);                      Plots()->hnhits() ->Draw();
  cv->cd(3);                      Plots()->hccdftr()->Draw();
  cv->cd(4);                      Plots()->hccdfev()->Draw();
  cv->cd(5);                      Plots()->hdtph()  ->Fit("gaus");
  cv->cd(6);                      Plots()->hmatch() ->Draw();

  return cv;
} // ReconstructionFactory::DisplayStandardPlots()

// -------------------------------------------------------------------------------------

} // namespace IRT2
