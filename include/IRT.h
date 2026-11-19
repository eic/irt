#pragma once

#include <vector>

#include <TVector3.h>
#include <TObject.h>
#include <TRef.h>

// Don't even care about the units here; [mm] in fact; just make it practically small;
#define _IRT_PRECISION_DEFAULT_     (1E-7)
// Well, assume radians in [theta,phi] -> 1urad step should be good enough;
#define _IRT_JACOBIAN_STEP_DEFAULT_ (1E-6)
#define _IRT_ITERATION_LIMIT_        ( 50)
// Here perhaps want something tangible, on a [mm] scale (?), matching the uncertaintly 
// in the sensor position measurement, and emission point uncertainty along the trajectory; 
#define _IRT_DERIVATIVE_XYZ_STEP_    (1.00)
// Refractive index variation; in units of the (1-n); ~1% makes sense?;
//#define _IRT_DERIVATIVE_NNN_STEP_    (0.01)

#include "CherenkovRadiator.h"
#include "OpticalBoundary.h"
#include "ParametricSurface.h"
#include "IRTSolution.h"

namespace IRT2 {

class ChargedParticle;
  
class IRT: public TObject {
 public:
  IRT(/*unsigned sector = 0*/): /*m_Sector(sector),*/ m_IterationLimit(_IRT_ITERATION_LIMIT_), 
    m_Precision(_IRT_PRECISION_DEFAULT_), m_JacobianStep(_IRT_JACOBIAN_STEP_DEFAULT_) {};
  ~IRT() {};

  void AddOpticalBoundary(OpticalBoundary *boundary) {
    _m_OpticalBoundaries.push_back(boundary);
  };

  void SetIterationLimit(unsigned value) { m_IterationLimit = value; };
  //void SetPrecision   (double value) { m_Precision = value; };
  void SetJacobianStep(double value) { m_JacobianStep = value; };

  IRTSolution Solve(ChargedParticle *mcparticle,
		    const TVector3 &xfrom, const TVector3 &nfrom, const TVector3 &xto, const TVector3 &beam, 
		    bool derivatives = false, const IRTSolution *seed = 0);

  // Seeds built from the geometry alone, i.e. without any MC truth: the specular impact
  // point(s) of the first mirror in this optical path, converted into emitted-ray
  // directions. Supported modes (see IRT.cc for the construction):
  //   kSeedModePlaneTangent : iterate the tangent-plane reflection starting from the chord
  //                           crossing; works for any mirror surface (dRICH included);
  //   kSeedModeUnrolledCone : the same iteration started from a set of windings of the
  //                           development of the cone, i.e. it also enumerates the extra
  //                           specular solutions of a conical mirror. Requires conical
  //                           mirror surfaces, so it is NOT usable for dRICH;
  // returns false when the path has no mirror, when the mode does not match the mirror
  // geometry, or when no consistent specular point was found;
  enum SeedMode { kSeedModePlaneTangent = 1, kSeedModeUnrolledCone = 2 };

  bool BuildDirectionSeeds(const TVector3 &xfrom, const TVector3 &xto, unsigned mode,
                           std::vector<TVector3> *seeds) const;

  // True when every mirror of this optical path is a cone of revolution;
  bool MirrorsAreConical( void ) const;

  //unsigned GetSector( void ) const { return m_Sector; };

  inline OpticalBoundary *tail( void ) const { 
    return _m_OpticalBoundaries.size() ? GetOpticalBoundary(_m_OpticalBoundaries.size()-1) : 0;
  };

 private:
  bool Transport(ChargedParticle *mcparticle, const TVector3 &xfrom, const TVector3 &nfrom, double *length = 0); 
  inline OpticalBoundary *GetOpticalBoundary(unsigned id) const { 
    return (id < _m_OpticalBoundaries.size() ? 
    	    dynamic_cast<OpticalBoundary*>(_m_OpticalBoundaries[id].GetObject()) : 0);
  };

  IRTSolution Solve(ChargedParticle *mcparticle, const TVector3 &xfrom, const TVector3 &nfrom,
		    const double xy[2], const TVector3 &beam, 
		    bool derivatives = false, const IRTSolution *seed = 0);

  // FIXME: this is not the right place for this variable;
  //unsigned m_Sector;

  unsigned m_IterationLimit;
  double m_Precision, m_JacobianStep; 

  std::vector<TRef> _m_OpticalBoundaries;

#ifndef DISABLE_ROOT_IO
  ClassDef(IRT, 4);
#endif
};

} // namespace IRT2
