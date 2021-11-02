
#include <map>
#include <vector>

#include <TVector3.h>

#ifndef _OPTICAL_PHOTON_
#define _OPTICAL_PHOTON_

#include <TransientParticle.h>
#include <CherenkovPhotonDetector.h>
class ReflectionPoint;
#include <RefractionPoint.h>
#include <SinglePDF.h>

class OpticalPhoton: public TransientParticle {
 public:
 OpticalPhoton(): TransientParticle(0), m_VertexRefractiveIndex(0.0), m_PhotonDetector(0), 
    m_VolumeCopy(0), m_DetectionTime(0.0), 
    m_Detected(false) {};
  ~OpticalPhoton() {};
  
  inline bool IsCharged( void )                          const { return false; };

  inline void SetDetected(bool what = true)                    { m_Detected = what; };
  inline void SetDetectionTime(double value)                   { m_DetectionTime = value; };
  inline void SetVertexRefractiveIndex(double value)           { m_VertexRefractiveIndex = value; };
  inline void SetVertexPosition(const TVector3 &position)      { m_VertexPosition = position; };
  inline void SetVertexMomentum(const TVector3 &momentum)      { m_VertexMomentum = momentum; };
  inline void SetVertexParentMomentum(const TVector3 &momentum){ m_VertexParentMomentum = momentum; };
  inline void SetVolumeCopy(unsigned copy)                     { m_VolumeCopy = copy; };
  inline void SetDetectionPosition(const TVector3 &position)   { m_DetectionPosition = position; };
  inline void SetPhotonDetector(CherenkovPhotonDetector *pd)   { m_PhotonDetector = pd; };

  inline void AddReflectionPoint(ReflectionPoint *reflection)  {
    m_ReflectionPoints.push_back(reflection);
  };
  unsigned ReflectionPointCount( void )                  const { return m_ReflectionPoints.size(); };
  const ReflectionPoint *GetReflectionPoint(unsigned id) const { 
    return (id < m_ReflectionPoints.size() ? m_ReflectionPoints[id] : 0);
  };
  inline void AddRefractionPoint(const RefractionPoint *refraction) {
    m_RefractionPoints.push_back(refraction);
  };
  unsigned RefractionPointCount( void )                  const { return m_RefractionPoints.size(); };
  const RefractionPoint *GetRefractionPoint(unsigned id) const { 
    return (id < m_RefractionPoints.size() ? m_RefractionPoints[id] : 0);
  };

  inline double GetVertexRefractiveIndex( void )         const { return m_VertexRefractiveIndex; };
  inline const TVector3 &GetVertexPosition( void )       const { return m_VertexPosition; };
  inline const TVector3 &GetVertexMomentum( void )       const { return m_VertexMomentum; };
  inline const TVector3 &GetVertexParentMomentum( void ) const { return m_VertexParentMomentum; };
  inline const TVector3 &GetDetectionPosition( void )    const { return m_DetectionPosition; };

  inline bool WasDetected( void )                        const { return m_Detected; };
  inline unsigned GetVolumeCopy( void )                  const { return m_VolumeCopy; };

  inline CherenkovPhotonDetector *GetPhotonDetector( void ) const {
    return dynamic_cast<CherenkovPhotonDetector*>(m_PhotonDetector.GetObject());
  };

 private:
  // Vertex and 3D at birth as given by GEANT; 
  TVector3 m_VertexPosition, m_VertexMomentum, m_VertexParentMomentum;

  // Refractive index for this wave length (if radiator was known);
  double m_VertexRefractiveIndex;

  // Reflection points on the mirrors; 
  std::vector<ReflectionPoint*> m_ReflectionPoints;
  // Refraction points on the radiator boundaries;
  std::vector<const RefractionPoint*> m_RefractionPoints;

  // Photon detector, where photon ended up; volume copy number; position and time;
  TRef m_PhotonDetector;
  unsigned m_VolumeCopy;
  TVector3 m_DetectionPosition;
  double m_DetectionTime;

  // 'true' if the photon was actually detected; QE-based and geometric efficiency accounted; 
  bool m_Detected;

  // Transient variables for some convenience in an analysis script;
 public:
  //IRTSolution m_Solution;  //!
  std::map<CherenkovRadiator*, VectorPDF> _m_PDF;           //!

  ClassDef(OpticalPhoton, 2);
};

#endif
