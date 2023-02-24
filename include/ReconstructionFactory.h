
#include <set>
#include <vector>

#include <TObject.h>
#include <TTree.h>
#include <TRandom.h>
class TH1D;
class TDatabasePDG;
class TParticlePDG;

class CherenkovEvent;
class CherenkovRadiator;
class CherenkovDetector;
#include <DigitizedHit.h>

#ifndef _RECONSTRUCTION_FACTORY_
#define _RECONSTRUCTION_FACTORY_

class ReconstructionFactory : public TObject {
 public:
  ReconstructionFactory(const char *dfname = 0, const char *cfname = 0, 
			const char *dname = 0);//, const char *rname = 0);
  ~ReconstructionFactory() {};

  void PerformCalibration( void );

  void IgnoreTimingInChiSquare( void )               { m_UseTimingInChiSquare = false; };
  void SetSingleHitCCDFcut(double value)             { m_SingleHitCCDFcut = value; };
  void RemoveAmbiguousHits( void )                   { m_ResolveHitOwnership = false; };
  void SetSinglePhotonTimingResolution(double value) { m_SinglePhotonTimingResolution = value; };
  void SetSensorActiveAreaPixellation(unsigned div)  { m_SensorActiveAreaPixellation = div; };
  CherenkovRadiator *UseRadiator(const char *rname, double smearing) {
    auto radiator = m_RICH->GetRadiator(rname);
    if (radiator) {
      m_Radiators.insert(radiator);
      radiator->SetGaussianSmearing(smearing);

      return radiator;
    } //if

    return 0;
  };
  //void SetThetaGaussianSmearing(double value)        { 
  //m_Radiator->SetGaussianSmearing(value);
  //};
  void SetQuietMode( void )                          { m_VerboseMode = false; };
  int AddHypothesis(int pdg);
  int AddHypothesis(const char *pdg);

  CherenkovEvent *GetNextEvent( void );
  unsigned GetEventCount( void ) const { return m_Tree ? m_Tree->GetEntries() : 0; };
  CherenkovEvent *GetEvent(unsigned ev);

  TH1D *hthph()   const { return m_hthph;   };
  TH1D *hccdfph() const { return m_hccdfph; };
  TH1D *hccdftr() const { return m_hccdftr; };
  TH1D *hccdfev() const { return m_hccdfev; };
  TH1D *hnpetr()  const { return m_hnpetr;  };
  TH1D *hdtph()   const { return m_hdtph;   };
  TH1D *hmatch()  const { return m_hmatch;  };
  
 private:
  TH1D *m_hthph, *m_hccdfph, *m_hccdftr, *m_hccdfev, *m_hnpetr, *m_hdtph, *m_hmatch;

  TRandom m_rndm;
  TDatabasePDG *m_DatabasePDG;

  TTree *m_Tree;

  //CherenkovRadiator *m_Radiator;
  std::set<CherenkovRadiator*> m_Radiators;
  CherenkovEvent *m_Event;
  CherenkovDetector *m_RICH;

  unsigned m_CurrentEvent;

  // Whether use hit timing or not in the chi^2 logic, see below;
  bool m_UseTimingInChiSquare;
  // Hits with chi^2 built in a 2D space (theta mismatch and timing mismatch) or 
  // just 1D space (theta mismatch only) will be accepted as "good" ones if 
  // the respective CCDF is more than this number;
  double m_SingleHitCCDFcut;

  // If true, keep at most one track owner for each hit; if false, delete 
  // hits, which would fit more than one track for a given combination of hypotheses;
  bool m_ResolveHitOwnership;

  double m_SinglePhotonTimingResolution;
  unsigned m_SensorActiveAreaPixellation;

  bool m_VerboseMode;

  std::vector<TParticlePDG*> m_Hypotheses;

  std::vector<DigitizedHit> m_Hits;

  ClassDef(ReconstructionFactory, 2)
};

#endif
