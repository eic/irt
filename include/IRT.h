
#include <vector>

#include <TVector3.h>
#include <TObject.h>
#include <TRef.h>

#ifndef _IRT_FACTORY_
#define _IRT_FACTORY_

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

class IRT: public TObject {
 public:
 IRT(unsigned sector = 0): m_Sector(sector), m_IterationLimit(_IRT_ITERATION_LIMIT_), 
    m_Precision(_IRT_PRECISION_DEFAULT_), m_JacobianStep(_IRT_JACOBIAN_STEP_DEFAULT_) {};
  ~IRT() {};

  void AddOpticalBoundary(OpticalBoundary *boundary) {
    _m_OpticalBoundaries.push_back(boundary);
  };

  void SetIterationLimit(unsigned value) { m_IterationLimit = value; };
  //void SetPrecision   (double value) { m_Precision = value; };
  void SetJacobianStep(double value) { m_JacobianStep = value; };

  IRTSolution Solve(const TVector3 &xfrom, const TVector3 &nfrom, const TVector3 &xto, const TVector3 &beam, 
		    //CherenkovRadiator *derivatives = 0, const IRTSolution *seed = 0);
		    bool derivatives = false, const IRTSolution *seed = 0);
  IRTSolution Solve(const TVector3 &xfrom, const TVector3 &nfrom, const double xy[2], const TVector3 &beam, 
		    bool derivatives = false, const IRTSolution *seed = 0);

  unsigned GetSector( void ) const { return m_Sector; };

  inline OpticalBoundary *tail( void ) const { 
    return _m_OpticalBoundaries.size() ? GetOpticalBoundary(_m_OpticalBoundaries.size()-1) : 0;
  };

 private:
  bool Transport(const TVector3 &xfrom, const TVector3 &nfrom); 
  inline OpticalBoundary *GetOpticalBoundary(unsigned id) const { 
    return (id < _m_OpticalBoundaries.size() ? 
    	    dynamic_cast<OpticalBoundary*>(_m_OpticalBoundaries[id].GetObject()) : 0);
    //return (id < _m_OpticalBoundaries.size() ? _m_OpticalBoundaries[id] : 0);
  };

  // FIXME: this is not the right place for this variable;
  unsigned m_Sector;

  unsigned m_IterationLimit;
  double m_Precision, m_JacobianStep; 

  std::vector<TRef> _m_OpticalBoundaries;
  //std::vector<OpticalBoundary *> _m_OpticalBoundaries;

#ifndef DISABLE_ROOT_IO
  ClassDef(IRT, 3);
#endif
};

#endif
