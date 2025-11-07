
#include <map>

#ifndef _CHERENKOV_PHOTON_DETECTOR_
#define _CHERENKOV_PHOTON_DETECTOR_

#include "G4Object.h"
#include "FlatSurface.h"

class G4DataInterpolation;

#include "IRT.h"

namespace IRT2 {

class CherenkovPhotonDetector: public G4Object {
 public:
 CherenkovPhotonDetector(G4VSolid *solid = 0, G4Material *material = 0):
  G4Object(solid, material), m_QERangeMin(0.0), m_QERangeMax(0.0), m_QE(0), m_ScaleFactor(1.0),
    m_GeometricEfficiency(0.0), m_CopyIdentifierLevel(0) {};
  ~CherenkovPhotonDetector() {};

  void SetQE(double min, double max, /*const*/ G4DataInterpolation *qe, double scale = 1.0) { 
    m_QERangeMin = min; 
    m_QERangeMax = max; 

    m_QE = qe; 
    m_ScaleFactor = scale;
  };

  inline bool CheckQERange(double e) const { return m_QE && e >= m_QERangeMin && e <= m_QERangeMax; };
  /*const*/ G4DataInterpolation *GetQE( void ) const { return m_QE; };
  double GetScaleFactor( void ) const { return m_ScaleFactor; };
  void SetGeometricEfficiency(double value) { m_GeometricEfficiency = value; };
  double GetGeometricEfficiency( void ) const { return m_GeometricEfficiency; };

  void SetActiveAreaSize(double size) { m_ActiveAreaSize = size; };
  double GetActiveAreaSize( void ) const { return m_ActiveAreaSize; };

  void SetCopyIdentifierLevel(unsigned lv) { m_CopyIdentifierLevel = lv; };
  unsigned GetCopyIdentifierLevel( void ) const { return m_CopyIdentifierLevel; };

  void AddItselfToOpticalBoundaries(IRT *irt, ParametricSurface *surface) /*const*/ {
    auto boundary = new OpticalBoundary(0, surface, true);
    irt->AddOpticalBoundary(boundary);

    m_OpticalBoundaryStorage.push_back(boundary);
  };
  IRT *AllocateIRT(unsigned sector, uint64_t icopy) { 
    auto irt = new IRT(/*sector*/); 

    if (m_IRT.find(std::make_pair(sector, icopy)) == m_IRT.end()) {
      std::vector<IRT*> ret;
      ret.push_back(irt);
      m_IRT[std::make_pair(sector,icopy)] = ret;
    } else 
      m_IRT[std::make_pair(sector,icopy)].push_back(irt);

    return irt; 
  };
  // FIXME: should be (sector,icopy) if ever used again;
  //IRT *GetIRT(uint64_t icopy) { return (m_IRT.find(icopy) == m_IRT.end() ? 0 : m_IRT[icopy][0]); };
  //std::vector<IRT*> *GetIRTs(uint64_t icopy) { 
  //return (m_IRT.find(icopy) == m_IRT.end() ? 0 : &m_IRT[icopy]); 
  //};
  std::vector<IRT*> *GetIRTs(unsigned sector, uint64_t icopy) { 
    return (m_IRT.find(std::make_pair(sector,icopy)) == m_IRT.end() ? 0 :
	    &m_IRT[std::make_pair(sector, icopy)]); 
  };

 private:
  // One optical path for each clone (identified by a logical volume copy);
  //std::map<uint64_t, std::vector<IRT*>> m_IRT;
  std::map<std::pair<unsigned,uint64_t>, std::vector<IRT*>> m_IRT;

  // IRT has a TRef by (unfortunate) design -> need a serialized storage buffer to refer to;
  std::vector<OpticalBoundary*> m_OpticalBoundaryStorage;

  // NB: the actual getQE() method can not be in this class since it will require linking 
  // against GEANT libraries; that's fine;
  double m_QERangeMin, m_QERangeMax;  //!
  /*const*/ G4DataInterpolation *m_QE;    //!
  // May want to renormalize the QE curve, for instance to peak at 100%, but keep the 
  // QE(lambda) dependency; also helps to select undetected photons, which follow the 
  // QE(lambda) curve (so are representative for e.g. <n> calculation), but do not pass 
  // the overall (uniform) efficiency test, see CherenkovSteppingAction::UserSteppingAction();
  double m_ScaleFactor;               //!

  double m_GeometricEfficiency;

  // In case a squared flat surface;
  double m_ActiveAreaSize;

  unsigned m_CopyIdentifierLevel;     //!

#ifndef DISABLE_ROOT_IO
  ClassDef(CherenkovPhotonDetector, 7);
#endif
};

} // namespace IRT2

#endif
