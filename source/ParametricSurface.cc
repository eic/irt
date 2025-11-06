
#include "ParametricSurface.h"

// -------------------------------------------------------------------------------------

bool ParametricSurface::GetQuadraticEquationCaseCrossing(const TVector3 &x0, const TVector3 &n0, 
							 TVector3 *crs, bool check_normal, 
							 double a, double b, double c) const
{
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

      // FIXME: make this check working;
      //if (!IsInside(*crs)) continue;

      return true;
    } //for iq

    return false;
  }
} // ParametricSurface::GetQuadraticEquationCaseCrossing()

// -------------------------------------------------------------------------------------
