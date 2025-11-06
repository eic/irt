
#include "ParametricSurface.h"

//
// Parameterization: R - major radius, r - donut tube radius;
//
// NB: this class is actually disfunctional (a crossing point with a straight line
// in 3D ends up with a quartic equation);
//

#ifndef _IRT_TORIC_SURFACE_
#define _IRT_TORIC_SURFACE_

class ToricSurface: public ParametricSurface {
 public:
 ToricSurface(): m_Concave(true), m_R(0.0), m_r(0.0), m_Alfa(0.0) {};
 ToricSurface(const TVector3 &x0, const TVector3 &nz, double R, double r, 
	      double umin = 0.0, double umax = 2*M_PI, 
	      double vmin = 0.0, double vmax = 2*M_PI): 
  ParametricSurface(x0, umin, umax, vmin, vmax), m_Concave(true), m_R(R), m_r(r) {
    if (!m_Nz.x() && !m_Nz.y()) {
      m_Alfa = 0.0;
      m_Nr = TVector3(0,0,1);
    } else {
      auto axis = TVector3(0,0,1).Cross(m_Nz);
      m_Alfa = axis.Mag();
      m_Nr = axis.Unit();
    } //if

  };
  ~ToricSurface() {};

  // FIXME: no range check?; 
  TVector3 GetSpacePoint(double theta, double phi) const {
    // Here theta is rotation angle around the tube, phi is rotation around the torus axis;
    double x = (m_R + m_r*cos(theta))*cos(phi), y = (m_R + m_r*cos(theta))*sin(phi), z = m_r*sin(theta);
    TVector3 local(x, y, z);
    // FIXME: sign check;
    local.Rotate(m_Alfa, m_Nr);

    return GetCenter() + local;
  };

  TVector3 GetNormal(const TVector3 &xx) const {
    //assert(0);
    auto dx = xx - GetCenter();
    // Rotate to align torus axis with (0,0,1);
    dx.Rotate(-m_Alfa, m_Nr);
    TVector3 pc(m_R, 0.0, 0.0);
    pc.Rotate(dx.Phi(), TVector3(0,0,1));
    dx -= pc;

    // Rotate back;
    dx.Rotate( m_Alfa, m_Nr);

    return (m_Concave ? -1.0 : 1.0)*dx.Unit();
  };
  //double GetRadius( void ) const { return m_Radius; };

  bool GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs, bool check_normal = true) const;
  double GetDistance(const TVector3 &xx) const;

  void SetConvex( void ) { m_Concave = false; };
  ParametricSurface *_Clone(double angle, const TVector3 &axis) const {
    auto copy = new ToricSurface(*this);

    copy->m_Center.Rotate(angle, axis);

    copy->m_Nz.Rotate(angle, axis);
    copy->m_Nr.Rotate(angle, axis);

    return copy;
  };

  // Theta;
  double GetU(const TVector3 &xx) const {
    auto dx = xx - GetCenter();
    // FIXME: sign check;
    dx.Rotate(-m_Alfa, m_Nr);
    dx.Rotate(-dx.Phi(), TVector3(0,0,1));
    TVector3 pc(m_R, 0.0, 0.0);
    dx -= pc;
    TVector2 qc(dx.X(), dx.Z());

    // Is this phi definition the same as in GetSpacePoint()?;
    return qc.Phi();
  };
  // Phi;
  double GetV(const TVector3 &xx) const {
    auto dx = xx - GetCenter();
    // FIXME: sign check;
    dx.Rotate(-m_Alfa, m_Nr);
    
    return dx.Phi();
  };

 private:
  bool m_Concave;
  TVector3 m_Nz, m_Nr;
  double m_R, m_r, m_Alfa;

#ifndef DISABLE_ROOT_IO
  ClassDef(ToricSurface, 1);
#endif
};

#endif
