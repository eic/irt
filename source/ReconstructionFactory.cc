
// Hits with chi^2 built in a 2D space (theta mismatch and timing mismatch) will 
// be accepted as "good" ones if the respective CCDF is more than this number;
#define _SINGLE_HIT_CCDF_CUT_DEFAULT_              (0.001)

// Especially this one is problematic since it should account for the size of radial bands;
//#define _EXPECTED_BG_PHOTON_COUNT_         ( 0.5)

#include <algorithm>
#include <cmath>
#include <vector>
#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TDatabasePDG.h>
#include <Math/ProbFunc.h>

#include <TApplication.h>
#include <TCanvas.h>

#include "CherenkovDetectorCollection.h"
#include "CherenkovEvent.h"
#include "ReconstructionFactory.h"

#ifdef JSON_IMPORT_EXPORT
using json = nlohmann::json;
#endif

// FIXME: move to a different place;
#define _MAGIC_CFF_ (1239.8)

namespace IRT2 {

// -------------------------------------------------------------------------------------

ReconstructionFactory::ReconstructionFactory(const char *dfname, const char *cfname, 
					     const char *dname):
  GeantImport(dfname, cfname, dname),
  m_OutputFile(0),
  m_VerboseMode(true), 
  m_UseTimingInChiSquare(true),
  m_SingleHitCCDFcut(_SINGLE_HIT_CCDF_CUT_DEFAULT_),
  m_ResolveHitOwnership(true),
  m_UsePoissonTermInChiSquare(true),
  m_UseMixtureLikelihood(false),
  m_BackgroundFraction(0.1),
  m_BackgroundWindow(0.02),
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
  m_wy(0), 
  m_OutputEventTree(0),
  m_OutputEventBranch(0)
{
} // ReconstructionFactory::ReconstructionFactory()
  
// -------------------------------------------------------------------------------------

// FIXME: do it better later;
ReconstructionFactory::ReconstructionFactory(CherenkovDetectorCollection *geometry,
					     CherenkovDetector *cdet, CherenkovEvent *event,
					     const char *json_config_file_name):
  GeantImport(geometry, cdet, event),
  m_OutputFile(0),
  m_VerboseMode(true), 
  m_UseTimingInChiSquare(true),
  m_SingleHitCCDFcut(_SINGLE_HIT_CCDF_CUT_DEFAULT_),
  m_ResolveHitOwnership(true),
  m_UsePoissonTermInChiSquare(true),
  m_UseMixtureLikelihood(false),
  m_BackgroundFraction(0.1),
  m_BackgroundWindow(0.02),
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
  m_wy(0), 
  m_OutputEventTree(0),
  m_OutputEventBranch(0)
{
  if (json_config_file_name) JsonParser(json_config_file_name);
} // ReconstructionFactory::ReconstructionFactory()
  
// -------------------------------------------------------------------------------------

ReconstructionFactory::~ReconstructionFactory()
{
  if (m_OutputFile) {
    m_OutputFile->cd();
    
    if (m_OutputEventTree) m_OutputEventTree->Write();
    
    GetIrtGeometry()->Write();
  } //if
  
  // Avoid calling this stuff from a dummy IrtInterface instantiation upon eicrecon startup;
  if (GetProcessedEventCount()) {
    int argc      = 1;
    char* argv[1] = {(char*)""};
    // FIXME: the way this stuff is presently coded, if at least one panel was requested in a "display"
    // mode, all panels requested as a "store" will also be displayed;
    bool display  = m_CombinedPlotVisualizationEnabled;
    for (auto [name, rad] : GetMyRICH()->Radiators())
      if (rad->UsedInRingImaging() && rad->m_OutputPlotVisualizationEnabled)
	display = true;
    
    // FIXME: well, if at least one is "display", all "store" will be shown as well;
    // NB: do not want to invoke it twice (and 'delete' crashes, for some reason);
    auto app = display && !gApplication ? new TApplication("", &argc, argv) : 0;
    
    std::vector<TCanvas*> canvases;

    auto cv = DisplayStandardPlots("Track / event level plots", m_wtopx, m_wtopy, m_wx, m_wy);
    if (cv)
      canvases.push_back(cv);
    
    for (auto [name, rad] : GetMyRICH()->Radiators())
      if (rad->UsedInRingImaging() && rad->Plots()) {
	TString cname, wname;
	// FIXME: won't work for Acrylic and Aerogel together;
	cname.Form("c%c", std::tolower(name.Data()[0]));
	wname.Form("%s radiator", name.Data());

	auto cv = rad->DisplayStandardPlots(cname.Data(), wname.Data(),
					    // FIXME: may want to improve the API here;
					    rad->m_wtopx, rad->m_wtopy, rad->m_wx, rad->m_wy);
	if (cv)
	  canvases.push_back(cv);
      } //for rad..if
    
    // 'true': do not call exit() in the end;
    if (app && canvases.size()) 
      app->Run(true);

    for (auto cv : canvases) {
      if (m_OutputFile)
	cv->Write();
      
      delete cv;
    } //for cv
  } //if
  
  if (m_Plots) delete m_Plots;
  
  // FIXME: crashes;
  if (gApplication) {
    //delete gApplication;
    //gApplication = nullptr;
  } //if
    
  if (m_OutputFile)
    m_OutputFile->Close();
} // ReconstructionFactory::~ReconstructionFactory()

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
  if (jconfig.find("OutputRootFile") != jconfig.end()) {
    std::string fname = jconfig["OutputRootFile"].template get<std::string>().c_str();

    auto len = strlen(fname.c_str());
    if (len > 5 && !strcmp(fname.c_str() + len - 5, ".root"))
      //if (strcmp(fname.c_str(), "none"))
      m_OutputFile = new TFile(fname.c_str(), "RECREATE");
  } //if
    
  if (jconfig.find("WriteOutputTree") != jconfig.end() &&
      strcmp(jconfig["WriteOutputTree"].template get<std::string>().c_str(), "no")) {
    m_OutputEventTree   = new TTree("t", "IRT2 output tree");
    
    // This does not work after IRT2 namespace was introduced;
    //m_OutputEventBranch = m_OutputEventTree->Branch("e", "IRT2::CherenkovEvent", &m_Event);//, 16000, 2);
    //m_OutputEventBranch = m_EventTree->Branch("e", "IRT2::CherenkovEvent", m_EventPtr, 16000, 2);
    //m_OutputEventBranch = m_EventTree->Branch("e", "IRT2::CherenkovEvent", m_EventPtr, 16000, 2);
  } //if
  
  // Timing information usage in a chi^2 ansatz;
  if (jconfig.find("UseTimingInChiSquare") != jconfig.end() &&
      !strcmp(jconfig["UseTimingInChiSquare"].template get<std::string>().c_str(), "no"))
    IgnoreTimingInChiSquare();

  // Poisson term usage in a chi^2 ansatz;
  if (jconfig.find("UsePoissonTermInChiSquare") != jconfig.end() &&
      !strcmp(jconfig["UsePoissonTermInChiSquare"].template get<std::string>().c_str(), "no"))
    IgnorePoissonTermInChiSquare();

  // Outlier hit selection;
  if (jconfig.find("Estimator") != jconfig.end()) {
    TString estimator = jconfig["Estimator"].template get<std::string>();
    estimator.ToLower();
    m_UseMixtureLikelihood = (estimator == "ml" || estimator == "mixture" ||
                              estimator == "likelihood" || estimator == "maximum likelihood");
    if (BeVerbose()) printf("ReconstructionFactory: estimator = %s\n", estimator.Data());
  } //if

  if (jconfig.find("BackgroundFraction") != jconfig.end())
    m_BackgroundFraction = jconfig["BackgroundFraction"].template get<double>();

  if (jconfig.find("BackgroundWindow") != jconfig.end())
    // Angular window [rad] over which the background component of the mixture is flat;
    m_BackgroundWindow = jconfig["BackgroundWindow"].template get<double>();

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

        //{
	auto plots = radiator->Plots();

	if (plots) {
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
        } //if
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
  
  {
    // FIXME: for now assume a single photo detector type; cannot easily store this pointer;
    auto pd = GetMyRICH()->m_PhotonDetectors[0];

    if (jconfig.find("Photosensor") != jconfig.end()) {
      auto& jpref = jconfig["Photosensor"];

      double qe_rescaling_factor = 1.0;
      // An artificial rescaling factor may be provided;
      if (jpref.find("quantum-efficiency-rescaling-factor") != jpref.end())
        qe_rescaling_factor = jpref["quantum-efficiency-rescaling-factor"].template get<double>();

      if (jpref.find("quantum-efficiency") != jpref.end()) {
        auto& qeref = jpref["quantum-efficiency"];

        const int qeEntries = qeref.size();
        std::unique_ptr<double[]> WL(new double[qeEntries]);
        std::unique_ptr<double[]> QE(new double[qeEntries]);

        unsigned counter = 0;
        for (json::iterator it = qeref.begin(); it != qeref.end(); ++it) {
          std::string wlstr(it.key().c_str());
          // FIXME: assumes a 3-digit integer value; do it better later;
          wlstr[3] = 0;

          WL[counter]   = atoi(wlstr.c_str());
          QE[counter++] = it.value().template get<double>();
        } //it

        double qemax = 0.0;
        std::vector<double> qePhotonEnergy(qeEntries);
        std::vector<double> qeData(qeEntries);
        for (int iq = 0; iq < qeEntries; iq++) {
          qePhotonEnergy[iq] = _MAGIC_CFF_ / (WL[qeEntries - iq - 1] + 0.0);
          qeData[iq]         = QE[qeEntries - iq - 1] * qe_rescaling_factor;

          if (qeData[iq] > qemax)
            qemax = qeData[iq];
        } //for iq

        pd->SetQE(
            _MAGIC_CFF_ / WL[qeEntries - 1], _MAGIC_CFF_ / WL[0],
            // NB: last argument: want a built-in selection of unused photons, which follow the QE(lambda);
            // see CherenkovSteppingAction::UserSteppingAction() for a usage case;
            new DataInterpolation(qePhotonEnergy.data(), qeData.data(), qeEntries),
            qemax ? 1.0 / qemax : 1.0);
        // FIXME: 100 hardcoded;
        pd->GetQE()->CreateLookupTable(100);
      } //if
    } //if
  }
} // ReconstructionFactory::JsonParser()
#endif
  
// -------------------------------------------------------------------------------------
  
void ReconstructionFactory::JsonParser(const char *fname)
{
#ifdef JSON_IMPORT_EXPORT
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
  if (ibin >= _THETA_BIN_COUNT_) {
    if (BeVerbose()) printf("ProcessHits: theta=%.4f -> ibin=%u out of range [0,%u); skipping particle\n",
                            theta, ibin, _THETA_BIN_COUNT_);
    return;
  }
	
  // Resolve the radiator history of this particle once; TRef lookups, dynamic_casts and
  // the UsedInRingImaging() selection do not depend on hits or optical paths;
  //
  struct MyPath {CherenkovRadiator *m_Radiator; RadiatorHistory *m_History;};
  std::vector<MyPath> paths;

  for(auto rhistory: mcparticle->GetRadiatorHistory()) {
    auto radiator = mcparticle->GetRadiator(rhistory);
    if (!radiator->UsedInRingImaging()) continue;
    paths.push_back(MyPath{radiator, mcparticle->GetHistory(rhistory)});
  } //for rhistory

  if (!paths.size()) return;

  // Loop through all radiators crossed by this particle; apply IRT on a pre-calculated 
  // [vertex,momentum] pair for each hit-to-radiator association; no sampling any 
  // longer (assume gaussian errors);
  //
  // NB: reference refractive indices depend on [theta-bin,radiator] only, hence they are 
  // re-tuned once per radiator rather than once per [hit,optical-path] pair; the value in 
  // effect at the moment of any given Solve() call is unchanged by this reordering;
  //
  // FIXME: add orphan (a la DCR) photons;
  for(auto &path: paths) {
    auto radiator = path.m_Radiator;

    {
      unsigned ir = 0;
      auto *calib = &radiator->m_Calibrations[ibin];
      if (calib->m_AverageRefractiveIndices.size() < GetMyRICH()->Radiators().size()) {
        if (BeVerbose()) printf("ProcessHits: no calibration for radiator '%s' theta-bin %u "
                            "(have %zu refractive indices, need %zu); skipping radiator\n",
                            radiator->GetAlternativeMaterialName(), ibin,
                            calib->m_AverageRefractiveIndices.size(),
                            GetMyRICH()->Radiators().size());
        continue;
      }

#if 1
      for(auto [name,rad] : GetMyRICH()->Radiators()) 
        mcparticle->SetReferenceRefractiveIndex(rad, calib->m_AverageRefractiveIndices[ir++]);
#else
      for(auto [name,rad] : GetMyRICH()->Radiators()) 
        rad->SetReferenceRefractiveIndex(calib->m_AverageRefractiveIndices[ir++]);
#endif
    } 

    // Loop through all digitized hits of a given event;
    for(auto &hit: hits) {
      // Loop through all optical paths for this photosensor;
      for(auto irt: *hit.m_IRTs) {
        auto tag = std::make_pair(radiator, irt);

        {
          IRTSolution seed;

          // Loop through all of the provided seeds until IRT converges (yes, assume 
          // the solution is unique);
          unsigned smax = use_seed ? hit.m_DirectionSeeds.size() : 1;
          for(unsigned iq=0; iq<smax; iq++) {
            if (use_seed) seed.SetSeed(hit.m_DirectionSeeds[iq]);

            auto &solution = hit.m_Solutions[mcparticle].m_All[tag] = 
              irt->Solve(mcparticle, path.m_History->m_EstimatedVertex,
                         // FIXME: give beam line as a parameter;
                         path.m_History->m_AverageParentMomentum.Unit(), hit.GetDetectionPosition(), 
                         TVector3(0,0,1), false, use_seed ? &seed : 0);
            
            solution.m_Time = solution.m_Length/300;

            if (solution.m_Converged) break;
          } //for iq
          //solution.m_Time = radiator->m_AverageTime + solution.m_Length/300;
        }
      } //for irt
    } //for hit
  } //for path
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
  std::vector<double> lnLsave(cdim, 0.0);
  
  // Now loop through all [particle,hypothesis] combinations and evaluate their chi^2;
  for(unsigned itr=0; itr<2; itr++) {
    // IRT solutions do not depend on the PID hypothesis combination, so calculate them 
    // once per iteration and reuse them for every [particle,hypothesis] combination;
    bool solutionsValid = false;

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
	
	if (!solutionsValid) {
	// IRT solutions for this particle do not depend on the PID hypothesis combination; 
	// they are calculated once per iteration (see above) and reused below;
	// FIXME: memory leak; looks like nothing else needs to be cleaned up here?;
	for(auto &hit: Hits())
	  hit.m_Solutions[mcparticle].m_All.clear();

	// FIXME: 'true': use 3D direction seeds (MC truth); otherwise conical mirror case
	// is problematic; do it better later;
	ProcessHits(mcparticle, Hits(), m_UseMcTruthPhotonDirectionSeed);
} //if
	
	if (!m_UseMixtureLikelihood) {
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
	      //double thp = acos(sqrt(pp*pp + m*m)/(radiator->n()*pp));
	      double thp = acos(sqrt(pp*pp + m*m)/(mcparticle->n(radiator)*pp));
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

	} //if (chi^2 estimator)
	
	id /= Event()->ChargedParticles().size();
      } //for mcparticle

      // Solutions for this iteration are in place; keep them for the remaining 
      // [particle,hypothesis] combinations;
      solutionsValid = true;
      
      if (!m_UseMixtureLikelihood) {
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
      
      } //if (chi^2 estimator)

      if (!m_UseMixtureLikelihood) {
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
      } else {
	// Global mixture log-likelihood: every hit contributes through the sum inside 
	// the log, so no hit is discarded and no ownership is resolved at this point;
	lnLsave[ic] = MixtureLnL(calibration, ic);
	if (!ic || lnLsave[ic] > lnLsave[icbest]) icbest = ic;
      } //if

    _next_combination:
      ;
    } //for ic

    // A posteriori hit-to-track assignment for the winning combination;
    if (m_UseMixtureLikelihood && itr) MixtureLnL(calibration, icbest, true);
  } //for itr
  
  if (calibration) 
    UpdateYields();
  else {
    // Now check how the PID procedure performed and fill out histograms;
    // Sure of interest is only to see a flat CCDF distribution for the right combination 
    // of event-level hypotheses; 
    if (Plots() && !m_UseMixtureLikelihood) {
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

// Global mixture log-likelihood for one [particle,hypothesis] combination.
//
// Every detected hit is described by a mixture over all [particle,radiator,optical 
// path] components available for it, plus a flat background component. The hit-to-
// track assignment is therefore marginalized (sum inside the log) rather than 
// resolved a priori, so no hit is ever discarded and a hit claimed by several tracks 
// is shared between them instead of being deleted;
//
// The expected Cherenkov photon yield of every track enters as an extended Poisson 
// term, which is what makes the track hypotheses identifiable once the assignment is 
// marginalized. When `assign` is set, the responsibilities of the converged mixture 
// are used to attach each hit to its most probable track, i.e. the hit-to-track 
// assignment is performed a posteriori;
//
double ReconstructionFactory::MixtureLnL(bool calibration, unsigned ic, bool assign)
{
  const unsigned hdim = m_Hypotheses.size();
  std::vector<ChargedParticle*> &particles = Event()->ChargedParticles();
  const unsigned pdim = particles.size();
  if (!hdim || !pdim) return 0.0;

  // Decode the PID hypotheses of this combination, exactly as the chi^2 path does;
  std::vector<unsigned> hypo(pdim);
  {
    unsigned id = ic;
    for(unsigned ip=0; ip<pdim; ip++) { hypo[ip] = id%hdim; id /= pdim; }
  } //block

  const double lnbg = std::log(m_BackgroundFraction/m_BackgroundWindow);
  const double lnt  = std::log(1.0/pdim);

  double lnL = 0.0;
  std::vector<double> neff(pdim,0.0), thsum(pdim,0.0);

  for(auto &hit: Hits()) {
    struct MyComponent {unsigned m_Ip, m_Ibin; double m_LnG, m_Thp, m_Tt, m_Thdiff, m_Tmdiff;
      CherenkovRadiator *m_Radiator; IRTSolution *m_Solution;};
    std::vector<MyComponent> comps;

    for(unsigned ip=0; ip<pdim; ip++) {
      auto mcparticle = particles[ip];
      auto sit = hit.m_Solutions.find(mcparticle);
      if (sit == hit.m_Solutions.end()) continue;

      double theta = mcparticle->GetVertexMomentum().Theta();
      unsigned ibin = (unsigned)floor(theta/_THETA_BIN_WIDTH_);
      if (ibin >= _THETA_BIN_COUNT_) continue;

      for(auto &tag: sit->second.m_All) {
	auto radiator = tag.first.first;
	if (!radiator->UsedInRingImaging()) continue;
	auto &solution = tag.second;

	// Reference refractive index of this radiator for this particle;
	auto nfit = mcparticle->m_ReferenceRefractiveIndices.find(radiator);
	if (nfit == mcparticle->m_ReferenceRefractiveIndices.end()) continue;

	// Momentum and path length inside this radiator;
	double pp = 0.0, ll = 0.0;
	for(auto rhptr: mcparticle->GetRadiatorHistory())
	  if (mcparticle->GetRadiator(rhptr) == radiator) {
	    pp = mcparticle->GetHistory(rhptr)->m_AverageParentMomentum.Mag();
	    ll = mcparticle->GetHistory(rhptr)->m_EstimatedPath;
	    break;
	  } //if
	if (!pp) continue;

	const double mass = m_Hypotheses[hypo[ip]]->Mass();
	const double thp  = acos(sqrt(pp*pp + mass*mass)/(nfit->second*pp));
	if (std::isnan(thp)) continue;
	const double beta = 1./sqrt(1. + pow(mass/pp,2));
	const double tt   = ll/(beta*300);

	const double hsigma = (calibration || !AutomaticCalibrationRequired()) ?
	  GetDefaultSinglePhotonThetaResolution() : radiator->m_Calibrations[ibin].m_Csigma;

	const double thdiff = solution.GetTheta() - thp - radiator->m_Calibrations[ibin].m_Coffset;
	double lnd = -log(hsigma*sqrt(2*M_PI)) - 0.5*pow(thdiff/hsigma,2);

	double tmdiff = 0.0;
	if (m_UseTimingInChiSquare && GetSinglePhotonTimingResolution()) {
	  tmdiff = (tt + solution.m_Time) - hit.GetAverageDetectionTime();
	  lnd -= log(GetSinglePhotonTimingResolution()*sqrt(2*M_PI)) +
	    0.5*pow(tmdiff/GetSinglePhotonTimingResolution(),2);
	} //if

	comps.push_back(MyComponent{ip, ibin, lnd + lnt, thp, tt, thdiff, tmdiff, radiator, &solution});
      } //for tag
    } //for ip

    if (!comps.size()) { lnL += lnbg; continue; }

    // log-sum-exp over the components and the background slot;
    double mx = lnbg;
    for(auto &c: comps) mx = std::max(mx, c.m_LnG);
    double sum = std::exp(lnbg - mx);
    for(auto &c: comps) sum += std::exp(c.m_LnG - mx);
    const double lnorm = mx + std::log(sum);
    lnL += lnorm;

    // E-step at the current parameters;
    MyComponent *cbest = 0; double rbest = 0.0;
    for(auto &c: comps) {
      const double r = std::exp(c.m_LnG - lnorm);
      neff[c.m_Ip] += r;
      thsum[c.m_Ip] += r*c.m_Thp;
      if (r > rbest) { rbest = r; cbest = &c; }

      if (!assign) continue;
      const bool truth = (particles[c.m_Ip]->GetPDG() ==
			  m_Hypotheses[hypo[c.m_Ip]]->PdgCode());
      if (!calibration && truth && c.m_Radiator->Plots())
	c.m_Radiator->Plots()->hthph()->Fill(1000*c.m_Thdiff, r);
      if (calibration && truth && c.m_Radiator->m_Calibrations[c.m_Ibin].m_hcalib)
	c.m_Radiator->m_Calibrations[c.m_Ibin].m_hcalib->Fill(1000*c.m_Thdiff, r);
    } //for c

    // A posteriori hit-to-track assignment: attach the hit to its most probable 
    // track, provided that track is more likely than the background. The winning 
    // component is also recorded as the "best" solution for this [hit,track] pair, 
    // which is what the track-level Cherenkov observables are built from;
    if (assign && cbest && rbest > 0.5) {
      auto dit = hit.m_Solutions.find(particles[cbest->m_Ip]);
      if (dit != hit.m_Solutions.end()) {
	// note: the last argument is the chi^2-path CCDF slot, reused here for the 
	// posterior probability of this assignment;
	dit->second.AssignVariables(cbest->m_Solution, cbest->m_Thp, cbest->m_Tt,
				   cbest->m_Radiator, rbest);
	particles[cbest->m_Ip]->AddHit(&hit);
      } //if
    } //if
  } //for hit

  // Extended Poisson term for the expected photon yield of every track;
  for(unsigned ip=0; ip<pdim; ip++) {
    const double theta = neff[ip] ? thsum[ip]/neff[ip] : 0.0;
    double lambda = 0.0;
    for(auto rhptr: particles[ip]->GetRadiatorHistory()) {
      auto radiator = particles[ip]->GetRadiator(rhptr);
      if (!radiator->UsedInRingImaging()) continue;
      lambda += radiator->m_YieldCff*radiator->m_DetectedToCalibrationPhotonRatio*
	pow(sin(theta),2);
    } //for rhptr
    if (lambda <= 0.0) continue;
    lnL += neff[ip]*log(lambda) - lambda;
  } //for ip

  return lnL;
} // ReconstructionFactory::MixtureLnL()

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
  if (m_OutputEventTree) m_OutputEventTree->Fill();
  
  // Prepair for next event;
  ClearBlackoutCells();
  ClearDigitizedHits();

  // Get it from the ROOT tree; otherwise assume it is an EICrecon mode where event structure
  // has been populated by the IrtInterface already;
  if (m_InputEventTree) GetInputTreeEntry(ev);
  
  if (VerifyEventStructure()) return Event();
  
  // Use undetected photons (HERE DO THIS ON TRACK PER TRACK BASIS) to extract the 
  // expected average emission point 3D location, time and parent particle 3D momentum;  
  /*if (!m_ExperimentalMode)*/ CalibratePhotonEmissionPoints();

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
  
  if (BeVerbose() && !(ev%100)) printf("Event %5d ...\n", ev);

  m_ProcessedEventCount++;
  
  return Event();
} // ReconstructionFactory::GetEvent()

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
