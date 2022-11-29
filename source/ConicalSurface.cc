
#include <assert.h>

#include <ConicalSurface.h>

// -------------------------------------------------------------------------------------

bool ConicalSurface::GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs, 
				     bool check_normal) const
{
  TVector3 dx = x0 - GetCenter();
  TVector3 nc = m_Nz, A = dx - dx.Dot(nc)*nc, B = n0 - n0.Dot(nc)*nc;
  double Aq = m_Rc + m_Slope*dx.Dot(nc), Bq = m_Slope*n0.Dot(nc);
  double a = B.Mag2() - Bq*Bq, b = 2*(A.Dot(B) - Aq*Bq), c = A.Mag2() - Aq*Aq;

  return GetQuadraticEquationCaseCrossing(x0, n0, crs, check_normal, a, b, c);
} // ConicalSurface::GetCrossing()

// -------------------------------------------------------------------------------------

double ConicalSurface::GetDistance(const TVector3 &xx) const
{
  assert(0);
  auto dx = xx - GetCenter();
  double dz = dx.Dot(m_Nz);
  dx -= dx.Dot(m_Nz)*m_Nz;
  double r = dx.Mag(), rc = m_Rc + dz*m_Slope, dr = fabs(r - rc);

  return dr*cos(atan(m_Slope));
} // ConicalSurface::GetDistance()

// -------------------------------------------------------------------------------------
