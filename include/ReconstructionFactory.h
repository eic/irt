
class TParticlePDG;

#include <Configuration.h>
#include <Calibration.h>
#include <Digitization.h>

#ifndef _RECONSTRUCTION_FACTORY_
#define _RECONSTRUCTION_FACTORY_

class ReconstructionFactory : /*public virtual GeantImport,*/ public Digitization, public Calibration {
 public:
  ReconstructionFactory(const char *dfname = 0, const char *cfname = 0, const char *dname = 0);
  ~ReconstructionFactory() {};

  void IgnoreTimingInChiSquare( void )               { m_UseTimingInChiSquare = false; };
  void IgnorePoissonTermInChiSquare( void )          { m_UsePoissonTermInChiSquare = false; };
  void SetSingleHitCCDFcut(double value)             { m_SingleHitCCDFcut = value; };
  void RemoveAmbiguousHits( void )                   { m_ResolveHitOwnership = false; };

  int AddHypothesis(int pdg);
  int AddHypothesis(const char *pdg);

  void SetExperimentalMode( void )                   { m_ExperimentalMode = true; };
  void IgnoreMcTruthPhotonDirectionSeed( void )      { m_UseMcTruthPhotonDirectionSeed = false; };

  CherenkovEvent *GetEvent(unsigned ev, bool calibration = false);

 private:
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

  bool m_ExperimentalMode, m_UseMcTruthPhotonDirectionSeed;

  std::vector<TParticlePDG*> m_Hypotheses;

  void LaunchRingFinder(bool calibration);
};

#endif
