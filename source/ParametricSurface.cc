
#include <ParametricSurface.h>

// -------------------------------------------------------------------------------------

bool ParametricSurface::GetQuadraticEquationCaseCrossing(const TVector3 &x0, const TVector3 &n0, 
							 TVector3 *crs, bool check_normal, 
							 double a, double b, double c) const
{
  //printf("ParametricSurface::GetQuadraticEquationCaseCrossing() ...\n");
  double det = b*b - 4*a*c;

  //printf("Here-1\n");
  //printf("%f %f %f, %f %f %f\n", x0.X(), x0.Y(), x0.Z(), n0.X(), n0.Y(), n0.Z());
  if (!crs || det < 0.0) return false;
  
  {
    // Assume that n0[] was defining a proper direction, and the surface was "complete", 
    // so that one is looking for the smallest positive root;
    double tt[2] = {(-b - sqrt(det))/(2*a), (-b + sqrt(det))/(2*a)};
    //printf("%f %f\n", tt[0], tt[1]);
    for(unsigned iq=0; iq<2; iq++) {
      double t = tt[iq];

      if (t < 0.0) continue;
      *crs = x0 + t*n0;
      //printf("%f %f %f\n", crs->X(), crs->Y(), crs->Z());
      //if (t < 0.0) continue;
      if (check_normal && n0.Dot(GetNormal(*crs)) >= 0.0) continue;

      // FIXME: make this check working;
      //if (!IsInside(*crs)) continue;

      return true;
    } //for iq

    //printf("Here-2\n");
    return false;
  }
} // ParametricSurface::GetQuadraticEquationCaseCrossing()

// -------------------------------------------------------------------------------------
