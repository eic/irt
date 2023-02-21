
#include <set>
#include <map>
#include <vector>

#include <TObject.h>
#include <TVector3.h>

#ifndef _DIGITIZED_HIT_
#define _DIGITIZED_HIT_

#include <CherenkovPhotonDetector.h>
#include <SinglePDF.h>
class ChargedParticle;
class IRT;

class DigitizedHit: public TObject {
 public:
 DigitizedHit(): m_BackgroundCandidate(false), /*m_ExpectedTime(0.0),*/ m_Best(0), m_IRTs(0), 
    m_PhotonDetector(0), m_Copy(0), m_iX(0), m_iY(0) {};
  ~DigitizedHit() {
    m_Selected.clear();

    //for(auto tag: m_PDF)
    //tag.second.first.Reset();
    //m_PDF.clear();
  };

  double GetAverageDetectionTime( void ) {
    double ret = 0.0;
    for(auto t: m_DetectionTimes)
      ret += t;

    return ret;
  };

  inline const TVector3 &GetDetectionPosition( void )    const { return m_DetectionPosition; };

  // FIXME: private;
  bool m_BackgroundCandidate;                                     //!
  std::set<std::pair<ChargedParticle*, double>> m_Selected;       //!
  //std::map<std::pair<CherenkovRadiator*, IRT*>, std::pair<VectorPDF, double>> m_PDF; //!
  //std::map<std::pair<CherenkovRadiator*, IRT*>, std::pair<double, double>> m_PDF; //!
  std::map<std::pair<CherenkovRadiator*, IRT*>, IRTSolution> m_Solutions; //!
  //std::map<std::pair<CherenkovRadiator*, IRT*>, double> m_ExpectedTime; //!
  //double m_ExpectedTime;                                          //!
  IRTSolution *m_Best;                                            //!

  TVector3 m_DetectionPosition;                                   //!
  std::vector<TVector3> m_DirectionSeeds;                         //!
  std::vector<IRT*> *m_IRTs;                                      //!

  CherenkovPhotonDetector *m_PhotonDetector;                      //!
  unsigned m_Copy;                                                //!

  // Essentially should be unsigned values [0 .. dim-1];
  int m_iX, m_iY;                                                 //!

  std::vector<double> m_DetectionTimes;                           //!

  ClassDef(DigitizedHit, 1);
};
#endif
