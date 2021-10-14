
#include <vector>

#include <TObject.h>

#ifndef _RADIATOR_HISTORY_
#define _RADIATOR_HISTORY_

#include <OpticalPhoton.h>
#include <ChargedParticleStep.h>

class RadiatorHistory: public TObject {
 public:
  RadiatorHistory() {};
  ~RadiatorHistory() {
    for(auto photon: m_Photons)
      delete photon;
    m_Photons.clear(); 

    for(auto trace: m_Steps)
      delete trace;
    m_Steps.clear();
  };

  inline unsigned GetStepCount( void ) const { return m_Steps.size(); };
  inline void AddStep(ChargedParticleStep *step) { m_Steps.push_back(step); };
  inline void AddOpticalPhoton(OpticalPhoton *photon) { m_Photons.push_back(photon); };

  inline std::vector<OpticalPhoton*> &Photons( void )     { return m_Photons; };
  inline const ChargedParticleStep *GetStep(unsigned id) const { 
    return (id < m_Steps.size() ? m_Steps[id] : 0); 
  };
  inline unsigned StepCount( void ) { return m_Steps.size(); }; 

  // Charged particle trajectory and optical photons generated in this radiator;
 private:
  std::vector<OpticalPhoton*> m_Photons;
  std::vector<ChargedParticleStep*> m_Steps;

  ClassDef(RadiatorHistory, 1);
};

#endif
