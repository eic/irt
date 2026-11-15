
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
  m_EmMaxIterations(20), m_EmIterationsUsed(0),
  m_EmTolerance(1e-6),
  m_EmFitCalibration(false),
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
  m_EmMaxIterations(20), m_EmIterationsUsed(0),
  m_EmTolerance(1e-6),
  m_EmFitCalibration(false),
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

  // Expectation-maximization control of the mixture estimator (Stage B3);
  if (jconfig.find("EmMaxIterations") != jconfig.end())
    SetEmMaxIterations((unsigned)jconfig["EmMaxIterations"].template get<int>());

  if (jconfig.find("EmTolerance") != jconfig.end())
    SetEmTolerance(jconfig["EmTolerance"].template get<double>());

  // Also fit the per-[radiator,theta bin] angular offset and resolution inside the 
  // expectation-maximization loop; off by default, as this changes the calibration 
  // constants which the reconstruction itself uses;
  if (jconfig.find("EmFitCalibration") != jconfig.end() &&
      !strcmp(jconfig["EmFitCalibration"].template get<std::string>().c_str(), "yes"))
    UseEmCalibration();

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
  // The chi^2 estimator fixes one hypothesis per particle a priori, hence a scan over 
  // hdim^pdim combinations is needed; the mixture estimator marginalizes over the 
  // hypotheses inside the log, so for it the scan collapses to a single evaluation 
  // (Stage B2);
  if (!m_UseMixtureLikelihood)
    for(unsigned iq=0; iq<Event()->ChargedParticles().size(); iq++)
      cdim *= hdim;
  // Save intermediate chi^2 estimates;
  double  ccdfevsave[cdim];       memset(ccdfevsave, 0x00, sizeof(ccdfevsave));
  double  ccdftrsave[cdim][pdim]; memset(ccdftrsave, 0x00, sizeof(ccdftrsave));
  unsigned npetrsave[cdim][pdim]; memset(npetrsave,  0x00, sizeof(npetrsave));
  std::vector<double> lnLsave(cdim, 0.0);

  // Single-photon evaluation of every hit against every PID hypothesis of every 
  // particle, indexed [(ip*nhits + ihit)*hdim + ih]; recomputed only when the IRT 
  // solutions are (re)calculated, i.e. once per particle per iteration;
  const unsigned nhits = Hits().size();
  std::vector<HypoScore> scores(pdim*nhits*hdim);
  // Same idea for the bookkeeping of the combination scan: the hits selected by each 
  // particle in the current combination, and the [hit,particle] solution pointers, are 
  // kept in flat tables instead of a per-hit std::set and std::map lookups, which used 
  // to dominate the cost of the scan;
  std::vector<std::pair<ChargedParticle*,double>> sel(pdim*nhits);
  std::vector<unsigned> nsel(nhits, 0);
  std::vector<DigitizedHitSolution*> solptr(pdim*nhits, 0);
  // Same for the mixture estimator: the [hit,radiator,optical path] components of each 
  // particle and their per-hypothesis aggregates, evaluated once per iteration (Stage B1);
  MixtureCache mcache;
  // Hypothesis weights of the mixture estimator, [ip*hdim + ih], normalized per 
  // particle, and the winning hypothesis of every particle used for the PID output;
  std::vector<double> weights(pdim*hdim, 0.0);
  std::vector<unsigned> hypoBest(pdim, 0);
  
  // Now loop through all [particle,hypothesis] combinations and evaluate their chi^2;
  for(unsigned itr=0; itr<2; itr++) {
    // IRT solutions do not depend on the PID hypothesis combination, so calculate them 
    // once per iteration and reuse them for every [particle,hypothesis] combination;
    bool solutionsValid = false;

    if (m_UseMixtureLikelihood) {
      mcache.Reset(pdim, nhits, hdim);

      // Hypothesis weights start from the MC truth for the calibration pass (the chi^2 
      // path does the same by forcing the truth combination), and from a uniform prior 
      // otherwise;
      for(unsigned ip=0; ip<pdim; ip++)
	for(unsigned ih=0; ih<hdim; ih++)
	  weights[ip*hdim + ih] = calibration ?
	    (abs(Event()->ChargedParticles()[ip]->GetPDG()) ==
	     abs(m_Hypotheses[ih]->PdgCode()) ? 1.0 : 0.0) : 1.0/hdim;
    } //if

    for(unsigned ic=0; ic<cdim; ic++) {
      if (itr && ic != icbest) continue;

      // A running variable, to simplify hypothesis calculation;
      unsigned id = ic;  

      // Index of the particle in the loop below, used to index the cached hit 
      // evaluation tables;
      unsigned ip = 0;

      // FIXME: may want this check to be a bit more efficient; indeed for calibration pass
      // we better use MC truth as a PID hypothesis for all particles to calculate <theta>
      // as well as the width of this distribution; so skip all other combinations;
      if (calibration && !m_UseMixtureLikelihood) {
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
      for(auto &hit: Hits()) hit.m_BackgroundCandidate = false;
      for(unsigned ihit=0; ihit<nhits; ihit++) nsel[ihit] = 0;

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

	// The IRT solutions are in place; evaluate every hit for every PID hypothesis 
	// of this particle once, so the combination scan below is pure lookups;
	if (!m_UseMixtureLikelihood) ScoreHits(mcparticle, ip, calibration, scores);
	else ScoreMixture(mcparticle, ip, calibration, mcache);

	// Cache the [hit,particle] solution pointers of this particle, so the 
	// combination scan does not repeat a std::map lookup per hit per combination;
	for(unsigned ihit=0; ihit<nhits; ihit++)
	  solptr[ip*nhits + ihit] = &Hits()[ihit].m_Solutions[mcparticle];
  } //if
	
	if (!m_UseMixtureLikelihood) {
	//
	// By this moment every detected hit has been evaluated with respect to this 
	// particle trajectory as originated from every radiator via every possible optical 
	// path, for every PID hypothesis, and the outcome cached in `scores` (see 
	// ScoreHits()); the [particle,hypothesis] combination scan is thus reduced to 
	// table lookups, with no ray-tracing post-processing left in it;
	//
	for(unsigned ihit=0; ihit<nhits; ihit++) {
	  auto &hit = Hits()[ihit];
	  auto sptr = solptr[ip*nhits + ihit];
	  const HypoScore *tab = &scores[(ip*nhits + ihit)*hdim];

	  // Will never have means to know from which radiator a given photon originated, and 
	  // via which optical path it arrived to the sensor; the [radiator,irt] component 
	  // which matches best for the hypothesis considered here, on the criterion of 
	  // "closest in theta and closest in timing", has been picked when the hits were 
	  // scored;
	  sptr->m_Best = 0;

	  // The logic behind this is that all hits within +/-N*sigma window with respect to 
	  // at least one hypothesis in at least one radiator for at least one optical path 
	  // for every particle belong to the club "which needs to be explained" in terms of 
	  // Poisson statistics;
	  for(unsigned ih=0; ih<hdim; ih++)
	    if (ih != hypo && tab[ih].m_Ccdf > m_SingleHitCCDFcut) hit.m_BackgroundCandidate = true;

	  const HypoScore &sc = tab[hypo];

	  if (!sc.m_Solution) continue;

	  sptr->AssignVariables(sc.m_Solution, sc.m_Thp, sc.m_Tt, sc.m_Radiator, sc.m_Ccdf);

	  // FIXME: should be radiator-specific;
	  if (sc.m_Ccdf > m_SingleHitCCDFcut) {
	    sel[ihit*pdim + nsel[ihit]++] = std::make_pair(mcparticle, sc.m_Chi2);

	    // Sure of interest is only to see a distribution for the right hypothesis; 
	    if (mcparticle->GetPDG() == hyparticle->PdgCode()) {
	      if (itr && !calibration && sc.m_Radiator->Plots())
		sc.m_Radiator->Plots()->hthph()->Fill(1000*sc.m_Thdiff);

	      if (itr &&  calibration)
		sc.m_Radiator->m_Calibrations[ibin].m_hcalib->Fill(1000*sc.m_Thdiff);

	      if (itr && !calibration && Plots()) Plots()->hdtph()->Fill(1000*sc.m_Tmdiff);

	      if (itr && !calibration && sc.m_Radiator->Plots())
		sc.m_Radiator->Plots()->hccdfph()->Fill(sc.m_Ccdf);
	    } //if
	  } //if
	} //for hits

	} //if (chi^2 estimator)
	
	ip++;
	id /= Event()->ChargedParticles().size();
      } //for mcparticle

      // Solutions for this iteration are in place; keep them for the remaining 
      // [particle,hypothesis] combinations;
      solutionsValid = true;
      
      if (!m_UseMixtureLikelihood) {
      // Clean up background candidate list; obviously if at least one particle selected this 
      // hit for a hypothesis considered in this iteration, the hit is useful;
      for(unsigned ihit=0; ihit<nhits; ihit++)
	if (nsel[ihit])
	  Hits()[ihit].m_BackgroundCandidate = false;
      
      // Now either resolve ambiguous assignments, based on the hit-to-track chi^2 match for this 
      // PID hypothesis, or eliminate these hits everywhere; FIXME: any better idea?;
      for(unsigned ihit=0; ihit<nhits; ihit++) 
	if (nsel[ihit] >= 1) {
	  if (m_ResolveHitOwnership) {
	    double chi2min = sel[ihit*pdim].second;
	    for(unsigned k=1; k<nsel[ihit]; k++)
	      if (sel[ihit*pdim + k].second < chi2min) chi2min = sel[ihit*pdim + k].second;
	    // Keep the matching entries in their original (hit) order;
	    unsigned keep = 0;
	    for(unsigned k=0; k<nsel[ihit]; k++)
	      if (sel[ihit*pdim + k].second == chi2min) {
		if (keep != k) sel[ihit*pdim + keep] = sel[ihit*pdim + k];
		keep++;
	      } //if
	    nsel[ihit] = keep;
	  } //if
	  else
	    nsel[ihit] = 0;
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
	
	// Per-track sums over the selected hits. Walking the hits, instead of the tracks 
	// times the per-hit selection lists, removes a pdim*nhits loop per combination; 
	// the order in which hits are attached to a given track is unchanged;
	std::vector<unsigned> npev(pdim, 0);
	std::vector<double> chi2trv(pdim, 0.0);
	for(unsigned ihit=0; ihit<nhits; ihit++)
	  for(unsigned k=0; k<nsel[ihit]; k++) {
	    unsigned ipx = 0;
	    while(ipx < pdim && Event()->ChargedParticles()[ipx] != sel[ihit*pdim + k].first) ipx++;
	    if (ipx >= pdim) continue;

	    npev[ipx]++;
	    chi2trv[ipx] += sel[ihit*pdim + k].second;

	    if (itr) Event()->ChargedParticles()[ipx]->AddHit(&Hits()[ihit]);
	  } //for ihit

	unsigned ndfev = 0, idx = 0;
	double chi2ev = 0;
	for(auto mcparticle: Event()->ChargedParticles()) {
	  unsigned npe = npev[idx];
	  double chi2tr = chi2trv[idx];
	  
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
	lnLsave[ic] = RunEM(calibration, mcache, weights);
	if (!ic || lnLsave[ic] > lnLsave[icbest]) icbest = ic;
      } //if

    _next_combination:
      ;
    } //for ic

    // A posteriori hit-to-track assignment for the winning combination;
    if (m_UseMixtureLikelihood && itr) MixtureLnLWeighted(calibration, mcache, weights, true, false);
  } //for itr

  // The winning hypothesis of every particle: decoded from the best combination for the 
  // chi^2 estimator, read off the marginalized weights for the mixture estimator;
  if (m_UseMixtureLikelihood) {
    for(unsigned ip=0; ip<pdim; ip++) {
      unsigned ihbest = 0;
      for(unsigned ih=1; ih<hdim; ih++)
	if (weights[ip*hdim + ih] > weights[ip*hdim + ihbest]) ihbest = ih;
      hypoBest[ip] = ihbest;
    } //for ip
  } else {
    unsigned id = icbest;
    for(unsigned ip=0; ip<pdim; ip++) { hypoBest[ip] = id%hdim; id /= pdim; }
  } //if
  
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
      unsigned ip = 0;
      for(auto mcparticle: Event()->ChargedParticles()) {
	unsigned hypo = hypoBest[ip];
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
	
	ip++;
      } //for ip
    }
  } //if
} // ReconstructionFactory::LaunchRingFinder()

// -------------------------------------------------------------------------------------

// Evaluate every detected hit with respect to one particle, for every PID hypothesis, 
// and cache the outcome. None of this depends on the [particle,hypothesis] combination, 
// so it is done once per particle per iteration and the combination scan only has to 
// read these tables. For each hypothesis the [radiator,optical path] component with the 
// largest single-photon CCDF is kept, exactly as the in-line scan used to pick it;
//
void ReconstructionFactory::ScoreHits(ChargedParticle *mcparticle, unsigned ip,
                                      bool calibration, std::vector<HypoScore> &scores)
{
  const unsigned hdim  = m_Hypotheses.size();
  const unsigned nhits = Hits().size();

  double theta = mcparticle->GetVertexMomentum().Theta();
  unsigned ibin = (unsigned)floor(theta / _THETA_BIN_WIDTH_);

  std::map<CherenkovRadiator*, double> momenta;
  std::map<CherenkovRadiator*, double> paths;
  for(auto rhptr: mcparticle->GetRadiatorHistory()) {
    auto radiator = mcparticle->GetRadiator(rhptr);

    momenta[radiator] = mcparticle->GetHistory(rhptr)->m_AverageParentMomentum.Mag();
    paths  [radiator] = mcparticle->GetHistory(rhptr)->m_EstimatedPath;
  } //for rhptr

  for(unsigned ihit=0; ihit<nhits; ihit++) {
    auto &hit = Hits()[ihit];
    HypoScore *tab = &scores[(ip*nhits + ihit)*hdim];

    for(unsigned ih=0; ih<hdim; ih++) tab[ih] = HypoScore();

    for(auto &tag: hit.m_Solutions[mcparticle].m_All) {
      auto radiator = tag.first.first;
      auto &solution = tag.second;

      //if (!_IsSelected(radiator)) continue;
      if (!radiator->UsedInRingImaging()) continue;

      double hsigma = (calibration || !AutomaticCalibrationRequired()) ?
	GetDefaultSinglePhotonThetaResolution() : radiator->m_Calibrations[ibin].m_Csigma;

      double pp = momenta[radiator], ll = paths[radiator];

      for(unsigned ih=0; ih<hdim; ih++) {
	double mass = m_Hypotheses[ih]->Mass();

	double beta   = 1./sqrt(1. + pow(mass/pp, 2)), tt = ll/(beta*300);
	double thp    = acos(sqrt(pp*pp + mass*mass)/(mcparticle->n(radiator)*pp));
	double thdiff = solution.GetTheta() - thp - radiator->m_Calibrations[ibin].m_Coffset;
	double tmdiff = (tt + solution.m_Time) - hit.GetAverageDetectionTime();

	unsigned ndf = 1;
	double chi2 = pow(thdiff, 2)/pow(hsigma, 2);
	if (m_UseTimingInChiSquare && GetSinglePhotonTimingResolution()) {
	  ndf++;
	  chi2 += pow(tmdiff, 2)/pow(GetSinglePhotonTimingResolution(), 2);
	} //if
	double ccdf = ROOT::Math::chisquared_cdf_c(chi2, ndf);

	// Keep the component matching best for this hypothesis; on a tie the first one 
	// wins, as it did in the in-line scan;
	if (!tab[ih].m_Solution || ccdf > tab[ih].m_Ccdf) {
	  tab[ih] = HypoScore();
	  tab[ih].m_Solution = &solution;
	  tab[ih].m_Radiator  = radiator;
	  tab[ih].m_Thp    = thp;    tab[ih].m_Tt     = tt;
	  tab[ih].m_Chi2   = chi2;   tab[ih].m_Ccdf   = ccdf;
	  tab[ih].m_Thdiff = thdiff; tab[ih].m_Tmdiff = tmdiff;
	  tab[ih].m_Ndf    = ndf;
	} //if
      } //for ih
    } //for tag
  } //for hit
} // ReconstructionFactory::ScoreHits()

// -------------------------------------------------------------------------------------
//
// Stage B1: making the mixture objective separable.
//
// The mixture log-likelihood is a sum over hits of a log-sum-exp over every 
// [particle,radiator,optical path] component available for that hit, plus a flat 
// background term. No component depends on the [particle,hypothesis] combination, so 
// -- exactly as ScoreHits() does for the chi^2 estimator -- the components and their 
// per-hypothesis aggregates are evaluated once per particle per iteration. The 
// objective is then a sum over hits of log-sum-exp over cached entries only: it is 
// separable, and the [particle,hypothesis] combination scan no longer feeds anything 
// into it (which is what allows that scan to be dropped).
//
// Log-density of one component under one PID hypothesis; returns false when this 
// [momentum, mass, refractive index] combination emits no Cherenkov light.
//
bool ReconstructionFactory::MixtureLnD(const MixComponent &comp, unsigned ih, double &lnd,
                                       double &thp, double &tt, double &thdiff,
                                       double &tmdiff) const
{
  const double mass = m_Hypotheses[ih]->Mass();

  thp = acos(sqrt(comp.m_Pp*comp.m_Pp + mass*mass)/(comp.m_Nref*comp.m_Pp));
  if (std::isnan(thp)) return false;

  const double beta = 1./sqrt(1. + pow(mass/comp.m_Pp, 2));

  tt     = comp.m_Ll/(beta*300);
  thdiff = comp.m_Theta - thp - comp.m_Coffset;

  lnd = -log(comp.m_Hsigma*sqrt(2*M_PI)) - 0.5*pow(thdiff/comp.m_Hsigma, 2);

  tmdiff = 0.0;
  if (m_UseTimingInChiSquare && GetSinglePhotonTimingResolution()) {
    tmdiff = (tt + comp.m_Time) - comp.m_Tmeas;
    lnd -= log(GetSinglePhotonTimingResolution()*sqrt(2*M_PI)) +
      0.5*pow(tmdiff/GetSinglePhotonTimingResolution(), 2);
  } //if

  return true;
} // ReconstructionFactory::MixtureLnD()

// -------------------------------------------------------------------------------------

// Fill the component list and the per-hypothesis aggregates of one particle. Everything 
// which does not depend on the PID hypothesis -- radiator, IRT solution, calibration 
// bin, momentum and path length in the radiator, reference refractive index, angular 
// resolution and offset, measured detection time -- is resolved here once;
//
void ReconstructionFactory::ScoreMixture(ChargedParticle *mcparticle, unsigned ip,
                                         bool calibration, MixtureCache &cache)
{
  const unsigned hdim  = cache.m_Hdim;
  const unsigned nhits = cache.m_Nhits;

  const double theta = mcparticle->GetVertexMomentum().Theta();
  const unsigned ibin = (unsigned)floor(theta/_THETA_BIN_WIDTH_);
  if (ibin >= _THETA_BIN_COUNT_) return;

  // Momentum and path length inside every radiator this particle crossed;
  std::map<CherenkovRadiator*, std::pair<double,double>> mompath;
  for(auto rhptr: mcparticle->GetRadiatorHistory()) {
    auto radiator = mcparticle->GetRadiator(rhptr);
    mompath[radiator] = std::make_pair(
      mcparticle->GetHistory(rhptr)->m_AverageParentMomentum.Mag(),
      mcparticle->GetHistory(rhptr)->m_EstimatedPath);
  } //for rhptr

  // Scratch space for the log-densities of the current [hit,hypothesis] evaluation;
  std::vector<double> lnds, thps;

  for(unsigned ihit=0; ihit<nhits; ihit++) {
    auto &hit = Hits()[ihit];
    const unsigned idx = ip*nhits + ihit;

    cache.m_Beg[idx] = cache.m_Comps.size();
    cache.m_Num [idx] = 0;

    auto sit = hit.m_Solutions.find(mcparticle);
    if (sit == hit.m_Solutions.end()) continue;

    for(auto &tag: sit->second.m_All) {
      auto radiator = tag.first.first;
      //if (!_IsSelected(radiator)) continue;
      if (!radiator->UsedInRingImaging()) continue;

      // Reference refractive index of this radiator for this particle;
      auto nfit = mcparticle->m_ReferenceRefractiveIndices.find(radiator);
      if (nfit == mcparticle->m_ReferenceRefractiveIndices.end()) continue;

      auto mp = mompath.find(radiator);
      if (mp == mompath.end() || !mp->second.first) continue;

      MixComponent comp;
      comp.m_Ip       = ip;
      comp.m_Ibin     = ibin;
      comp.m_Radiator = radiator;
      comp.m_Solution = &tag.second;
      comp.m_Pp       = mp->second.first;
      comp.m_Ll       = mp->second.second;
      comp.m_Nref     = nfit->second;
      comp.m_Theta    = tag.second.GetTheta();
      comp.m_Time     = tag.second.m_Time;
      comp.m_Tmeas    = hit.GetAverageDetectionTime();
      comp.m_Hsigma   = (calibration || !AutomaticCalibrationRequired()) ?
        GetDefaultSinglePhotonThetaResolution() : radiator->m_Calibrations[ibin].m_Csigma;
      comp.m_Coffset  = radiator->m_Calibrations[ibin].m_Coffset;

      cache.m_Comps.push_back(comp);
      cache.m_Num[idx]++;
    } //for tag

    // Per-hypothesis aggregation over the components of this [particle,hit] pair;
    const unsigned nbeg = cache.m_Beg[idx], nnum = cache.m_Num[idx];
    if (!nnum) continue;

    const double ninf = -std::numeric_limits<double>::infinity();

    lnds.resize(nnum); thps.resize(nnum);

    for(unsigned ih=0; ih<hdim; ih++) {
      double mx = ninf;

      for(unsigned k=0; k<nnum; k++) {
        double lnd, thp, tt, thdiff, tmdiff;
        if (!MixtureLnD(cache.m_Comps[nbeg + k], ih, lnd, thp, tt, thdiff, tmdiff)) {
          lnds[k] = ninf; continue;
        } //if
        lnds[k] = lnd; thps[k] = thp;
        if (lnd > mx) mx = lnd;
      } //for k

      if (mx == ninf) continue;

      double sum = 0.0, thw = 0.0, thsw = 0.0, ths2 = 0.0, thsp = 0.0, thp2 = 0.0;
      for(unsigned k=0; k<nnum; k++) {
        if (lnds[k] == ninf) continue;
        const double w = std::exp(lnds[k] - mx);
        const double thp = thps[k], ths = cache.m_Comps[nbeg + k].m_Theta;
        sum  += w;
        thw  += w*thp;
        // moments of the measured and predicted Cherenkov angles, needed by the 
        // optional calibration M-step (Stage B3);
        thsw += w*ths; ths2 += w*ths*ths; thsp += w*ths*thp; thp2 += w*thp*thp;
      } //for k

      MixAgg &ag = cache.m_Agg[idx*hdim + ih];
      ag.m_LnSum    = mx + std::log(sum);
      ag.m_ThBar    = thw/sum;
      ag.m_ThSolBar = thsw/sum; ag.m_ThSol2 = ths2/sum;
      ag.m_ThSolP   = thsp/sum; ag.m_ThP2   = thp2/sum;
    } //for ih
  } //for hit
} // ReconstructionFactory::ScoreMixture()

// -------------------------------------------------------------------------------------
//
// Stage B2: the mixture objective with the PID hypothesis promoted to a component 
// index, i.e. a separable objective which no longer needs a combination scan.
//
// Every detected hit is described by a mixture over all 
// [particle,hypothesis,radiator,optical path] components available for it, plus a flat 
// background component. Neither the hit-to-track assignment nor the PID hypothesis 
// choice is resolved a priori: both are marginalized inside the log, a hit claimed by 
// several tracks is shared between them, and no hit is ever discarded. The expected 
// Cherenkov photon yield of every track enters as an extended Poisson term, which is 
// what makes the tracks identifiable once the assignment is marginalized.
//
// Because the objective is separable (Stage B1), there is nothing left to scan over: 
// the hdim^pdim [particle,hypothesis] combination loop collapses to a single 
// evaluation, and the hypothesis content of every track is carried by `weights` 
// (pdim*hdim entries, summing to 1 for each particle). With `mstep` the 
// responsibilities update those weights -- one expectation-maximization step; with 
// `assign` each hit is attached to its most probable track a posteriori.
//
double ReconstructionFactory::MixtureLnLWeighted(bool calibration, const MixtureCache &cache,
                                                 std::vector<double> &weights,
                                                 bool assign, bool mstep,
                                                 std::map<std::pair<CherenkovRadiator*,unsigned>,
                                                           std::array<double,3>> *calib)
{
  const unsigned hdim = cache.m_Hdim, nhits = cache.m_Nhits, pdim = cache.m_Pdim;
  std::vector<ChargedParticle*> &particles = Event()->ChargedParticles();
  if (!hdim || !pdim) return 0.0;

  const double lnbg = std::log(m_BackgroundFraction/m_BackgroundWindow);
  const double lnt  = std::log(1.0/pdim);
  const double ninf = -std::numeric_limits<double>::infinity();

  // Logarithms of the current hypothesis weights; a zero weight simply drops that 
  // component from the mixture;
  std::vector<double> lnw(pdim*hdim);
  for(unsigned i=0; i<pdim*hdim; i++)
    lnw[i] = weights[i] > 0.0 ? std::log(weights[i]) : ninf;

  double lnL = 0.0;
  std::vector<double> neff(pdim, 0.0), thsum(pdim, 0.0), nhip(pdim*hdim, 0.0);
  // Scratch space for the responsibilities of one hit;
  std::vector<double> resp(pdim*hdim, 0.0);

  for(unsigned ihit=0; ihit<nhits; ihit++) {
    auto &hit = Hits()[ihit];

    // log-sum-exp over the background slot and every [particle,hypothesis] component;
    double mx = lnbg;
    for(unsigned ip=0; ip<pdim; ip++)
      for(unsigned ih=0; ih<hdim; ih++) {
        const MixAgg &ag = cache.m_Agg[(ip*nhits + ihit)*hdim + ih];
        if (ag.m_LnSum == ninf || lnw[ip*hdim + ih] == ninf) continue;
        const double tt = ag.m_LnSum + lnt + lnw[ip*hdim + ih];
        if (tt > mx) mx = tt;
      } //for ip, ih

    double sum = std::exp(lnbg - mx);
    for(unsigned ip=0; ip<pdim; ip++)
      for(unsigned ih=0; ih<hdim; ih++) {
        const MixAgg &ag = cache.m_Agg[(ip*nhits + ihit)*hdim + ih];
        if (ag.m_LnSum == ninf || lnw[ip*hdim + ih] == ninf) continue;
        sum += std::exp(ag.m_LnSum + lnt + lnw[ip*hdim + ih] - mx);
      } //for ip, ih

    const double lnorm = mx + std::log(sum);
    lnL += lnorm;

    // E-step at the current parameters: the responsibility of every track, and of each 
    // of its hypotheses, for this hit follows from the cached aggregates alone;
    double rbest = 0.0; unsigned ipbest = 0;
    for(unsigned ip=0; ip<pdim; ip++) {
      double rip = 0.0, th = 0.0;
      for(unsigned ih=0; ih<hdim; ih++) {
        const MixAgg &ag = cache.m_Agg[(ip*nhits + ihit)*hdim + ih];
        if (ag.m_LnSum == ninf || lnw[ip*hdim + ih] == ninf) {
          resp[ip*hdim + ih] = 0.0; continue;
        } //if
        const double r = std::exp(ag.m_LnSum + lnt + lnw[ip*hdim + ih] - lnorm);
        resp[ip*hdim + ih] = r;
        rip += r; th += r*ag.m_ThBar;
        nhip[ip*hdim + ih] += r;
      } //for ih
      neff[ip] += rip; thsum[ip] += th;
      if (rip > rbest) { rbest = rip; ipbest = ip; }
    } //for ip

    if (!assign && !calib) continue;

    // The a posteriori assignment, and the optional calibration M-step, need the 
    // per-component responsibilities. The plots 
    // are filled for every component, weighted by its responsibility; the hit itself is 
    // attached to the most probable track, via the [hypothesis,radiator,optical path] 
    // component which matches best for that track;
    const MixComponent *cbest = 0; double cbest_r = 0.0, bthp = 0.0, btt = 0.0;

    for(unsigned ip=0; ip<pdim; ip++) {
      const unsigned idx = ip*nhits + ihit;

      for(unsigned k=0; k<cache.m_Num[idx]; k++) {
        const MixComponent &comp = cache.m_Comps[cache.m_Beg[idx] + k];

        for(unsigned ih=0; ih<hdim; ih++) {
          if (lnw[ip*hdim + ih] == ninf) continue;

          double lnd, thp, tt, thdiff, tmdiff;
          if (!MixtureLnD(comp, ih, lnd, thp, tt, thdiff, tmdiff)) continue;

          const double r = std::exp(lnd + lnt + lnw[ip*hdim + ih] - lnorm);

          const bool truth = (particles[ip]->GetPDG() == m_Hypotheses[ih]->PdgCode());
          if (!calibration && truth && comp.m_Radiator->Plots())
            comp.m_Radiator->Plots()->hthph()->Fill(1000*thdiff, r);
          if (calibration && truth && comp.m_Radiator->m_Calibrations[comp.m_Ibin].m_hcalib)
            comp.m_Radiator->m_Calibrations[comp.m_Ibin].m_hcalib->Fill(1000*thdiff, r);

          if (calib) {
            // sums of [N, sum r*(theta_meas - theta_pred), sum r*(theta_meas - theta_pred)^2] 
            // per [radiator,theta bin], i.e. the moments of a weighted least-squares fit; 
            auto &s = (*calib)[std::make_pair(comp.m_Radiator, comp.m_Ibin)];
            const double dd = comp.m_Theta - thp;
            s[0] += r; s[1] += r*dd; s[2] += r*dd*dd;
          } //if

          if (ip == ipbest && r > cbest_r) { cbest_r = r; cbest = &comp; bthp = thp; btt = tt; }
        } //for ih
      } //for k
    } //for ip

    // Attach the hit to its most probable track, provided that track is more likely 
    // than the background. The winning component is also recorded as the "best" 
    // solution for this [hit,track] pair, which is what the track-level Cherenkov 
    // observables are built from;
    if (assign && cbest && rbest > 0.5) {
      auto dit = hit.m_Solutions.find(particles[ipbest]);
      if (dit != hit.m_Solutions.end()) {
        // note: the last argument is the chi^2-path CCDF slot, reused here for the 
        // posterior probability of this assignment;
        dit->second.AssignVariables(cbest->m_Solution, bthp, btt, cbest->m_Radiator, rbest);
        particles[ipbest]->AddHit(&hit);
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

  // M-step for the hypothesis weights of every track (the objective above was 
  // evaluated at the weights coming in);
  if (mstep) {
    for(unsigned ip=0; ip<pdim; ip++) {
      double tot = 0.0;
      for(unsigned ih=0; ih<hdim; ih++) tot += nhip[ip*hdim + ih];
      // A track nobody claimed keeps the weights it had;
      if (tot <= 0.0) continue;
      for(unsigned ih=0; ih<hdim; ih++) weights[ip*hdim + ih] = nhip[ip*hdim + ih]/tot;
    } //for ip
  } //if

  return lnL;
} // ReconstructionFactory::MixtureLnLWeighted()

// -------------------------------------------------------------------------------------
//
// Stage B3: expectation-maximization iterations for the mixture estimator.
//
// The expectation step is the separable mixture objective of Stage B2, evaluated at the
// current hypothesis weights of every track; the maximization step updates those weights
// in closed form from the responsibilities (a track nobody claimed keeps the weights it
// had). Iterations continue until the objective stops improving.
//
// Deliberately *not* free parameters of the fit: the expected photon yield of a track.
// It is tied to the responsibility-weighted mean Cherenkov angle of that track by the
// radiator yield model, and that tie is exactly what makes the hypotheses identifiable
// once the hit-to-track assignment is marginalized -- freeing the yield would turn the
// extended Poisson term into a monotone function of the number of assigned photons and
// destroy the discrimination. The global yield coefficients are still fitted by the
// existing calibration machinery (UpdateYields).
//
// Optionally (off by default) the per-[radiator,theta bin] angular offset and width are
// also updated by a maximization step, which is a weighted least-squares fit of the
// single-photon angle residuals; since the objective uses those constants, the
// component caches are rebuilt after such an update.
//
double ReconstructionFactory::RunEM(bool calibration, MixtureCache &cache,
                                    std::vector<double> &weights)
{
  const double ninf = -std::numeric_limits<double>::infinity();

  double prev = ninf, lnL = ninf;
  m_EmIterationsUsed = 0;

  for(unsigned it=0; it<m_EmMaxIterations; it++) {
    std::map<std::pair<CherenkovRadiator*,unsigned>, std::array<double,3>> sums;

    lnL = MixtureLnLWeighted(calibration, cache, weights, false, true,
                             m_EmFitCalibration ? &sums : 0);
    m_EmIterationsUsed = it + 1;

    if (m_EmFitCalibration) {
      ApplyCalibrationMStep(sums);
      RebuildMixtureCache(cache, calibration);
    } //if

    // The objective is evaluated at the parameters which were in effect for this 
    // expectation step, so successive values are directly comparable;
    if (it && std::fabs(lnL - prev) < m_EmTolerance) break;
    prev = lnL;
  } //for it

  if (BeVerbose()) printf("ReconstructionFactory: EM used %u iterations, lnL = %g\n",
                          m_EmIterationsUsed, lnL);

  return lnL;
} // ReconstructionFactory::RunEM()

// -------------------------------------------------------------------------------------

void ReconstructionFactory::RebuildMixtureCache(MixtureCache &cache, bool calibration)
{
  const unsigned pdim = cache.m_Pdim, nhits = cache.m_Nhits, hdim = cache.m_Hdim;

  cache.Reset(pdim, nhits, hdim);

  unsigned ip = 0;
  for(auto mcparticle: Event()->ChargedParticles())
    ScoreMixture(mcparticle, ip++, calibration, cache);
} // ReconstructionFactory::RebuildMixtureCache()

// -------------------------------------------------------------------------------------

void ReconstructionFactory::ApplyCalibrationMStep(
  const std::map<std::pair<CherenkovRadiator*,unsigned>, std::array<double,3>> &sums)
{
  for(auto &entry: sums) {
    auto radiator = entry.first.first;
    const unsigned ibin = entry.first.second;
    const double n = entry.second[0];

    if (!radiator || ibin >= _THETA_BIN_COUNT_ || n <= 0.0) continue;

    const double off = entry.second[1]/n;
    double var = entry.second[2]/n - off*off;
    if (var < 0.0) var = 0.0;

    auto &calib = radiator->m_Calibrations[ibin];
    calib.m_Coffset = off;
    calib.m_Csigma  = std::sqrt(var);
  } //for entry
} // ReconstructionFactory::ApplyCalibrationMStep()

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
