
#include <TObject.h>
#include <TVector3.h>

#ifndef _CHARGED_PARTICLE_STEP_
#define _CHARGED_PARTICLE_STEP_

class ChargedParticleStep: public TObject {
 public:
  ChargedParticleStep()/*: m_Length(0.0)*/ {};
 ChargedParticleStep(const TVector3 &position, const TVector3 &momentum/*, double length*/):
  m_Position(position), m_Momentum(momentum)/*, m_Length(length)*/ {};
  ~ChargedParticleStep() {};

  inline const TVector3 &GetPosition ( void ) const { return m_Position; };
  inline const TVector3 &GetMomentum ( void ) const { return m_Momentum; };
  inline       TVector3  GetDirection( void ) const { return m_Momentum.Unit(); };

  // Nodes along the particle trajectory; 
 private:
  TVector3 m_Position, m_Momentum;
  //double m_Length;

#ifdef WITH_IRT_ROOT_IO
  ClassDef(ChargedParticleStep, 1);
#endif
};

#endif
