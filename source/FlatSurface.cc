
#include "ParametricSurface.h"

// -------------------------------------------------------------------------------------

bool FlatSurface::GetCrossing(const TVector3 &x0, const TVector3 &n0, TVector3 *crs, 
			      bool check_normal) const
{
  double npl = n0.Dot(m_Nz);

  // FIXME: this is not clean, and also the line can belong to the plane;
  if (!crs || !npl) return false;

  // May want to check that approach the surface from a right direction;
  if (check_normal && n0.Dot(m_Nz) >= 0.0) return false;

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

double FlatSurface::GetDistance(const TVector3 &xx) const
{
  return fabs((xx - GetCenter()).Dot(m_Nz));
} // FlatSurface::GetDistance()

// -------------------------------------------------------------------------------------
