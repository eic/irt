
#include "SphericalSurface.h"

// -------------------------------------------------------------------------------------

bool SphericalSurface::GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs, 
				   bool check_normal) const
{
  TVector3 dx = x0 - GetCenter();
  double a = 1.0, b = 2*dx.Dot(n0), c = dx.Mag2() - pow(m_Radius, 2);

  return GetQuadraticEquationCaseCrossing(x0, n0, crs, check_normal, a, b, c);
} // SphericalSurface::GetCrossing()

// -------------------------------------------------------------------------------------

double SphericalSurface::GetDistance(const TVector3 &xx) const
{
  return fabs((xx - GetCenter()).Mag() - m_Radius);
} // SphericalSurface::GetDistance()

// -------------------------------------------------------------------------------------
