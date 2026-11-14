#pragma once

#ifdef JSON_IMPORT_EXPORT
#include <nlohmann/json.hpp>
#endif

#include <TH1D.h>
class TParticlePDG;
class TCanvas;
class TFile;

#include <vector>

#include "Calibration.h"
#include "Digitization.h"

namespace IRT2 {

class ChargedParticle;
class CherenkovRadiator;
class IRTSolution;

struct ReconstructionFactoryPlots {
  ReconstructionFactoryPlots();
  ~ReconstructionFactoryPlots() {
    delete m_hnpe;
    delete m_hnhits;
    delete m_hccdftr;
    delete m_hccdfev;
    delete m_hdtph;
    delete m_hmatch;
  };
  
  // Monte-Carlo plot(s);
  TH1D *hnpe()              const { return m_hnpe;  };
  
  // Reconstruction plots;
  TH1D *hnhits()            const { return m_hnhits;  };
  TH1D *hccdftr()           const { return m_hccdftr;  };
  TH1D *hccdfev()           const { return m_hccdfev;  };
  TH1D *hdtph()             const { return m_hdtph;  };
  TH1D *hmatch()            const { return m_hmatch;  };
  
private:
  TH1D *m_hnpe, *m_hnhits, *m_hccdftr, *m_hccdfev, *m_hdtph, *m_hmatch;
};

class ReconstructionFactory : public Digitization, public Calibration {
 public:
  ReconstructionFactory(const char *dfname = 0, const char *cfname = 0, const char *dname = 0);
  ReconstructionFactory(CherenkovDetectorCollection *geometry, CherenkovDetector *cdet, CherenkovEvent *event,
			const char *json_config_file_name = 0);
  virtual ~ReconstructionFactory();
  
  // Will be empty calls if JSON_IMPORT_EXPORT is not defined;
  void JsonParser(const char *fname);
#ifdef JSON_IMPORT_EXPORT
  void JsonParser(nlohmann::json jconfig);
#endif
  
  void IgnoreTimingInChiSquare( void )               { m_UseTimingInChiSquare = false; };
  void IgnorePoissonTermInChiSquare( void )          { m_UsePoissonTermInChiSquare = false; };
  void SetSingleHitCCDFcut(double value)             { m_SingleHitCCDFcut = value; };
  void RemoveAmbiguousHits( void )                   { m_ResolveHitOwnership = false; };

  int AddHypothesis(int pdg);
  int AddHypothesis(const char *pdg);

  //void SetExperimentalMode( void )                   { m_ExperimentalMode = true; };
  void IgnoreMcTruthPhotonDirectionSeed( void )      { m_UseMcTruthPhotonDirectionSeed = false; };

  CherenkovEvent *GetEvent(unsigned ev, bool calibration = false);

  void SetQuietMode( void )       { m_VerboseMode = false; };
  
  // Multi-particle case;
  void ProcessHits(ChargedParticle *mcparticle, std::vector<DigitizedHit> &hits, bool use_seed = false);

  CherenkovRadiator *UseInRingImaging(const char *name) {
    if (!name) return 0;
    
    auto *radiator = GetMyRICH()->GetRadiator(name);
    if (!radiator) return 0;

    radiator->UseInRingImaging();
    return radiator;
  };

  int VerifyEventStructure( void );
  
  void InitializePlots( void ) { m_Plots = new ReconstructionFactoryPlots(); };
  ReconstructionFactoryPlots *Plots( void ) const { return m_Plots; };
  TCanvas *DisplayStandardPlots(const char *cname, int wtopx, unsigned wtopy, unsigned wx, unsigned wy) const;
  
  void SetHitCountCutoff(unsigned value) { m_HitCountCutoff = value; };

  unsigned GetProcessedEventCount() const { return m_ProcessedEventCount; };

  // Estimator used to rank [particle,hypothesis] combinations: chi^2/CCDF (default) 
  // or the global mixture log-likelihood;
  bool MixtureLikelihoodRequired( void ) const { return m_UseMixtureLikelihood; };
  void UseMixtureLikelihood( bool value = true ) { m_UseMixtureLikelihood = value; };
  void SetBackgroundFraction( double value ) { m_BackgroundFraction = value; };
  double GetBackgroundFraction( void ) const { return m_BackgroundFraction; };
  
 private:
  TFile *m_OutputFile;
  
  bool m_VerboseMode;
  
  // Whether use hit timing or not in the chi^2 logic, see below;
  bool m_UseTimingInChiSquare;
  // Hits with chi^2 built in a 2D space (theta mismatch and timing mismatch) or 
  // just 1D space (theta mismatch only) will be accepted as "good" ones if 
  // the respective CCDF is more than this number;
  double m_SingleHitCCDFcut;

  // If true, keep at most one track owner for each hit; if false, delete 
  // hits, which would fit more than one track for a given combination of hypotheses;
  bool m_ResolveHitOwnership;

  // Whether use Poisson in the chi^2 logic or not;
  bool m_UsePoissonTermInChiSquare;

  // Estimator used to rank [particle,hypothesis] combinations and to decide which 
  // hits are kept: false = chi^2/CCDF with a hard, a priori hit-to-track assignment; 
  // true = global mixture log-likelihood, where the assignment is marginalized over 
  // all [particle,radiator,optical path] components and performed a posteriori;
  bool m_UseMixtureLikelihood;

  // Fraction of photosensor hits attributed to the flat background component of the 
  // per-hit mixture, and the angular window [rad] normalizing that component;
  double m_BackgroundFraction, m_BackgroundWindow;

  bool /*m_ExperimentalMode,*/ m_UseMcTruthPhotonDirectionSeed;

  std::vector<TParticlePDG*> m_Hypotheses;

  ReconstructionFactoryPlots  *m_Plots;      

  unsigned m_HitCountCutoff;
  unsigned m_ProcessedEventCount;

  bool m_CombinedPlotVisualizationEnabled;
  int m_wtopx;
  unsigned m_wtopy, m_wx, m_wy;

  TTree* m_OutputEventTree;
  TBranch* m_OutputEventBranch;
  
  bool BeVerbose( void )    const { return m_VerboseMode; };
  void LaunchRingFinder(bool calibration);

  // Global mixture log-likelihood of the [particle,hypothesis] combination `ic`; 
  // when `assign` is set, the resulting responsibilities are also used to attach 
  // every hit to its most probable track (a posteriori assignment);
  double MixtureLnL(bool calibration, unsigned ic, bool assign = false);

  // Single-photon evaluation of one hit for one PID hypothesis of one particle: the 
  // [radiator,optical path] component which matches best for that hypothesis, together 
  // with everything the combination scan needs to know about it;
  struct HypoScore { IRTSolution *m_Solution; CherenkovRadiator *m_Radiator;
    double m_Thp, m_Tt, m_Chi2, m_Ccdf, m_Thdiff, m_Tmdiff; unsigned m_Ndf;
    HypoScore(): m_Solution(0), m_Radiator(0), m_Thp(0.0), m_Tt(0.0), m_Chi2(0.0),
      m_Ccdf(0.0), m_Thdiff(0.0), m_Tmdiff(0.0), m_Ndf(1) {};
  };

  // Fill the [particle,hit,hypothesis] cache of the above for one particle. These 
  // quantities do not depend on the [particle,hypothesis] combination at all, so they 
  // are evaluated once per particle per iteration, which leaves the combination scan 
  // itself with nothing but table lookups;
  void ScoreHits(ChargedParticle *mcparticle, unsigned ip, bool calibration,
                 std::vector<HypoScore> &scores);
};

} // namespace IRT2
