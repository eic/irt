
#include <ParametricSurface.h>

// -------------------------------------------------------------------------------------

bool SphericalSurface::GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs) const
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
      if (n0.Dot(GetNormal(*crs)) >= 0.0) continue;

      return true;
    } //for iq

    return false;
  }
} // SphericalSurface::GetCrossing()

// -------------------------------------------------------------------------------------

bool FlatSurface::GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs) const
{
  double npl = n0.Dot(m_Nz);

  // FIXME: this is not clean, and also the line can belong to the plane;
  if (!crs || !npl) return false;

  // May want to check that approach the surface from a right direction;
  if (/*check_normal &&*/ n0.Dot(m_Nz) >= 0.0) return false;

  {
    TVector3 dx = x0 - GetCenter();
    double t = -m_Nz.Dot(dx)/npl;
    
    // Again, assume n0[] was providing a direction -> check the sign; 
    if (t < 0.0) return false;
    
    *crs = x0 + t*n0;
    
    return true;
  }
} // FlatSurface::GetCrossing()

// -------------------------------------------------------------------------------------
