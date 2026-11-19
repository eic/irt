#pragma once

#ifdef JSON_IMPORT_EXPORT
#include <nlohmann/json.hpp>
#endif

#include <TH1D.h>
class TParticlePDG;
class TCanvas;
class TFile;

#include <vector>
#include <limits>
#include <map>
#include <array>

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
  // How the initial photon direction for the IRT iteration is chosen:
  //   "mc-truth"        - the direction from the MC truth photon vertex momentum (the
  //                       old "yes"; an upper bound, not a reconstruction option);
  //   "track-direction" - no seed at all, the iteration starts from the flat prior
  //                       (the old "no");
  //   "plane-tangent"   - seed from the specular point of the first mirror, obtained by
  //                       iterating the tangent-plane reflection (any mirror shape);
  //   "unrolled-cone"   - the same, started from several windings of the development of
  //                       the cone, so the additional specular (and multi-bounce)
  //                       solutions become reachable. Requires conical mirrors, i.e. it
  //                       is not available for dRICH;
  enum PhotonDirectionSeed { kSeedMcTruth, kSeedTrackDirection, kSeedPlaneTangent,
                             kSeedUnrolledCone };
  void SetPhotonDirectionSeed( PhotonDirectionSeed mode ) { m_PhotonDirectionSeed = mode; };
  PhotonDirectionSeed GetPhotonDirectionSeed( void ) const {
    return (PhotonDirectionSeed)m_PhotonDirectionSeed;
  };
  bool McTruthSeedsUsed( void ) const { return m_PhotonDirectionSeed == kSeedMcTruth; };

  CherenkovEvent *GetEvent(unsigned ev, bool calibration = false);

  void SetQuietMode( void )       { m_VerboseMode = false; };
  
  // Multi-particle case;
  void ProcessHits(ChargedParticle *mcparticle, std::vector<DigitizedHit> &hits);

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
  // Expected number of background photon hits per event (a fixed input, the same
  // quantity the chi^2 path knows as _EXPECTED_BG_PHOTON_COUNT_);
  void SetExpectedBackgroundPhotons( double value ) { m_ExpectedBackgroundPhotons = value; };
  double GetExpectedBackgroundPhotons( void ) const { return m_ExpectedBackgroundPhotons; };

  // How the a posteriori (MAP) hit-to-track attachment decides that a hit is signal:
  //   majority  - the winning track must carry more than 1/2 of the total posterior
  //               (the behaviour of v2.2.3a5..a7: with several competing tracks an
  //               ambiguous 0.4/0.4/0.2 hit is thrown away as background);
  //   plurality - the winning track must simply carry more than the background
  //               component, i.e. the hit is more likely signal than background. This
  //               keeps genuinely claimed-by-nobody hits out while not punishing
  //               ambiguity between tracks.
  enum AssignmentMode { kAssignMajority = 0, kAssignPlurality = 1 };
  void SetAssignmentMode( AssignmentMode mode ) { m_AssignmentMode = mode; };
  AssignmentMode GetAssignmentMode( void ) const { return (AssignmentMode)m_AssignmentMode; };

  // Expectation-maximization control of the mixture estimator (Stage B3);
  void SetEmMaxIterations( unsigned value )  { m_EmMaxIterations = value; };
  void SetEmTolerance( double value )        { m_EmTolerance = value; };
  void UseEmCalibration( bool value = true ) { m_EmFitCalibration = value; };

  // Fill the single-photon, track- and event-level CCDF plots a posteriori, i.e. from the
  // final hit-to-track assignment, instead of during the combination scan (default: yes);
  void UsePosteriorCCDFPlots( bool value = true ) { m_PosteriorCCDFPlots = value; };
  unsigned GetEmIterationsUsed( void ) const { return m_EmIterationsUsed; };
  
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
  double m_BackgroundWindow, m_ExpectedBackgroundPhotons;

  // Expectation-maximization control of the mixture estimator: iteration cap, the 
  // absolute improvement of the objective below which the iterations stop, and which 
  // parameters are actually fitted -- the per-[radiator,theta bin] angular offset and 
  // resolution (off by default, as this changes the calibration constants which the 
  // reconstruction itself uses), the background fraction, and the per-radiator yield 
  // scale of the event being reconstructed;
  unsigned m_EmMaxIterations, m_EmIterationsUsed;
  double m_EmTolerance;
  bool m_EmFitCalibration;
  unsigned m_AssignmentMode;
  bool m_PosteriorCCDFPlots;                                                //!

  unsigned m_PhotonDirectionSeed;                                               //!

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

  //
  // Stage B1: the same idea for the mixture (maximum-likelihood) estimator.
  //

  // One [hit, radiator, optical path] component of one particle, with everything the 
  // mixture needs -- and which does not depend on the PID hypothesis -- resolved once: 
  // radiator, IRT solution, calibration bin, momentum and path length inside the 
  // radiator, reference refractive index, angular resolution and offset;
  struct MixComponent { unsigned m_Ip, m_Ibin; CherenkovRadiator *m_Radiator;
    IRTSolution *m_Solution; double m_Pp, m_Ll, m_Nref, m_Hsigma, m_Coffset,
      m_Theta, m_Time, m_Tmeas;
    MixComponent(): m_Ip(0), m_Ibin(0), m_Radiator(0), m_Solution(0), m_Pp(0.0), m_Ll(0.0),
      m_Nref(0.0), m_Hsigma(0.0), m_Coffset(0.0), m_Theta(0.0), m_Time(0.0), m_Tmeas(0.0) {};
  };

  // Aggregation of the components of one [particle,hit] pair under one PID hypothesis: 
  // the log-sum-exp of the component log-densities and the density-weighted mean of the 
  // Cherenkov angles. `m_LnSum` is -inf when the pair has no usable component. The 
  // remaining moments of the *measured* and *predicted* Cherenkov angles are needed for 
  // the optional calibration M-step of Stage B3; they are all normalized by the same 
  // sum of densities as `m_ThBar`;
  struct MixAgg { double m_LnSum, m_ThBar, m_ThSolBar, m_ThSol2, m_ThSolP, m_ThP2;
    MixAgg(): m_LnSum(-std::numeric_limits<double>::infinity()), m_ThBar(0.0),
      m_ThSolBar(0.0), m_ThSol2(0.0), m_ThSolP(0.0), m_ThP2(0.0) {};
  };

  // Caches for one iteration, filled particle by particle: components are indexed 
  // [ip*nhits + ihit], aggregates [(ip*nhits + ihit)*hdim + ih];
  struct MixtureCache { std::vector<MixComponent> m_Comps;
    std::vector<unsigned> m_Beg, m_Num; std::vector<MixAgg> m_Agg;
    unsigned m_Pdim, m_Nhits, m_Hdim;
    MixtureCache(): m_Pdim(0), m_Nhits(0), m_Hdim(0) {};
    void Reset(unsigned pdim, unsigned nhits, unsigned hdim) {
      m_Pdim = pdim; m_Nhits = nhits; m_Hdim = hdim;
      m_Comps.clear(); m_Beg.assign(pdim*nhits, 0); m_Num.assign(pdim*nhits, 0);
      m_Agg.assign(pdim*nhits*hdim, MixAgg());
    } //Reset
  };

  // Log-density of one component under one PID hypothesis, together with the kinematics 
  // the assignment step needs; returns false when no Cherenkov light is expected for 
  // this [momentum, mass, refractive index] combination;
  bool MixtureLnD(const MixComponent &comp, unsigned ih, double &lnd, double &thp,
                  double &tt, double &thdiff, double &tmdiff) const;

  // Fill the component list and the per-hypothesis aggregates for one particle; the 
  // mixture objective is then a sum over hits of a log-sum-exp over cached aggregates, 
  // i.e. it is separable and independent of the [particle,hypothesis] combination;
  void ScoreMixture(ChargedParticle *mcparticle, unsigned ip, bool calibration,
                    MixtureCache &cache);

  // Fill the existing CCDF plots (ccdfph per photon, ccdftr per track, ccdfev per event)
  // from the final hit-to-track attachment rather than during the combination scan;
  void FillPosteriorCCDFPlots( void );

  // Parameters of the per-hit mixture that expectation-maximization actually
  // fits: the hypothesis weights pi_{p,h}, pdim*hdim of them, normalized to 1 for
  // each particle. Everything else is fixed input -- the background is a configured
  // expected photon count, and the photon yield of a radiator comes from the
  // calibration file ("YieldCff"), i.e. the average number of detected photons per
  // track at a saturated Cherenkov angle;
  struct EmParameters { std::vector<double> m_Pi; };

  // Global mixture log-likelihood evaluated from the caches above, with the PID 
  // hypothesis promoted to a mixture-component index: the assignment and the 
  // hypothesis choice are both marginalized (sum inside the log) rather than resolved 
  // a priori. When `mstep` is set, the responsibilities also update the parameters 
  // (maximization steps); when `assign` is set, every hit is attached to its most 
  // probable track, i.e. the hit-to-track assignment is performed a posteriori;
  double MixtureLnLWeighted(bool calibration, const MixtureCache &cache,
                            EmParameters &par, bool assign, bool mstep,
                            std::map<std::pair<CherenkovRadiator*,unsigned>,
                                      std::array<double,3>> *calib = 0);

  // Stage B3: expectation-maximization iterations for the mixture estimator. The 
  // expectation step is the separable objective above; the maximization steps update 
  // the hypothesis weights of every track, the background fraction and the per-
  // radiator yield scales of this event (and, when enabled, the per-[radiator,theta 
  // bin] angular offset and resolution, which forces the component caches to be 
  // rebuilt); iterated until the objective stops improving;
  double RunEM(bool calibration, MixtureCache &cache, EmParameters &par);

  // Rebuild the whole component/aggregate cache of the current event (needed when the 
  // calibration constants used by the mixture change);
  void RebuildMixtureCache(MixtureCache &cache, bool calibration);

  // Write the calibration constants accumulated by one maximization step back into the 
  // radiators;
  void ApplyCalibrationMStep(const std::map<std::pair<CherenkovRadiator*,unsigned>,
                                            std::array<double,3>> &sums);
};

} // namespace IRT2
