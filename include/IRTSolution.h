#pragma once

#include <TVector3.h>

namespace IRT2 {

class IRTSolution {
  friend class IRT;

  public:
 IRTSolution(): m_Converged(false), m_Theta(0.0), m_SigmaTheta(0.0), m_Phi(0.0), m_Length(0.0), m_Time(0.0),
    m_DtDx(0.0), m_DtDy(0.0), m_DtDz(0.0), /*m_DtDn(0.0),*/m_DtDt(0.0), m_DtDf(0.0), 
    m_SigmaDx(0.0), m_SigmaDy(0.0), m_SigmaDz(0.0), m_SigmaDt(0.0), m_SigmaDf(0.0) {};
  ~IRTSolution() {};

  void SetSeed(const TVector3 &n0) {
    m_Theta = n0.Theta();
    m_Phi   = n0.Phi();
  };

  bool Converged( void )       const { return m_Converged; }
  double GetTheta( void )      const { return m_Theta; };
  double GetSigmaTheta( void ) const { return m_SigmaTheta; };
  double GetPhi( void )        const { return m_Phi; };
#if _LATER_
  IRTSolution& operator = (IRTSolution other) {
    m_Converged = false; m_Theta = other.m_Theta; m_SigmaTheta = 0.0; m_Phi = other.m_Phi;
    return *this;
  };
#else
  void Set(const IRTSolution *other) {
    m_Converged = false; m_Theta = other->m_Theta; m_SigmaTheta = 0.0; m_Phi = other->m_Phi;
    //memcpy(this, other, sizeof(IRTSolution));
  };
#endif
  void SetSigmaDx(double value) { m_SigmaDx = value; };
  void SetSigmaDy(double value) { m_SigmaDy = value; };
  void SetSigmaDz(double value) { m_SigmaDz = value; };
  void SetSigmaDt(double value) { m_SigmaDt = value; };
  void SetSigmaDf(double value) { m_SigmaDf = value; };

  void CalculateSigmaThetaEstimate( void ) {
    //FIXME: do it better later;
    const unsigned dim = 5;
    double sigma     [dim] = {m_SigmaDx, m_SigmaDy, m_SigmaDz, m_SigmaDt, m_SigmaDf};
    double derivative[dim] = {   m_DtDx,    m_DtDy,    m_DtDz,    m_DtDt,     m_DtDf};
    m_SigmaTheta = 0.0;
    for(unsigned iq=0; iq<dim; iq++)
      m_SigmaTheta += pow(sigma[iq]*derivative[iq], 2);
    m_SigmaTheta = sqrt(m_SigmaTheta);
  };

  // This is a reconstructed direction in MARS 3D system; 
  TVector3 m_Direction;

  //private:
  bool m_Converged;
  double m_Theta, m_SigmaTheta, m_Phi, m_Length, m_Time;

  // Derivatives: XY-coordinates on the sensor, emission point, "refractive index" (?), 
  // charged particle track theta and phi; these ones can be evaluated internally;
  double m_DtDx, m_DtDy, m_DtDz/*, m_DtDn*/, m_DtDt, m_DtDf;

  // Qaudratic error estimates as provided by the user; assume tracking errors diagonal;
  double m_SigmaDx, m_SigmaDy, m_SigmaDz, m_SigmaDt, m_SigmaDf;
};

} // namespace IRT2
