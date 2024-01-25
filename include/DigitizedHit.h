
#include <set>
#include <map>
#include <vector>

#include <TObject.h>
#include <TVector3.h>

#ifndef _DIGITIZED_HIT_
#define _DIGITIZED_HIT_

#include "CherenkovPhotonDetector.h"
#include "SinglePDF.h"
class ChargedParticle;
class IRT;

class DigitizedHitSolution {
 public:
 DigitizedHitSolution(): m_Best(0), m_thp(0.0), m_tt(0.0), m_ccdf(0.0), m_rbest(0) {};
  ~DigitizedHitSolution() {};

  // FIXME: very inefficient;
  CherenkovRadiator *GetRadiator( void ) {
    for(auto &ptr: m_All)
      if (&ptr.second == m_Best)
	return ptr.first.first;

    return 0;
  };

  void AssignVariables(IRTSolution *best, double thp, double tt, CherenkovRadiator *rptr, double ccdf) {
    m_Best  = best;
    m_thp   = thp;
    m_tt    = tt;
    m_rbest = rptr;
    m_ccdf  = ccdf;
  };

  std::map<std::pair<CherenkovRadiator*, IRT*>, IRTSolution> m_All;
  IRTSolution* m_Best;      

  double m_thp, m_tt, m_ccdf;
  CherenkovRadiator *m_rbest;
};

class DigitizedHit {
 public:
 DigitizedHit(): m_BackgroundCandidate(false), m_IRTs(0), m_PhotonDetector(0), 
    m_Copy(0), m_iX(0), m_iY(0) {};
  ~DigitizedHit() {
    m_Selected.clear();
  };

  double GetAverageDetectionTime( void ) const {
    double ret = 0.0;

    for(auto t: m_DetectionTimes)
      ret += t;

    return (m_DetectionTimes.size() ? ret / m_DetectionTimes.size() : 0.0);
  };
  double GetDetectionTime(unsigned ih) const { 
    return (ih < m_DetectionTimes.size() ? m_DetectionTimes[ih] : 0.0);
  };
  unsigned GetPhotonCount( void ) const { return m_DetectionTimes.size(); };

  inline const TVector3 &GetDetectionPosition( void )    const { return m_DetectionPosition; };

  // FIXME: private;
  bool m_BackgroundCandidate;                                     //!
  std::set<std::pair<ChargedParticle*, double>> m_Selected;       //!
  std::map<ChargedParticle*, DigitizedHitSolution> m_Solutions;   //!

  TVector3 m_DetectionPosition;                                   //!
  std::vector<TVector3> m_DirectionSeeds;                         //!
  std::vector<IRT*> *m_IRTs;                                      //!

  CherenkovPhotonDetector *m_PhotonDetector;                      //!
  unsigned m_Copy;                                                //!

  // Essentially should be unsigned values [0 .. dim-1];
  int m_iX, m_iY;                                                 //!

  std::vector<double> m_DetectionTimes;                           //!
};
#endif
