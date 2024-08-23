
#include "IRT.h"

thread_local TVector3 OpticalBoundary::m_ImpactPoint, OpticalBoundary::m_IncomingDirection;
thread_local TVector3 OpticalBoundary::m_OutgoingDirection;

// -------------------------------------------------------------------------------------

bool IRT::Transport(const TVector3 &xfrom, const TVector3 &nfrom)
{
  bool transport_in_progress = false;
  bool reflected = false;
  TVector3 x0 = xfrom, n0 = nfrom;
  // Just go through the optical boundaries, and calculate either reflection 
  // or refraction on that particular surface;
  for(unsigned iq=0; iq<_m_OpticalBoundaries.size(); iq++) {
    auto boundary = GetOpticalBoundary(iq), prev = iq ? GetOpticalBoundary(iq-1) : 0;
    auto surface = boundary->m_Surface;

    // if already reflected, skip following mirrors
    if(reflected && boundary->m_Reflective) continue;
    
    bool ok = surface->GetCrossing(x0, n0, &boundary->m_ImpactPoint);
    
    // The logic here is that the first few boundaries may be irrelenat for this 
    // emission point (say for the gas case the emission point is beyond the aerogel-gas
    // refractive boundary; then just skip to the next one without changing [x0,n0]); 
    // however if one valid boundary was handled already, this must be a no-crossing case; 
    // then return false immediately;
    if (!ok) {
      // if a mirror was missed, can still hit the next mirror.
      // if missed a non-mirror boundary, return false
      if (transport_in_progress && !boundary->m_Reflective) 
	return false;
      else
	continue;
    } //if
    transport_in_progress = true;

    boundary->m_IncomingDirection = (boundary->m_ImpactPoint - x0).Unit();
    TVector3 ns = surface->GetNormal(boundary->m_ImpactPoint);
    TVector3 na = ns.Cross(boundary->m_IncomingDirection);

    boundary->m_OutgoingDirection = boundary->m_IncomingDirection;
    // Must be the sensor dump; FIXME:: do this check better;
    if (!boundary->m_Radiator.GetObject() && !boundary->m_Reflective) return true;

    if (boundary->m_Refractive) {
      // Will not be able to determine the refractive index;
      if (!prev) return false;
      double n1 = prev->GetRadiator()->n(), n2 = boundary->GetRadiator()->n();

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
      reflected = true;
      boundary->m_OutgoingDirection.Rotate(M_PI - 2*acos(ns.Dot(boundary->m_IncomingDirection)), na);
    } //if

      // Prepare input for the next boundary;
    x0 = boundary->m_ImpactPoint;
    n0 = boundary->m_OutgoingDirection;
  } //for boundary

  return true;
} // IRT::Transport()

// -------------------------------------------------------------------------------------

IRTSolution IRT::Solve(const TVector3 &xfrom, const TVector3 &nfrom, const TVector3 &xto, 
		       const TVector3 &beam, bool derivatives, const IRTSolution *seed)
{
  IRTSolution solution; 
  if (!_m_OpticalBoundaries.size()) return solution;
  //return solution;

  // Simplify the situation for now: assume a single flat surface at the end;
  auto sensor = dynamic_cast<const LocalCoordinatesXY*>(tail()->m_Surface);
  if (!sensor) return solution;
  
  // XY in the local sensor coordinate system; 
  double m0[2] = {sensor->GetLocalX(xto), sensor->GetLocalY(xto)};

  return Solve(xfrom, nfrom, m0, beam, derivatives, seed);
} // IRT::Solve()

// -------------------------------------------------------------------------------------

IRTSolution IRT::Solve(const TVector3 &xfrom, const TVector3 &nfrom, const double m0[2], 
		       const TVector3 &beam, bool derivatives, const IRTSolution *seed)
{
  //printf("Here-1!\n");
  IRTSolution solution; if (seed) solution = *seed;
  if (!_m_OpticalBoundaries.size()) return solution;

  //printf("Here-2!\n");
  // Simplify the situation for now: assume a single flat surface at the end;
  auto sensor = dynamic_cast<const LocalCoordinatesXY*>(tail()->m_Surface);
  if (!sensor) return solution;

  // Enter infinite loop; carelessly assume that [x0,n0] is a good first approximation 
  // to shoot; since the refraction on aerogel/C2F6 boundary kicks propagation out of 2D, 
  // and there can be an additional flat mirror installed, there is no good reason to 
  // try solving the initial approximation analytically;
  if (!seed) solution.m_Theta = nfrom.Theta(); solution.m_Phi = nfrom.Phi();

  //printf("Here-3!\n");
  for(unsigned itr=0; ; itr++ ) {
    //printf("Here-4!\n");
    if (itr == m_IterationLimit) return solution;
    {
      auto nn = TVector3(sin(solution.m_Theta)*cos(solution.m_Phi), 
			 sin(solution.m_Theta)*sin(solution.m_Phi), 
			 cos(solution.m_Theta));
      if (!Transport(xfrom, nn)) return solution;
    }
    //printf("Here-5!\n");
    double mc[2] = {sensor->GetLocalX(tail()->m_ImpactPoint), sensor->GetLocalY(tail()->m_ImpactPoint)};
    
    // Check the transported-to-measured 2D distance; if it is small enough, return;
    {
      double dist = sqrt(pow(mc[0] - m0[0], 2) + pow(mc[1] - m0[1], 2));
      //printf("%10.4f\n", dist);

      if (dist < m_Precision) {
	solution.m_Converged = true;

	{ 
	  double slope = acos(nfrom.Dot(beam));
	  auto axis = nfrom.Cross(beam).Unit();
	  auto nn = TVector3(sin(solution.m_Theta)*cos(solution.m_Phi), 
			     sin(solution.m_Theta)*sin(solution.m_Phi), 
			     cos(solution.m_Theta));
	  solution.m_Direction = nn;
	  nn.Rotate(slope, axis); 

	  solution.m_Theta = nn.Theta();
	  solution.m_Phi   = nn.Phi();
	  //printf(" -> %7.2f [mrad], %7.2f [degree]\n", 1000*solution.m_Theta, (180/M_PI)*solution.m_Phi);
	}

	// Calculate derivatives if needed; FIXME: coding style;
	if (derivatives) {
	  // NB: here of course do not need any recursive derivative calculations; also want 
	  // to help the convergence -> provide a reasonable first approximation;
	  {
	    double mq0[2] = {m0[0] - _IRT_DERIVATIVE_XYZ_STEP_, m0[1]};
	    auto s0 = Solve(xfrom, nfrom, mq0, beam, 0, &solution);
	    double mq1[2] = {m0[0] + _IRT_DERIVATIVE_XYZ_STEP_, m0[1]};
	    auto s1 = Solve(xfrom, nfrom, mq1, beam, 0, &solution);
	    if (s0.Converged() && s1.Converged())
	      solution.m_DtDx = (s1.m_Theta - s0.m_Theta)/(2*_IRT_DERIVATIVE_XYZ_STEP_);
	  }
	  {
	    double mq0[2] = {m0[0], m0[1] - _IRT_DERIVATIVE_XYZ_STEP_};
	    auto s0 = Solve(xfrom, nfrom, mq0, beam, 0, &solution);
	    double mq1[2] = {m0[0], m0[1] + _IRT_DERIVATIVE_XYZ_STEP_};
	    auto s1 = Solve(xfrom, nfrom, mq1, beam, 0, &solution);
	    if (s0.Converged() && s1.Converged())
	      solution.m_DtDy = (s1.m_Theta - s0.m_Theta)/(2*_IRT_DERIVATIVE_XYZ_STEP_);
	  }
	  {
	    auto s0 = Solve(xfrom - _IRT_DERIVATIVE_XYZ_STEP_*nfrom, nfrom, m0, beam, 0, &solution);
	    auto s1 = Solve(xfrom + _IRT_DERIVATIVE_XYZ_STEP_*nfrom, nfrom, m0, beam, 0, &solution);
	    if (s0.Converged() && s1.Converged())
	      solution.m_DtDz = (s1.m_Theta - s0.m_Theta)/(2*_IRT_DERIVATIVE_XYZ_STEP_);
	  }
#if _WRONG_
	  {
	    // Assume that I'm mostly interested in the refractive index variation in the
	    // media where the photon was created;
	    auto radiator = derivatives;
	    double nref = radiator->n(), step = (nref - 1.0)*_IRT_DERIVATIVE_NNN_STEP_;
	    radiator->SetReferenceRefractiveIndex(nref - step);
	    auto s0 = Solve(xfrom, nfrom, m0, beam, 0, &solution);
	    radiator->SetReferenceRefractiveIndex(nref + step);
	    auto s1 = Solve(xfrom, nfrom, m0, beam, 0, &solution);
	    if (s0.Converged() && s1.Converged())
	      solution.m_DtDn = (s1.m_Theta - s0.m_Theta)/(2*step);

	    // Restore the original value;
	    radiator->SetReferenceRefractiveIndex(nref);
	  }
#endif

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
	if (!Transport(xfrom, nn)) return solution;
      }
      double mt[2] = {sensor->GetLocalX(tail()->m_ImpactPoint), sensor->GetLocalY(tail()->m_ImpactPoint)};
      
      {
	double phi = solution.m_Phi + m_JacobianStep;
	auto nn = TVector3(sin(solution.m_Theta)*cos(phi), sin(solution.m_Theta)*sin(phi), cos(solution.m_Theta));
	if (!Transport(xfrom, nn)) return solution;
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
