
#ifndef _CHERENKOV_PHOTON_DETECTOR_
#define _CHERENKOV_PHOTON_DETECTOR_

#include <G4Object.h>
#include <ParametricSurface.h>

class G4DataInterpolation;

#include <IRT.h>

class CherenkovPhotonDetector: public G4Object {
 public:
 CherenkovPhotonDetector(G4VSolid *solid = 0, G4Material *material = 0, ParametricSurface *surface = 0): 
  G4Object(solid, material), m_OpticalBoundary(0), m_QERangeMin(0.0), m_QERangeMax(0.0), m_QE(0), 
    m_GeometricEfficiency(0.0) { 
    m_IRT = new IRT(); 

    // The "0" implicitly says "there is no photon life beyond this boundary in IRT"; 
    // FIXME: will have to add the finite thickness window material as well;
    m_OpticalBoundary = new OpticalBoundary(0, surface, true);
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

  void AddItselfToOpticalBoundaries( void ) const {
    m_IRT->AddOpticalBoundary(m_OpticalBoundary);
  };
  IRT *GetIRT( void ) const { return m_IRT; };

 private:
  // FIXME: need to return back the ability to have more than one optical path 
  // per photosensor (e.g. for a dual mirror setup, with potentially overlapping 
  // focal areas and / or for decay-like particles which do not originate in the 
  // IP area);
  IRT *m_IRT;

  OpticalBoundary *m_OpticalBoundary;

  // NB: the actual getQE() method can not be in this class since it will require linking 
  // against GEANT libraries; that's fine;
  double m_QERangeMin, m_QERangeMax;  //!
  const G4DataInterpolation *m_QE;    //!

  double m_GeometricEfficiency;

  ClassDef(CherenkovPhotonDetector, 1);
};

#endif
