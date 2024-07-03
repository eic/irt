
#ifndef _CHERENKOV_MIRROR_
#define _CHERENKOV_MIRROR_

#include "G4Object.h"
#include "ParametricSurface.h"

class SurfaceCopy: public G4ObjectCopy {
 public:
 SurfaceCopy(G4VPhysicalVolume *phys = 0): G4ObjectCopy(phys), m_Surface(0) {};
  ~SurfaceCopy() {};

  ParametricSurface *m_Surface;

#ifndef DISABLE_ROOT_IO
  ClassDef(SurfaceCopy, 1);
#endif
};

class CherenkovMirror: public G4Object {
 public:
 CherenkovMirror(G4VSolid *solid = 0, G4Material *material = 0): G4Object(solid, material), m_MirrorSurface(0) {};
  ~CherenkovMirror() {};

  void SetReflectivity( void );

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
  
#ifndef DISABLE_ROOT_IO
  ClassDef(CherenkovMirror, 1);
#endif
};

class FlatMirror: public CherenkovMirror, public FlatSurface {
 public:
  FlatMirror() {};
 FlatMirror(G4VSolid *solid, G4Material *material, const TVector3 &x0, const TVector3 &nx, const TVector3 &ny, 
	    double sx = 0.0, double sy = 0.0): 
  CherenkovMirror(solid, material), FlatSurface(x0, nx, ny, sx, sy) {};
  ~FlatMirror() {};

#ifndef DISABLE_ROOT_IO
  ClassDef(FlatMirror, 2);
#endif
};

class SphericalMirror: public CherenkovMirror, public SphericalSurface {
 public:
 SphericalMirror() {};
 SphericalMirror(G4VSolid *solid, G4Material *material, const TVector3 &x0, double r0): 
 CherenkovMirror(solid, material), SphericalSurface(x0, r0) {};
  ~SphericalMirror() {};
  
#ifndef DISABLE_ROOT_IO
  ClassDef(SphericalMirror, 2);
#endif
};

class SphericalMirrorWithHalfSpace: public CherenkovMirror, public SphericalSurfaceWithHalfSpace {
 public:
  SphericalMirrorWithHalfSpace() {};
  SphericalMirrorWithHalfSpace(G4VSolid *solid, G4Material *material, const TVector3 &x0, double r0, const TVector3 &pNorm, const TVector3 &pPoint): 
    CherenkovMirror(solid, material), SphericalSurfaceWithHalfSpace(x0, r0, pNorm, pPoint) {};
  ~SphericalMirrorWithHalfSpace() {};
  
#ifndef DISABLE_ROOT_IO
  ClassDef(SphericalMirrorWithHalfSpace, 2);
#endif
};

class CherenkovMirrorGroup: public TObject {
 public:
  CherenkovMirrorGroup() {};
  ~CherenkovMirrorGroup() {};

  void AddMirror(CherenkovMirror *mirror) { m_Mirrors.push_back(mirror); };

 private:
  std::vector<CherenkovMirror*> m_Mirrors;

#ifndef DISABLE_ROOT_IO
  ClassDef(CherenkovMirrorGroup, 1);
#endif
};

#endif
