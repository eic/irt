
#include "ParametricSurface.h"
// -------------------------------------------------------------------------------------


// check if inside sphere and half space
bool SphericalSurfaceWithHalfSpace::IsInside(TVector3 p) const {
  // lab frame theta, phi
  double u = p.Theta();
  double v = p.Phi();
  if (v < 0) v += 2*M_PI;

  bool insideSphere;
  if (Vmin() <= Vmax()) {
    insideSphere = (u >= Umin() && u <= Umax() && v >= Vmin() && v <= Vmax());
  } else {
    insideSphere = ( (v >= Vmin() || v <= Vmax()) && (u >= Umin() && u <= Umax() ) );
  }
  
  // check if outside of any given half spaces
  for(int i = 0; i < m_HSNorm.size(); i++){
    if (m_HSNorm[i].Dot(p-m_HSPoint[i]) < 0) return false;
  }
  
  return insideSphere;
} // SphericalSurfaceWithHalfSpace::IsInside()
