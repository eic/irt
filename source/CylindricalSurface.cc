
#include <assert.h>

#include "CylindricalSurface.h"

// -------------------------------------------------------------------------------------

bool CylindricalSurface::GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs, 
				     bool check_normal) const
{
  TVector3 dx = x0 - GetCenter();
  TVector3 nc = m_Nz, A = dx - dx.Dot(nc)*nc, B = n0 - n0.Dot(nc)*nc;
  double a = B.Mag2(), b = 2*A.Dot(B), c = A.Mag2() - pow(m_Radius, 2);

  return GetQuadraticEquationCaseCrossing(x0, n0, crs, check_normal, a, b, c);
} // CylindricalSurface::GetCrossing()

// -------------------------------------------------------------------------------------

double CylindricalSurface::GetDistance(const TVector3 &xx) const
{
  assert(0);
  auto dx = xx - GetCenter();
  dx -= dx.Dot(m_Nz)*m_Nz;

  return fabs(dx.Mag() - m_Radius);
} // CylindricalSurface::GetDistance()

// -------------------------------------------------------------------------------------
