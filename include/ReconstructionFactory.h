
#include <TH1D.h>
class TParticlePDG;
class TCanvas;

#include "Calibration.h"
#include "Digitization.h"

#ifndef _RECONSTRUCTION_FACTORY_
#define _RECONSTRUCTION_FACTORY_

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
  ReconstructionFactory(CherenkovDetectorCollection *geometry, CherenkovDetector *cdet, CherenkovEvent *event);
  virtual ~ReconstructionFactory() {
    if (m_Plots) delete m_Plots;
  };

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
  
 private:
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

  bool /*m_ExperimentalMode,*/ m_UseMcTruthPhotonDirectionSeed;

  std::vector<TParticlePDG*> m_Hypotheses;

  ReconstructionFactoryPlots  *m_Plots;      

  unsigned m_HitCountCutoff;
  unsigned m_ProcessedEventCount;
  
  bool BeVerbose( void )    const { return m_VerboseMode; };
  void LaunchRingFinder(bool calibration);
};

#endif
