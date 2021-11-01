
#include <map>

#ifndef _CHERENKOV_PHOTON_DETECTOR_
#define _CHERENKOV_PHOTON_DETECTOR_

#include <G4Object.h>
#include <ParametricSurface.h>

class G4DataInterpolation;

#include <IRT.h>

class CherenkovPhotonDetector: public G4Object {
 public:
 CherenkovPhotonDetector(G4VSolid *solid = 0, G4Material *material = 0)://, ParametricSurface *surface = 0): 
  G4Object(solid, material), /*m_IRT(0),*/ /*m_OpticalBoundary(0),*/ /*mQ(0),*/ m_QERangeMin(0.0), m_QERangeMax(0.0), m_QE(0), 
    m_GeometricEfficiency(0.0) { 
    //m_IRT = new IRT(); 

    //_m_IRT[0] = new IRT(); 

    // The "0" implicitly says "there is no photon life beyond this boundary in IRT"; 
    // FIXME: will have to add the finite thickness window material as well;
    //_m_OpticalBoundary = new OpticalBoundary(0, surface, true);
  };
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
    //m_IRT->AddOpticalBoundary(_m_OpticalBoundary);
    irt->AddOpticalBoundary(boundary);//new OpticalBoundary(0, surface, true));//_m_OpticalBoundary);
    //_m_IRT[0]->AddOpticalBoundary(m_OpticalBoundary);
    m_OpticalBoundaryStorage.push_back(boundary);
    //m_OpticalBoundary = boundary;
    //mQ = boundary;
  };
  //IRT *GetIRT( void ) const { return m_IRT; };
  //IRT *AllocateIRT(unsigned icopy) { m_IRT = new IRT(); return m_IRT; };
  IRT *AllocateIRT(unsigned icopy) { auto irt = new IRT(); _m_IRT[icopy] = irt; return irt; };
  IRT *GetIRT(unsigned icopy) { return (_m_IRT.find(icopy) == _m_IRT.end() ? 0 : _m_IRT[icopy]); };

 private:
  // FIXME: need to return back the ability to have more than one optical path 
  // per photosensor (e.g. for a dual mirror setup, with potentially overlapping 
  // focal areas and / or for decay-like particles which do not originate in the 
  // IP area);
  //IRT *m_IRT;
  std::map<unsigned, IRT*> _m_IRT;

  //OpticalBoundary *mQ;
  // IRT has a TRef by (unfortunate) design -> need a serialized storage buffer to refer to;
  std::vector<OpticalBoundary*> m_OpticalBoundaryStorage;
  //OpticalBoundary *m_OpticalBoundary;//, *mQ;
  //OpticalBoundary *m_OpticalBoundary;//, *mQ;

  // NB: the actual getQE() method can not be in this class since it will require linking 
  // against GEANT libraries; that's fine;
  double m_QERangeMin, m_QERangeMax;  //!
  const G4DataInterpolation *m_QE;    //!

  double m_GeometricEfficiency;

  ClassDef(CherenkovPhotonDetector, 2);
};

#endif
