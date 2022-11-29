
//
// Yes, prefer to keep it separately even that it is a subset of a ConicalSurface;
//

#include <ParametricSurface.h>

#ifndef _IRT_CYLINDRICAL_SURFACE_
#define _IRT_CYLINDRICAL_SURFACE_

class CylindricalSurface: public ParametricSurface {
 public:
 CylindricalSurface(): m_Concave(true), m_Radius(0.0), m_Alfa(0.0) {};
 CylindricalSurface(const TVector3 &x0, const TVector3 &nz, double r0, double dz,
		  double vmin = 0.0, double vmax = 2*M_PI): 
  ParametricSurface(x0, -dz/2, dz/2, vmin, vmax), m_Concave(true), m_Nz(nz.Unit()), m_Radius(r0) {
    if (!m_Nz.x() && !m_Nz.y()) {
      m_Alfa = 0.0;
      m_Nr = TVector3(0,0,1);
    } else {
      auto axis = TVector3(0,0,1).Cross(m_Nz);
      m_Alfa = axis.Mag();
      m_Nr = axis.Unit();
    } //if
  };
  ~CylindricalSurface() {};

  // FIXME: no range check?; 
  TVector3 GetSpacePoint(double z, double phi) const {
    TVector3 nn(cos(phi), sin(phi), 0.0);
    auto local = z*m_Nz + m_Radius*nn;
    // FIXME: sign check;
    local.Rotate(m_Alfa, m_Nr);

    return GetCenter() + local;
  };

  TVector3 GetNormal(const TVector3 &xx) const {
    auto dx = xx - GetCenter();
    dx -= dx.Dot(m_Nz)*m_Nz;

    //return (m_Concave ? -1.0 : 1.0)*(xx - GetCenter()).Unit();
    return (m_Concave ? -1.0 : 1.0)*dx.Unit();
  };

  bool GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs, bool check_normal = true) const;
  double GetDistance(const TVector3 &xx) const;

  void SetConvex( void ) { m_Concave = false; };
  ParametricSurface *_Clone(double angle, const TVector3 &axis) const {
    auto copy = new CylindricalSurface(*this);

    copy->m_Center.Rotate(angle, axis);

    copy->m_Nz.Rotate(angle, axis);
    copy->m_Nr.Rotate(angle, axis);

    return copy;
  };

  double GetU(const TVector3 &xx) const {
    auto dx = xx - GetCenter();
    // FIXME: sign check;
    dx.Rotate(-m_Alfa, m_Nr);
    
    return dx.Phi();
  };
  double GetV(const TVector3 &xx) const {
    return (xx - GetCenter()).Dot(m_Nz);
  };

 private:
  bool m_Concave;
  TVector3 m_Nz, m_Nr;
  double m_Radius, m_Alfa;

  ClassDef(CylindricalSurface, 1);
};

#endif
