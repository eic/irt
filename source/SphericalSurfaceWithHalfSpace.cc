
#include "ParametricSurface.h"
// -------------------------------------------------------------------------------------


// check if inside sphere and half space
bool SphericalSurfaceWithHalfSpace::IsInside(TVector3 p) const {
  // lab frame theta, phi
  double u = p.Theta();
  double v = p.Phi();

  bool insideSphere;
  if (Vmin() <= Vmax()) {
    insideSphere = (u >= Umin() && u <= Umax() && v >= Vmin() && v <= Vmax());
  } else {
    insideSphere = ( (v >= Vmin() || v <= Vmax()) && (u >= Umin() && u <= Umax() ) );
  }
  
  auto insideHalfSpace = (m_HSNorm.Dot(p-m_HSPoint) > 0);
  return (insideSphere && insideHalfSpace);
} // SphericalSurfaceWithHalfSpace::IsInside()
