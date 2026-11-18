#pragma once

#include <set>
#include <map>
#include <vector>

#include <TObject.h>
#include <TVector3.h>

#include "CherenkovPhotonDetector.h"
#include "SinglePDF.h"

namespace IRT2 {

class ChargedParticle;
class IRT;

class DigitizedHitSolution {
 public:
 DigitizedHitSolution(): m_Best(0), m_thp(0.0), m_tt(0.0), m_ccdf(0.0), m_rbest(0),
    m_chi2(0.0), m_ndf(0), m_posterior(0.0) {};
  ~DigitizedHitSolution() {};

  // FIXME: very inefficient;
  CherenkovRadiator *GetRadiator( void ) {
    for(auto &ptr: m_All)
      if (&ptr.second == m_Best)
	return ptr.first.first;

    return 0;
  };

  // chi2/ndf of the winning [radiator, optical path] component are stored as well, so
  // track- and event-level CCDFs can be built a posteriori from the attached hits;
  void AssignVariables(IRTSolution *best, double thp, double tt, CherenkovRadiator *rptr,
                       double ccdf, double chi2 = 0.0, unsigned ndf = 0) {
    m_Best  = best;
    m_thp   = thp;
    m_tt    = tt;
    m_rbest = rptr;
    m_ccdf  = ccdf;
    m_chi2  = chi2;
    m_ndf   = ndf;
  };

  std::map<std::pair<CherenkovRadiator*, IRT*>, IRTSolution> m_All;
  IRTSolution* m_Best;      

  double m_thp, m_tt, m_ccdf;
  double m_chi2;                // single-photon chi^2 of the winning component   //!
  unsigned m_ndf;               // its number of degrees of freedom               //!
  double m_posterior;           // mixture posterior of this hit-to-track link    //!
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

  // FIXME: remove once EICrecon debugging is over;
  //TVector3 m_PhotonVertexPosition;                                //!
  //TVector3 m_PhotonVertexMomentum;                                //!
};

} // namespace IRT2
