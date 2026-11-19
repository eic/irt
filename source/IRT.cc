
#include "IRT.h"
#include "ChargedParticle.h"
#include "ConicalSurface.h"

#include <mutex>

namespace IRT2 {

thread_local TVector3 OpticalBoundary::m_ImpactPoint, OpticalBoundary::m_IncomingDirection;
thread_local TVector3 OpticalBoundary::m_OutgoingDirection;

// -------------------------------------------------------------------------------------

  bool IRT::Transport(ChargedParticle *mcparticle, const TVector3 &xfrom, const TVector3 &nfrom, double *length)
{
  bool transport_in_progress = false;
  TVector3 x0 = xfrom, n0 = nfrom;
  if (length) *length = 0.0;
  
  // Just go through the optical boundaries, and calculate either reflection 
  // or refraction on that particular surface;
  for(unsigned iq=0; iq<_m_OpticalBoundaries.size(); iq++) {
    auto boundary = GetOpticalBoundary(iq), prev = iq ? GetOpticalBoundary(iq-1) : 0;
    auto surface = boundary->m_Surface;

    bool ok = surface->GetCrossing(x0, n0, &boundary->m_ImpactPoint);

    // The logic here is that the first few boundaries may be irrelenat for this 
    // emission point (say for the gas case the emission point is beyond the aerogel-gas
    // refractive boundary; then just skip to the next one without changing [x0,n0]); 
    // however if one valid boundary was handled already, this must be a no-crossing case; 
    // then return false immediately;
    if (!ok) {
      if (transport_in_progress) 
	return false;
      else
	continue;
    } //if
    transport_in_progress = true;

    if (length) {
      auto *radiator = prev ? prev->GetRadiator() : 0;

      //*length += (boundary->m_ImpactPoint - x0).Mag()*(radiator ? radiator->n() : 1.0);
      *length += (boundary->m_ImpactPoint - x0).Mag()*(radiator ? mcparticle->n(radiator) : 1.0);
    } //if

    boundary->m_IncomingDirection = (boundary->m_ImpactPoint - x0).Unit();
    TVector3 ns = surface->GetNormal(boundary->m_ImpactPoint);
    TVector3 na = ns.Cross(boundary->m_IncomingDirection);

    boundary->m_OutgoingDirection = boundary->m_IncomingDirection;
    // Must be the sensor dump; FIXME: do this check better;
    if (!boundary->m_Radiator.GetObject()) return true;

    if (boundary->m_Refractive) {
      // Will not be able to determine the refractive index;
      if (!prev) return false;
      //double n1 = prev->GetRadiator()->n(), n2 = boundary->GetRadiator()->n();
      double n1 = mcparticle->n(prev->GetRadiator()), n2 =mcparticle->n(boundary->GetRadiator());
      
      // Refraction; check that the refractive indices are different;
      if (n1 != n2) {
	double theta1 = acos(boundary->m_IncomingDirection.Dot(-1*ns));
	double arg = (n1/n2)*sin(theta1);
	if (fabs(arg) > 1.0) return false;
	double theta2 = asin(arg);

	boundary->m_OutgoingDirection.Rotate(theta1 - theta2, na);
      } //if
    } else {
      // Reflection;
      boundary->m_OutgoingDirection.Rotate(M_PI - 2*acos(ns.Dot(boundary->m_IncomingDirection)), na);
    } //if

      // Prepare input for the next boundary;
    x0 = boundary->m_ImpactPoint;
    n0 = boundary->m_OutgoingDirection;
  } //for boundary

  return true;
} // IRT::Transport()

// -------------------------------------------------------------------------------------
//
// Seeds built from the geometry alone. A specular bounce is the stationary point of
// |xfrom - P| + |P - xto| with P on the mirror: reflect xto in the tangent plane at a
// trial P, intersect xfrom -> xto' with the mirror, and iterate. The tangent plane of a
// cone at P is isometric to its development, so this iteration IS what an unrolled-cone
// construction solves; what differs is where the search starts and how many solutions are
// looked for:
//   kSeedModePlaneTangent : one start, taken where the chord crosses the mirror;
//   kSeedModeUnrolledCone : starts at a set of windings around the axis, which enumerates
//                           the additional specular solutions of a cone (and, by the same
//                           token, the multi-bounce candidates that a single chord start
//                           can never reach).
// The resulting emitted-ray directions are handed to the existing Newton iteration as
// seeds, exactly like the MC truth ones.
//
bool IRT::MirrorsAreConical( void ) const
{
  bool any = false;

  for(unsigned iq=0; iq<_m_OpticalBoundaries.size(); iq++) {
    auto boundary = GetOpticalBoundary(iq);
    if (!boundary || !boundary->m_Surface || boundary->m_Refractive) continue;

    any = true;
    if (!dynamic_cast<const ConicalSurface*>(boundary->m_Surface)) return false;
  } //for boundary

  return any;
} // IRT::MirrorsAreConical()

// -------------------------------------------------------------------------------------

bool IRT::BuildDirectionSeeds(const TVector3 &xfrom, const TVector3 &xto, unsigned mode,
                              std::vector<TVector3> *seeds) const
{
  if (!seeds) return false;
  seeds->clear();

  // First mirror in this optical path;
  const ParametricSurface *surf = 0;
  for(unsigned iq=0; iq<_m_OpticalBoundaries.size(); iq++) {
    auto boundary = GetOpticalBoundary(iq);
    if (!boundary || !boundary->m_Surface || boundary->m_Refractive) continue;
    surf = boundary->m_Surface;
    break;
  } //for boundary

  // A path without mirrors (the direct one) has nothing to reflect: no seed, and the
  // unseeded solve is the right thing;
  if (!surf) return false;   // the direct path has nothing to reflect

  if (mode == kSeedModeUnrolledCone && !MirrorsAreConical()) {
    // dRICH mirrors are spheres: the development argument does not apply there;
    static std::once_flag warned;
    std::call_once(warned, []() {
        printf("IRT: \"unrolled-cone\" photon direction seeding requires conical mirror "
               "surfaces, which this detector does not have - falling back to "
               "\"plane-tangent\"\n");
      });
    mode = kSeedModePlaneTangent;
  } //if

  const TVector3 chord = xto - xfrom;
  const double scale = (chord.Mag() > 1e-9 ? chord.Mag() : 1.0);

  // Iterate the tangent-plane reflection to a specular point; returns the emitted
  // direction when the reflection law holds at the converged point;
  auto specular = [&](const TVector3 &P0, TVector3 *dir) -> bool {
    TVector3 P = P0;

    // Containment test. The "V" range of these surfaces is an azimuth, and the
    // parameterisations do not wrap it into [Vmin, Vmax] - GetV() may return values well
    // outside it - so the periodicity has to be applied before the range check;
    auto inside = [&](const TVector3 &xx) -> bool {
      const double uu = surf->GetU(xx);
      if (uu < surf->Umin() || uu > surf->Umax()) return false;

      double vv = surf->GetV(xx);
      const double lo = surf->Vmin(), hi = surf->Vmax(), period = hi - lo;
      if (vv < lo || vv > hi) {
        if (period <= 0.0) return false;
        vv = lo + std::fmod(vv - lo, period);
        if (vv < lo) vv += period;
        if (vv > hi) return false;
      } //if
      return true;
    }; //lambda inside

    for(unsigned i=0; i<4; i++) {
      const TVector3 nn = surf->GetNormal(P);
      // xto mirrored in the tangent plane at P;
      const TVector3 xm = xto - 2.0*nn*nn.Dot(xto - P);
      const TVector3 d  = xm - xfrom;
      if (d.Mag() < 1e-9) return false;

      TVector3 Pn;
      if (!surf->GetCrossing(xfrom, d.Unit(), &Pn, false)) return false;
      if (!inside(Pn)) return false;

      const bool done = (Pn - P).Mag() < 1e-4*scale;
      P = 0.5*(P + Pn);          // damping: the plain iteration oscillates on a cone
      if (done) { P = Pn; break; } //if
    } //for iteration

    const TVector3 din  = (P - xfrom).Unit(), dout = (xto - P).Unit();
    const TVector3 nn   = surf->GetNormal(P);
    // A seed only has to land in the basin of the specular solution - the Newton
    // iteration in Solve() decides - so the reflection law is used as a soft quality
    // indicator rather than as a hard rejection;
    if (din.Dot(dout) > 0.0) return false;   // the "bounce" would be a straight pass-through
    const double resid = fabs(din.Dot(nn) + dout.Dot(nn));
    if (resid > 0.5) return false;

    *dir = din;
    return true;
  }; //lambda specular

  // Starting points: the chord crossing, and - for the cone - a set of windings;
  std::vector<TVector3> starts;

  TVector3 guess;
  if (surf->GetCrossing(xfrom, chord.Unit(), &guess, false) && surf->IsInside(guess))
    starts.push_back(guess);
  else {
    // Chord misses the mirror: sample the surface so the iteration still gets a chance;
    const double umid = 0.5*(surf->Umin() + surf->Umax());
    const unsigned nstep = 12;
    for(unsigned iq=0; iq<nstep; iq++) {
      const double v = surf->Vmin() + (surf->Vmax() - surf->Vmin())*(iq + 0.5)/nstep;
      starts.push_back(surf->GetSpacePoint(umid, v));
    } //for sampling

    // The chord crossing above failed: use the mirror image of the detection point in the
    // plane through the cone axis as a second guess;
  } //if

  // Either way, start the iteration from a set of points spread over the mirror: on a
  // cone the specular point can be anywhere along the generator, and the development
  // argument says those starts are exactly the windings of the unrolled surface, which is
  // what makes the additional (and multi-bounce) solutions reachable. The unrolled mode
  // simply looks harder;
  {
    const unsigned nstep = (mode == kSeedModeUnrolledCone ? 24 : 12);
    const double ulo = surf->Umin(), uhi = surf->Umax();
    const unsigned nu = (mode == kSeedModeUnrolledCone ? 3 : 2);

    for(unsigned iu=0; iu<nu; iu++) {
      const double u = ulo + (uhi - ulo)*(iu + 1.0)/(nu + 1.0);
      for(unsigned iq=0; iq<nstep; iq++) {
        const double v = surf->Vmin() + (surf->Vmax() - surf->Vmin())*(iq + 0.5)/nstep;
        starts.push_back(surf->GetSpacePoint(u, v));
      } //for azimuth
    } //for slant position
  } //block

  for(auto &P0: starts) {
    TVector3 dir;
    if (!specular(P0, &dir)) continue;

    bool duplicate = false;
    for(auto &s: *seeds)
      if (s.Dot(dir) > std::cos(1e-3)) { duplicate = true; break; } //if
    if (duplicate) continue;

    seeds->push_back(dir);
    if (seeds->size() >= 8) break;   // bounded: the Newton iteration sorts out the rest
  } //for starts

  return !seeds->empty();
} // IRT::BuildDirectionSeeds()

// -------------------------------------------------------------------------------------

  IRTSolution IRT::Solve(ChargedParticle *mcparticle,
			 const TVector3 &xfrom, const TVector3 &nfrom, const TVector3 &xto, 
			 const TVector3 &beam, bool derivatives, const IRTSolution *seed)
{ 
  IRTSolution solution; 
  if (!_m_OpticalBoundaries.size()) return solution;

  // Simplify the situation for now: assume a single flat surface at the end;
  auto sensor = dynamic_cast<const LocalCoordinatesXY*>(tail()->m_Surface);
  if (!sensor) return solution;
  
  // XY in the local sensor coordinate system; 
  double m0[2] = {sensor->GetLocalX(xto), sensor->GetLocalY(xto)};
  
  return Solve(mcparticle, xfrom, nfrom, m0, beam, derivatives, seed);
} // IRT::Solve()

// -------------------------------------------------------------------------------------

  IRTSolution IRT::Solve(ChargedParticle *mcparticle,
			 const TVector3 &xfrom, const TVector3 &nfrom, const double m0[2], 
		       const TVector3 &beam, bool derivatives, const IRTSolution *seed)
{
  IRTSolution solution; if (seed) solution.Set(seed);
  if (!_m_OpticalBoundaries.size()) return solution;

  // Simplify the situation for now: assume a single flat surface at the end;
  auto sensor = dynamic_cast<const LocalCoordinatesXY*>(tail()->m_Surface);
  if (!sensor) return solution;

  // Enter infinite loop; carelessly assume that [x0,n0] is a good first approximation 
  // to shoot; since the refraction on aerogel/C2F6 boundary kicks propagation out of 2D, 
  // and there can be an additional flat mirror installed, there is no good reason to 
  // try solving the initial approximation analytically;
  if (!seed) {
    solution.m_Theta = nfrom.Theta(); 
    solution.m_Phi   = nfrom.Phi();
  } //if

  for(unsigned itr=0; ; itr++ ) {
    double length;
    if (itr == m_IterationLimit) return solution;
    {
      auto nn = TVector3(sin(solution.m_Theta)*cos(solution.m_Phi), 
			 sin(solution.m_Theta)*sin(solution.m_Phi), 
			 cos(solution.m_Theta));
      if (!Transport(mcparticle, xfrom, nn, &length)) return solution;
    }
    double mc[2] = {sensor->GetLocalX(tail()->m_ImpactPoint), sensor->GetLocalY(tail()->m_ImpactPoint)};
    
    // Check the transported-to-measured 2D distance; if it is small enough, return;
    {
      double dist = sqrt(pow(mc[0] - m0[0], 2) + pow(mc[1] - m0[1], 2));
      
      if (dist < m_Precision) {
	solution.m_Converged = true;
	solution.m_Length = length;

	{ 
	  double slope = acos(nfrom.Dot(beam));
	  auto axis = nfrom.Cross(beam).Unit();
	  auto nn = TVector3(sin(solution.m_Theta)*cos(solution.m_Phi), 
			     sin(solution.m_Theta)*sin(solution.m_Phi), 
			     cos(solution.m_Theta));
	  solution.m_Direction = nn;
	  nn.Rotate(slope, axis); 

	  solution.m_Theta = nn.Theta();
	  // Yes, as of 2023/03/02 subtract the changed particle phi angle;
	  solution.m_Phi   = nn.Phi() - nfrom.Phi();
	  //printf(" -> %7.2f [mrad], %7.2f [degree]\n", 1000*solution.m_Theta, (180/M_PI)*solution.m_Phi);
	}

	// Calculate derivatives if needed; FIXME: coding style;
	if (derivatives) {
	  // NB: here of course do not need any recursive derivative calculations; also want 
	  // to help the convergence -> provide a reasonable first approximation;
	  {
	    double mq0[2] = {m0[0] - _IRT_DERIVATIVE_XYZ_STEP_, m0[1]};
	    auto s0 = Solve(mcparticle, xfrom, nfrom, mq0, beam, 0, &solution);
	    double mq1[2] = {m0[0] + _IRT_DERIVATIVE_XYZ_STEP_, m0[1]};
	    auto s1 = Solve(mcparticle, xfrom, nfrom, mq1, beam, 0, &solution);
	    if (s0.Converged() && s1.Converged())
	      solution.m_DtDx = (s1.m_Theta - s0.m_Theta)/(2*_IRT_DERIVATIVE_XYZ_STEP_);
	  }
	  {
	    double mq0[2] = {m0[0], m0[1] - _IRT_DERIVATIVE_XYZ_STEP_};
	    auto s0 = Solve(mcparticle, xfrom, nfrom, mq0, beam, 0, &solution);
	    double mq1[2] = {m0[0], m0[1] + _IRT_DERIVATIVE_XYZ_STEP_};
	    auto s1 = Solve(mcparticle, xfrom, nfrom, mq1, beam, 0, &solution);
	    if (s0.Converged() && s1.Converged())
	      solution.m_DtDy = (s1.m_Theta - s0.m_Theta)/(2*_IRT_DERIVATIVE_XYZ_STEP_);
	  }
	  {
	    auto s0 = Solve(mcparticle, xfrom - _IRT_DERIVATIVE_XYZ_STEP_*nfrom, nfrom, m0, beam, 0, &solution);
	    auto s1 = Solve(mcparticle, xfrom + _IRT_DERIVATIVE_XYZ_STEP_*nfrom, nfrom, m0, beam, 0, &solution);
	    if (s0.Converged() && s1.Converged())
	      solution.m_DtDz = (s1.m_Theta - s0.m_Theta)/(2*_IRT_DERIVATIVE_XYZ_STEP_);
	  }

	  // And eventually calculate a quadratic error estimate; 
	  //solution.CalculateSigmaThetaEstimate();
	} //if

	return solution;
      } //if
    }

    // Calculate the next iteration; Gauss-Newton algorithm; FIXME: rewrite in a covariant R^2 -> R^2 fashion;
    {
      {
	double theta = solution.m_Theta + m_JacobianStep;
	auto nn = TVector3(sin(theta)*cos(solution.m_Phi), sin(theta)*sin(solution.m_Phi), cos(theta));
	if (!Transport(mcparticle, xfrom, nn)) return solution;
      }
      double mt[2] = {sensor->GetLocalX(tail()->m_ImpactPoint), sensor->GetLocalY(tail()->m_ImpactPoint)};
      
      {
	double phi = solution.m_Phi + m_JacobianStep;
	auto nn = TVector3(sin(solution.m_Theta)*cos(phi), sin(solution.m_Theta)*sin(phi), cos(solution.m_Theta));
	if (!Transport(mcparticle, xfrom, nn)) return solution;
      }
      double mf[2] = {sensor->GetLocalX(tail()->m_ImpactPoint), sensor->GetLocalY(tail()->m_ImpactPoint)};
      
      // Calculate Jacobian;
      double jac[2][2], inv[2][2];
      for(unsigned ip=0; ip<2; ip++) 
	for(unsigned iq=0; iq<2; iq++) 
	  jac[ip][iq] = ((ip ? mf[iq] : mt[iq]) - mc[iq])/m_JacobianStep;
      double det = jac[0][0]*jac[1][1] - jac[0][1]*jac[1][0];
      if (!det) return solution;
      
      // Perform matrix inversion by hand;
      inv[0][0] = jac[1][1]; inv[0][1] = -jac[0][1]; inv[1][0] = -jac[1][0]; inv[1][1] = jac[0][0];
      for(unsigned ip=0; ip<2; ip++) 
	for(unsigned iq=0; iq<2; iq++) 
	  inv[ip][iq] /= det;   
      
      {
	double *vec[2] = {&solution.m_Theta, &solution.m_Phi};
	
	for(unsigned ip=0; ip<2; ip++) 
	  for(unsigned iq=0; iq<2; iq++) 
	    *vec[ip] -= inv[iq][ip]*(mc[iq] - m0[iq]);
      }
    }
  } //for inf
} // IRT::Solve()

// -------------------------------------------------------------------------------------

} // namespace IRT2
