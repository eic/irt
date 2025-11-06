
#ifndef _CHERENKOV_MIRROR_
#define _CHERENKOV_MIRROR_

#include <G4Object.h>
#include "FlatSurface.h"
#include "SphericalSurface.h"
#include "CylindricalSurface.h"
#include "ConicalSurface.h"
#include "ParametricSurface.h"

class CherenkovWaveLengthRange;

class SurfaceCopy: public G4ObjectCopy {
 public:
 SurfaceCopy(G4VPhysicalVolume *phys = 0): G4ObjectCopy(phys), m_Surface(0) {};
  ~SurfaceCopy() {};

  ParametricSurface *m_Surface;

  ClassDef(SurfaceCopy, 1);
};

class CherenkovMirror: public G4Object {
 public:
 CherenkovMirror(G4VSolid *solid = 0, G4Material *material = 0): G4Object(solid, material), m_MirrorSurface(0) {};
  ~CherenkovMirror() {};

  void SetReflectivity(double reflectivity, CherenkovWaveLengthRange *wlrange);// void );

  G4ObjectCopy *CreateCopy(G4VPhysicalVolume *phys) { return new SurfaceCopy(phys); };
  
  void AdjustWedgeCopies(G4VPhysicalVolume *mother) {
    for(int iq=0; iq<6; iq++) {
      auto mcopy = dynamic_cast<SurfaceCopy*>(m_Copies[iq]);

      // FIXME: well, one-off here because originally the "top 60 degree wedge is defined 
      // as a basic shape everywhere; however want to count them starting from 0 degrees;
      mcopy->m_Surface = dynamic_cast<ParametricSurface*>(this)->_Clone((iq-1)*60*M_PI/180, TVector3(0,0,1));
    } //for iq
  };

  G4OpticalSurface *GetMirrorSurface( void ) const { return m_MirrorSurface; };

 private:
  G4OpticalSurface *m_MirrorSurface;   //!
  
  ClassDef(CherenkovMirror, 1);
};

class FlatMirror: public CherenkovMirror, public FlatSurface {
 public:
  FlatMirror() {};
 FlatMirror(G4VSolid *solid, G4Material *material, const TVector3 &x0, const TVector3 &nx, const TVector3 &ny, 
	    double sx = 0.0, double sy = 0.0): 
  CherenkovMirror(solid, material), FlatSurface(x0, nx, ny, sx, sy) {};
  ~FlatMirror() {};

  ClassDef(FlatMirror, 2);
};

class SphericalMirror: public CherenkovMirror, public SphericalSurface {
 public:
 SphericalMirror() {};
 SphericalMirror(G4VSolid *solid, G4Material *material, const TVector3 &x0, double r0): 
 CherenkovMirror(solid, material), SphericalSurface(x0, r0) {};
  ~SphericalMirror() {};
  
  ClassDef(SphericalMirror, 2);
};

class CylindricalMirror: public CherenkovMirror, public CylindricalSurface {
 public:
 CylindricalMirror() {};
 CylindricalMirror(G4VSolid *solid, G4Material *material, const TVector3 &x0, const TVector3 &nz, double r0, double dz): 
 CherenkovMirror(solid, material), CylindricalSurface(x0, nz, r0, dz) {};
  ~CylindricalMirror() {};
  
  ClassDef(CylindricalMirror, 1);
};

class ConicalMirror: public CherenkovMirror, public ConicalSurface {
 public:
 ConicalMirror() {};
 ConicalMirror(G4VSolid *solid, G4Material *material, const TVector3 &x0, const TVector3 &nz, double r0, double r1, double dz): 
 CherenkovMirror(solid, material), ConicalSurface(x0, nz, r0, r1, dz) {};
  ~ConicalMirror() {};
  
  ClassDef(ConicalMirror, 1);
};

#endif
