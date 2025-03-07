
#include <TRef.h>
#include <TObject.h>

#ifndef _OPTICAL_BOUNDARY_
#define _OPTICAL_BOUNDARY_

#include "CherenkovRadiator.h"
class ParametricSurface;

class OpticalBoundary: public TObject {
  friend class IRT;

 public:
 OpticalBoundary(): m_Radiator(0), m_Surface(0), m_Refractive(true) {};
  // The "0" as a radiator ptr implicitly says "there is no photon life beyond this boundary in IRT";
 OpticalBoundary(/*const*/ CherenkovRadiator *radiator, const ParametricSurface *surface, bool refractive): 
  m_Radiator(radiator), m_Surface(surface), m_Refractive(refractive) {};
  ~OpticalBoundary() {};

  CherenkovRadiator *GetRadiator( void ) const {
    return dynamic_cast<CherenkovRadiator*>(m_Radiator.GetObject());
  };
  const ParametricSurface *GetSurface( void ) const { return m_Surface; };

 private:
  // Radiator pointer (only need average refractive index from this base class in IRT);
  // it is assumed that the code which makes use of this class knows how to properly 
  // order the boundaries, so a new one in the respective std::vector has a preceding 
  // media with its own (and potentially different) m_Radiator pointer;
  TRef m_Radiator;

  // Boundary surface; either refractive or reflective; 
  const ParametricSurface *m_Surface;
  bool m_Refractive;

  // Working variables; FIXME: not multithreading friendly;
  static thread_local TVector3 m_ImpactPoint, m_IncomingDirection, m_OutgoingDirection; //!

  ClassDef(OpticalBoundary, 1);
};

#endif
