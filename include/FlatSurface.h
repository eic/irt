
#include "ParametricSurface.h"

#ifndef _IRT_FLAT_SURFACE_
#define _IRT_FLAT_SURFACE_

namespace IRT2 {

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

  double GetU(const TVector3 &xx) const { return GetLocalX(xx); };
  double GetV(const TVector3 &xx) const { return GetLocalY(xx); };

 private:
  TVector3 m_Nx, m_Ny, m_Nz;

#ifndef DISABLE_ROOT_IO
  ClassDef(FlatSurface, 1);
#endif
};

} // namespace IRT2

#endif
