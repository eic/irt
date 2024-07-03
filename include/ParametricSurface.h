
#include <math.h>

#include <TObject.h>
#include <TVector3.h>

#ifndef _IRT_PARAMETRIC_SURFACE_
#define _IRT_PARAMETRIC_SURFACE_

class ParametricSurface: public TObject {
 public:
 ParametricSurface(): m_Umin(0.0), m_Umax(0.0), m_Vmin(0.0), m_Vmax(0.0) {};
 ParametricSurface(const TVector3 &x0, double umin, double umax, double vmin, double vmax):
  m_Center(x0), m_Umin(umin), m_Umax(umax), m_Vmin(vmin), m_Vmax(vmax) {};
  ~ParametricSurface() {};
  
  // 2D parameter ranges (call them U&V);
  double Umin( void ) const { return m_Umin; };
  double Umax( void ) const { return m_Umax; };
  double Vmin( void ) const { return m_Vmin; };
  double Vmax( void ) const { return m_Vmax; };

  virtual bool IsInside(double u, double v) const {
    return (u >= m_Umin && u <= m_Umax && v >= m_Vmin && v <= m_Vmax);
  };  
  void SetUVranges(double umin, double umax, double vmin, double vmax) {
    m_Umin = umin; m_Umax = umax; m_Vmin = vmin; m_Vmax = vmax;
  };
  virtual TVector3 GetSpacePoint(double u, double v) const = 0;
  // There is no check that the point actually belongs to the surface;
  // it is assumed that GEANT stepping was done correctly, so the point 
  // is more or less close to the 2D surface; for flat and spherical 
  // surfaces this is a mute point anyway since this function is implemented 
  // in a trivial way;
  virtual TVector3 GetNormal(const TVector3 &xx) const = 0;
  // Provide a reasonable wrapper; of course simple surfaces 
  // can to their own calculations;
  virtual TVector3 GetNormal(double u, double v) const {
    auto pt = GetSpacePoint(u, v);
    return GetNormal(pt);
  };
  // default, not overriden by all derived classes
  virtual TVector3 GetNormal( void ) const {
    return TVector3(0.0, 0.0, 0.0);
  };
  const TVector3 &GetCenter( void ) const { return m_Center; };
  virtual double GetDistance(const TVector3 &xx) const = 0;

  // Crossing with the straight line defined by {x0,n0}; 
  virtual bool GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs, 
			   bool check_normal = true) const = 0;

  // Introduce only the transformations needed for the current task;
  virtual ParametricSurface *_Clone(double angle, const TVector3 &axis) const = 0;
  void Shift(const TVector3 &shift) {
    m_Center += shift;
  };
  
 protected:
  // It looks like surfaces I need typically have m_Center defined;
  TVector3 m_Center;

 private:
  double m_Umin, m_Umax, m_Vmin, m_Vmax;

#ifndef DISABLE_ROOT_IO
  ClassDef(ParametricSurface, 1);
#endif
};

class SphericalSurface: public ParametricSurface {
 public:
 SphericalSurface(): m_Concave(true), m_Radius(0.0) {};
 SphericalSurface(const TVector3 &x0, double r0, double umin = 0.0, double umax = M_PI, 
		  double vmin = 0.0, double vmax = 2*M_PI): 
  ParametricSurface(x0, umin, umax, vmin, vmax), m_Concave(true), m_Radius(r0) {};
  ~SphericalSurface() {};
  using ParametricSurface::IsInside;
  virtual bool IsInside(TVector3 p) const {
    double u = p.Theta();
    double v = p.Phi();
    if (v < 0) v += 2*M_PI;
    // override, accounting for \phi periodicity
    if (Vmin() <= Vmax()) {
      return (u >= Umin() && u <= Umax() && v >= Vmin() && v <= Vmax());
    } else {
      return ( (v >= Vmin() || v <= Vmax()) && (u >= Umin() && u <= Umax() ) );
    }    
  }
  // FIXME: no range check?; 
  TVector3 GetSpacePoint(double theta, double phi) const {
    TVector3 nn(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
    return GetCenter() + m_Radius*nn;
  };

  TVector3 GetNormal(const TVector3 &xx) const {
    return (m_Concave ? -1.0 : 1.0)*(xx - GetCenter()).Unit();
  };
  double GetRadius( void ) const { return m_Radius; };

  bool GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs, bool check_normal = true) const;
  double GetDistance(const TVector3 &xx) const;

  void SetConvex( void ) { m_Concave = false; };
  ParametricSurface *_Clone(double angle, const TVector3 &axis) const {
    auto copy = new SphericalSurface(*this);

    copy->m_Center.Rotate(angle, axis);

    return copy;
  };

 private:
  bool m_Concave;
  double m_Radius;

#ifndef DISABLE_ROOT_IO
  ClassDef(SphericalSurface, 2);
#endif
};

// want the same behavior as a SphericalSurface, but also checking
// that any crossing point is also passing the given half-space condition
class SphericalSurfaceWithHalfSpace: public SphericalSurface {
public:
  SphericalSurfaceWithHalfSpace(): SphericalSurface() {} ;
  SphericalSurfaceWithHalfSpace(const TVector3 &x0, double r0, const TVector3 &pNorm, const TVector3 &pPoint, double umin = 0.0, double umax = M_PI, 
		  double vmin = 0.0, double vmax = 2*M_PI): 
    SphericalSurface(x0, r0, umin, umax, vmin, vmax), m_HSNorm(pNorm), m_HSPoint(pPoint) {};
  ~SphericalSurfaceWithHalfSpace() {};

  bool IsInside(TVector3 p) const;
private:
  // point and normal defining half-space
  TVector3 m_HSNorm;
  TVector3 m_HSPoint;
#ifndef DISABLE_ROOT_IO
  ClassDef(SphericalSurfaceWithHalfSpace, 1);
#endif
};

class LocalCoordinatesXY: public TObject {
 public:
  LocalCoordinatesXY() {};
  ~LocalCoordinatesXY() {};

  virtual double GetLocalX(const TVector3 &xx) const = 0;
  virtual double GetLocalY(const TVector3 &xx) const = 0;

#ifndef DISABLE_ROOT_IO
  ClassDef(LocalCoordinatesXY, 1);
#endif
}; 

// In fact a rectangle in space;
class FlatSurface: public ParametricSurface, public LocalCoordinatesXY {
 public:
  FlatSurface() {};
 FlatSurface(const TVector3 &x0, const TVector3 &nx, const TVector3 &ny, double sx = 0.0, double sy = 0.0):
  // FIXME: no orthogonality and / or normalization check?;
  ParametricSurface(x0, -sx/2, sx/2, -sy/2, sy/2), m_Nx(nx.Unit()), m_Ny(ny.Unit()) {
    m_Nz = nx.Cross(ny).Unit();
  };
  ~FlatSurface() {};

  TVector3 GetSpacePoint(double x, double y) const {
    return GetCenter() + x*m_Nx + y*m_Ny;
  };
  TVector3 GetNormal(const TVector3 &xx) const {
    // Make compiler happy, use 'xx' variable;
    return m_Nz + 0.0*xx;
  };
  TVector3 GetNormal( void ) const {
    return m_Nz;
  };
  // FIXME: well, this calculation assumes orthogonal X&Y vectors were provided;
  inline double GetLocalX(const TVector3 &xx) const {
    return (xx - GetCenter()).Dot(m_Nx);
  };
  inline double GetLocalY(const TVector3 &xx) const {
    return (xx - GetCenter()).Dot(m_Ny);
  };

  bool GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs, bool check_normal = true) const;
  double GetDistance(const TVector3 &xx) const;
  ParametricSurface *_Clone(double angle, const TVector3 &axis) const {
    auto copy = new FlatSurface(*this);

    copy->m_Center.Rotate(angle, axis);

    copy->m_Nx.Rotate(angle, axis);
    copy->m_Ny.Rotate(angle, axis);
    copy->m_Nz.Rotate(angle, axis);

    return copy;
  };

 private:
  TVector3 m_Nx, m_Ny, m_Nz;

#ifndef DISABLE_ROOT_IO
  ClassDef(FlatSurface, 1);
#endif
};

#endif
