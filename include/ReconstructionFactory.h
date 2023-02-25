
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
class CherenkovPhotonDetector;
#include <DigitizedHit.h>

#ifndef _RECONSTRUCTION_FACTORY_
#define _RECONSTRUCTION_FACTORY_

struct BlackoutCell {
BlackoutCell(CherenkovPhotonDetector *pd, unsigned icopy, unsigned iX, unsigned iY):
  m_PhotonDetector(pd), m_Copy(icopy), m_iX(iX), m_iY(iY) {};
  ~BlackoutCell() {};

  //bool operator>=(const BlackoutCell& other) const {
  //return m_PhotonDetector >= other.m_PhotonDetector;
  //}
  //bool operator<=(const BlackoutCell& other) const {
  //return m_PhotonDetector <= other.m_PhotonDetector;
  //}
  //bool operator==(const BlackoutCell& other) const {
  //return m_PhotonDetector == other.m_PhotonDetector;
  //}
  //bool operator!=(const BlackoutCell& other) const {
  //return m_PhotonDetector != other.m_PhotonDetector;
  //}
  //bool operator>(const BlackoutCell& other) const {
  //return m_PhotonDetector > other.m_PhotonDetector;
  //}
  bool operator<(const BlackoutCell& other) const {
    return 
      (m_PhotonDetector <  other.m_PhotonDetector) ||
      (m_PhotonDetector == other.m_PhotonDetector && m_Copy <  other.m_Copy) ||
      (m_PhotonDetector == other.m_PhotonDetector && m_Copy == other.m_Copy && m_iX  < other.m_iX) ||
      (m_PhotonDetector == other.m_PhotonDetector && m_Copy == other.m_Copy && m_iX == other.m_iX && m_iY < other.m_iY);
  }

  CherenkovPhotonDetector *m_PhotonDetector;
  unsigned m_Copy, m_iX, m_iY;
};

class ReconstructionFactory : public TObject {
 public:
  ReconstructionFactory(const char *dfname = 0, const char *cfname = 0, 
			const char *dname = 0);//, const char *rname = 0);
  ~ReconstructionFactory() {};

  void PerformCalibration(unsigned stat = 0);

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

  CherenkovRadiator *AddBlackoutRadiator(const char *rname) {
    auto radiator = m_RICH->GetRadiator(rname);

    if (radiator) m_BlackoutRadiators.insert(radiator);
    
    return radiator;
  };
  void SetBlackoutBlowupValue(unsigned value) { m_BlackoutBlowupValue = value; };
  void UseAutomaticCalibration( void )        { m_UseAutomaticCalibration = true; };

  void SetQuietMode( void )                          { m_VerboseMode = false; };
  int AddHypothesis(int pdg);
  int AddHypothesis(const char *pdg);

  CherenkovEvent *GetNextEvent(bool calibration = false);
  unsigned GetEventCount( void ) const { return m_Tree ? m_Tree->GetEntries() : 0; };
  CherenkovEvent *GetEvent(unsigned ev, bool calibration = false);

  unsigned GetDigitizedHitCount( void )                     const { return m_Hits.size(); };
  const std::vector<DigitizedHit> &GetDigitizedHits( void ) const { return m_Hits; };
  const DigitizedHit *GetDigitizedHit(unsigned id)          const { 
    return (id < m_Hits.size() ? &m_Hits[id] : 0);
  };

  TH1D *hthph()   const { return m_hthph;   };
  TH1D *hcalib()  const { return m_hcalib;   };
  TH1D *hccdfph() const { return m_hccdfph; };
  TH1D *hccdftr() const { return m_hccdftr; };
  TH1D *hccdfev() const { return m_hccdfev; };
  TH1D *hnpetr()  const { return m_hnpetr;  };
  TH1D *hdtph()   const { return m_hdtph;   };
  TH1D *hmatch()  const { return m_hmatch;  };
  
 private:
  TH1D *m_hthph, *m_hcalib, *m_hccdfph, *m_hccdftr, *m_hccdfev, *m_hnpetr, *m_hdtph, *m_hmatch;

  TRandom m_rndm;
  TDatabasePDG *m_DatabasePDG;

  TTree *m_Tree;

  std::set<CherenkovRadiator*> m_Radiators;

  // Radiator pointer and summing up (assume 2^n) scaling factor on the sensor plane;
  std::set<CherenkovRadiator*> m_BlackoutRadiators;
  // FIXME: this can be done more efficiently (using cell-to-cell distance); for now just 
  // shut down all hits which fall into the same cell as a "background" radiator, at 
  // 2^n larger macro-pad;
  unsigned m_BlackoutBlowupValue;

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

  bool m_VerboseMode, m_UseAutomaticCalibration;

  std::vector<TParticlePDG*> m_Hypotheses;

  std::set<BlackoutCell> m_BlackoutCells; //!
  std::vector<DigitizedHit> m_Hits;

  ClassDef(ReconstructionFactory, 2)
};

#endif
