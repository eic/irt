
#include "ToricSurface.h"

// -------------------------------------------------------------------------------------

bool ToricSurface::GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs, 
			       bool check_normal) const
{
  TVector3 dx = x0 - GetCenter();


  //double a = 1.0, b = 2*dx.Dot(n0), c = dx.Mag2() - pow(m_Radius, 2);

  return true;//GetQuadraticEquationCaseCrossing(x0, n0, crs, check_normal, a, b, c);
} // ToricSurface::GetCrossing()

// -------------------------------------------------------------------------------------

double ToricSurface::GetDistance(const TVector3 &xx) const
{
  // FIXME: unify with GetU();
  auto dx = xx - GetCenter();
  // Rotate to align torus axis with (0,0,1);
  dx.Rotate(-m_Alfa, m_Nr);
  dx.Rotate(-dx.Phi(), TVector3(0,0,1));
  TVector3 pc(m_R, 0.0, 0.0);
  dx -= pc;

  return fabs(dx.Mag() - m_r);
} // ToricSurface::GetDistance()

// -------------------------------------------------------------------------------------
