
#include <TRandom.h>

#ifndef _DIGITIZATION_
#define _DIGITIZATION_

#include "DigitizedHit.h"
#include "Configuration.h"
class CherenkovPhotonDetector;

struct BlackoutCell {
BlackoutCell(CherenkovPhotonDetector *pd, unsigned icopy, unsigned iX, unsigned iY):
  m_PhotonDetector(pd), m_Copy(icopy), m_iX(iX), m_iY(iY) {};
  ~BlackoutCell() {};

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

class Digitization : /*public virtual GeantImport,*/ public virtual Configuration {
 public:
  Digitization();
  ~Digitization() {};

  unsigned GetDigitizedHitCount( void )                     const { return m_Hits.size(); };
  const std::vector<DigitizedHit> &GetDigitizedHits( void ) const { return m_Hits; };
  const DigitizedHit *GetDigitizedHit(unsigned id)          const { 
    return (id < m_Hits.size() ? &m_Hits[id] : 0);
  };

  void SetBlackoutBlowupValue(unsigned value) { m_BlackoutBlowupValue = value; };

  CherenkovRadiator *AddBlackoutRadiator(const char *rname);
  void SetSensorActiveAreaPixellation(unsigned div)  { m_SensorActiveAreaPixellation = div; };
  void SetSinglePhotonTimingResolution(double value) { m_SinglePhotonTimingResolution = value; };

 protected:
  std::vector<DigitizedHit> &Hits( void ) { return m_Hits; };
  void ProduceDigitizedHits(bool calibration);
  void ClearBlackoutCells( void ) { m_BlackoutCells.clear(); };
  void ClearDigitizedHits( void ) { m_Hits.clear(); }
  double GetSinglePhotonTimingResolution( void ) const { return m_SinglePhotonTimingResolution; };

 private:
  TRandom m_rndm;                                   
  std::vector<DigitizedHit> m_Hits;                 

  // Radiator pointer and summing up (assume 2^n) scaling factor on the sensor plane;
  std::set<CherenkovRadiator*> m_BlackoutRadiators;
  // FIXME: this can be done more efficiently (using cell-to-cell distance); for now just 
  // shut down all hits which fall into the same cell as a "background" radiator, at 
  // 2^n larger macro-pad;
  unsigned m_BlackoutBlowupValue;

  std::set<BlackoutCell> m_BlackoutCells;    

  double m_SinglePhotonTimingResolution;
  unsigned m_SensorActiveAreaPixellation;

  //double 
};

#endif
