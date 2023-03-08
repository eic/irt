
#include "ParametricSurface.h"

// -------------------------------------------------------------------------------------

bool SphericalSurface::GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs, 
				   bool check_normal) const
{
  TVector3 dx = x0 - GetCenter();
  double a = 1.0, b = 2*dx.Dot(n0), c = dx.Mag2() - pow(m_Radius, 2);
  double det = b*b - 4*a*c;

  if (!crs || det < 0.0) return false;
  
  {
    // Assume that n0[] was defining a proper direction, and the surface was "complete", 
    // so that one is looking for the smallest positive root;
    double tt[2] = {(-b - sqrt(det))/(2*a), (-b + sqrt(det))/(2*a)};
    for(unsigned iq=0; iq<2; iq++) {
      double t = tt[iq];

      if (t < 0.0) continue;
      *crs = x0 + t*n0;
      if (check_normal && n0.Dot(GetNormal(*crs)) >= 0.0) continue;

      return true;
    } //for iq

    return false;
  }
} // SphericalSurface::GetCrossing()

// -------------------------------------------------------------------------------------

double SphericalSurface::GetDistance(const TVector3 &xx) const
{
  return fabs((xx - GetCenter()).Mag() - m_Radius);
} // SphericalSurface::GetDistance()

// -------------------------------------------------------------------------------------
