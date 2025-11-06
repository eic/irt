
#include "ParametricSurface.h"

#ifndef _IRT_CONICAL_SURFACE_
#define _IRT_CONICAL_SURFACE_

class ConicalSurface: public ParametricSurface {
 public:
 ConicalSurface(): m_Concave(true), m_R0(0.0), m_R1(0.0), m_Dz(0.0), m_Alfa(0.0), m_Rc(0.0), m_Slope(0.0) {};
 ConicalSurface(const TVector3 &x0, const TVector3 &nz, double r0, double r1, double dz,
		  double vmin = 0.0, double vmax = 2*M_PI): 
  ParametricSurface(x0, -dz/2, dz/2, vmin, vmax), m_Concave(true), m_Nz(nz.Unit()), m_R0(r0), m_R1(r1), m_Dz(dz) {
    if (!m_Nz.x() && !m_Nz.y()) {
      m_Alfa = 0.0;
      m_Nr = TVector3(0,0,1);
    } else {
      auto axis = TVector3(0,0,1).Cross(m_Nz);
      m_Alfa = axis.Mag();
      m_Nr = axis.Unit();
    } //if

    m_Rc = (r0+r1)/2;
    // FIXME: assumes dz>0;
    m_Slope = (r1-r0)/dz; 
  };
  ~ConicalSurface() {};

  // FIXME: no range check?; 
  TVector3 GetSpacePoint(double z, double phi) const {
    TVector3 nn(cos(phi), sin(phi), 0.0);
    double r = m_Rc + z*m_Slope;
    auto local = z*m_Nz + r*nn;
    // FIXME: sign check;
    local.Rotate(m_Alfa, m_Nr);
    return GetCenter() + local;
  };

  TVector3 GetNormal(const TVector3 &xx) const {
    //assert(0);
    auto dx = xx - GetCenter(), axis = dx.Cross(m_Nz).Unit();
    dx -= dx.Dot(m_Nz)*m_Nz;
    dx.Rotate(-atan(m_Slope), axis);

    return (m_Concave ? -1.0 : 1.0)*dx.Unit();
  };

  bool GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs, bool check_normal = true) const;
  double GetDistance(const TVector3 &xx) const;

  void SetConvex( void ) { m_Concave = false; };
  ParametricSurface *_Clone(double angle, const TVector3 &axis) const {
    auto copy = new ConicalSurface(*this);

    copy->m_Center.Rotate(angle, axis);

    copy->m_Nz.Rotate(angle, axis);
    copy->m_Nr.Rotate(angle, axis);

    return copy;
  };

  // These two are the same for ConicalSurface & CylindricalSurface;
  double GetU(const TVector3 &xx) const {
    auto dx = xx - GetCenter();
    // FIXME: sign check;
    dx.Rotate(-m_Alfa, m_Nr);
    
    // FIXME: check units;
    return dx.Phi();
  };
  double GetV(const TVector3 &xx) const {
    return (xx - GetCenter()).Dot(m_Nz);
  };

 private:
  bool m_Concave;
  TVector3 m_Nz, m_Nr;
  double m_R0, m_R1, m_Dz, m_Alfa, m_Rc, m_Slope;

  ClassDef(ConicalSurface, 1);
};

#endif
