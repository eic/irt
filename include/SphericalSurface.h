#pragma once

#include "ParametricSurface.h"

namespace IRT2 {

class SphericalSurface: public ParametricSurface {
 public:
 SphericalSurface(): m_Concave(true), m_Radius(0.0) {};
 SphericalSurface(const TVector3 &x0, double r0, double umin = 0.0, double umax = M_PI, 
		  double vmin = 0.0, double vmax = 2*M_PI): 
  ParametricSurface(x0, umin, umax, vmin, vmax), m_Concave(true), m_Radius(r0) {};
  ~SphericalSurface() {};

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

  // FIXME: for now ignore possible sphere piece rotations;
  double GetU(const TVector3 &xx) const { return (xx - GetCenter()).Theta(); };
  double GetV(const TVector3 &xx) const { return (xx - GetCenter()).Phi();   };

 private:
  bool m_Concave;
  double m_Radius;

#ifndef DISABLE_ROOT_IO
  ClassDef(SphericalSurface, 2);
#endif
};

} // namespace IRT2
