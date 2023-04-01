
#include <map>

#ifndef _CHERENKOV_PHOTON_DETECTOR_
#define _CHERENKOV_PHOTON_DETECTOR_

#include "G4Object.h"
#include "ParametricSurface.h"

class G4DataInterpolation;

#include "IRT.h"

class CherenkovPhotonDetector: public G4Object {
 public:
 CherenkovPhotonDetector(G4VSolid *solid = 0, G4Material *material = 0):
  G4Object(solid, material), m_QERangeMin(0.0), m_QERangeMax(0.0), m_QE(0), 
    m_GeometricEfficiency(0.0) {};
  ~CherenkovPhotonDetector() {};

  void SetQE(double min, double max, const G4DataInterpolation *qe) { 
    m_QERangeMin = min; 
    m_QERangeMax = max; 

    m_QE = qe; 
  };

  inline bool CheckQERange(double e) const { return m_QE && e >= m_QERangeMin && e <= m_QERangeMax; };
  const G4DataInterpolation *GetQE( void ) const { return m_QE; };
  void SetGeometricEfficiency(double value) { m_GeometricEfficiency = value; };
  double GetGeometricEfficiency( void ) const { return m_GeometricEfficiency; };

  void AddItselfToOpticalBoundaries(IRT *irt, ParametricSurface *surface) /*const*/ {
    auto boundary = new OpticalBoundary(0, surface, true);
    irt->AddOpticalBoundary(boundary);

    m_OpticalBoundaryStorage.push_back(boundary);
  };
  IRT *AllocateIRT(unsigned sector, uint64_t icopy) { 
    auto irt = new IRT(sector); _m_IRT[icopy] = irt; return irt; 
  };
  IRT *GetIRT(uint64_t icopy) { return (_m_IRT.find(icopy) == _m_IRT.end() ? 0 : _m_IRT[icopy]); };

 private:
  // One optical path for each clone (identified by a logical volume copy);
  std::map<uint64_t, IRT*> _m_IRT;

  // IRT has a TRef by (unfortunate) design -> need a serialized storage buffer to refer to;
  std::vector<OpticalBoundary*> m_OpticalBoundaryStorage;

  // NB: the actual getQE() method can not be in this class since it will require linking 
  // against GEANT libraries; that's fine;
  double m_QERangeMin, m_QERangeMax;  //!
  const G4DataInterpolation *m_QE;    //!

  double m_GeometricEfficiency;

#ifndef DISABLE_ROOT_IO
  ClassDef(CherenkovPhotonDetector, 3);
#endif
};

#endif
