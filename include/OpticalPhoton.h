
#include <set>
#include <map>
#include <vector>

#include <TVector3.h>

#ifndef _OPTICAL_PHOTON_
#define _OPTICAL_PHOTON_

#include "TransientParticle.h"
#include "CherenkovPhotonDetector.h"
class ReflectionPoint;
class ChargedParticle;
#include "RefractionPoint.h"
#include "SinglePDF.h"

class OpticalPhoton: public TransientParticle {
 public:
 OpticalPhoton(): TransientParticle(0), m_VertexAttenuationLength(0.0), 
    m_VertexRefractiveIndex(0.0), m_PhotonDetector(0), m_VolumeCopy(0), m_DetectionTime(0.0), 
    m_Detected(false), m_CalibrationFlag(false), m_BlackoutFlag(false) {};
  ~OpticalPhoton() {};
  
  inline bool IsCharged( void )                          const { return false; };

  inline void SetDetected(bool what = true)                    { m_Detected = what; };
  inline void SetCalibrationFlag( void )                       { m_CalibrationFlag = true; };
  inline void SetBlackoutFlag( void )                          { m_BlackoutFlag = true; };
  inline void SetDetectionTime(double value)                   { m_DetectionTime = value; };
  inline void SetVertexAttenuationLength(double value)         { m_VertexAttenuationLength = value; };
  inline void SetVertexRefractiveIndex(double value)           { m_VertexRefractiveIndex = value; };
  inline void SetVertexParentMomentum(const TVector3 &momentum){ m_VertexParentMomentum = momentum; };
  inline void SetVolumeCopy(uint64_t copy)                     { m_VolumeCopy = copy; };
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

  inline double GetVertexAttenuationLength( void )       const { return m_VertexAttenuationLength; };
  inline double GetVertexRefractiveIndex( void )         const { return m_VertexRefractiveIndex; };
  inline double GetDetectionTime( void )                 const { return m_DetectionTime; };
  inline const TVector3 &GetVertexParentMomentum( void ) const { return m_VertexParentMomentum; };
  inline const TVector3 &GetDetectionPosition( void )    const { return m_DetectionPosition; };

  inline bool WasDetected( void )                        const { return m_Detected; };
  inline bool IsUsefulForCalibration( void )             const { return m_CalibrationFlag; };
  inline bool IsMarkedAsBlackout( void )                 const { return m_BlackoutFlag; };
  inline uint64_t GetVolumeCopy( void )                  const { return m_VolumeCopy; };

  inline CherenkovPhotonDetector *GetPhotonDetector( void ) const {
    return dynamic_cast<CherenkovPhotonDetector*>(m_PhotonDetector.GetObject());
  };

  void StoreRefractiveIndex(double ri) {
    m_RefractiveIndices.push_back(ri);
  };
  const std::vector<double> &StoredRefractiveIndices( void )   const { return m_RefractiveIndices; };
  
 private:
  // 3D momentum vector of a parent particle at the photon production vertex location;
  TVector3 m_VertexParentMomentum;

  // Attenuation length for this wave length (if radiator was known); 
  double m_VertexAttenuationLength;

  // Refractive index for this wave length (if radiator was known);
  double m_VertexRefractiveIndex;

  // Reflection points on the mirrors; 
  std::vector<ReflectionPoint*> m_ReflectionPoints;
  // Refraction points on the radiator boundaries;
  std::vector<const RefractionPoint*> m_RefractionPoints;

  // Photon detector, where photon ended up; volume copy number; position and time;
  TRef m_PhotonDetector;
  uint64_t m_VolumeCopy;
  TVector3 m_DetectionPosition;
  double m_DetectionTime;

  // 'true' if the photon was actually detected; QE-based and geometric efficiency accounted; 
  bool m_Detected, m_CalibrationFlag, m_BlackoutFlag;

  // For all known radiators, for a completeness;
  std::vector<double> m_RefractiveIndices;
  
  // Transient variables for some convenience in an analysis script;
 public:
  std::set<std::pair<unsigned, CherenkovRadiator*>> _m_Selected;  //!
  // FIXME: this one is obsolete;
  std::map<CherenkovRadiator*, VectorPDF> _m_PDF;                 //!

  // Average estimated phi angle; no need to know it precisely (?);
  std::map<CherenkovRadiator*, double> m_Phi;                     //!

#ifndef DISABLE_ROOT_IO
  ClassDef(OpticalPhoton, 12);
#endif
};

#endif
