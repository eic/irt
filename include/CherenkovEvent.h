#pragma once

#include <vector>

#include "ChargedParticle.h"

class OpticalPhoton;

namespace IRT2 {

class CherenkovEvent: public TObject {
 public:
  CherenkovEvent() {};
  ~CherenkovEvent() { Reset(); };

  void Reset( void ) { 
    for(auto particle: m_ChargedParticles)
      delete particle;
    m_ChargedParticles.clear();

    for(auto photon: m_OrphanPhotons)
      delete photon;
    m_OrphanPhotons.clear();
  };

  inline void AddChargedParticle(ChargedParticle *particle) { m_ChargedParticles.push_back(particle); };
  std::vector<ChargedParticle*> &ChargedParticles( void ) { return m_ChargedParticles; };

  inline void AddOrphanPhoton(OpticalPhoton *photon) { m_OrphanPhotons.push_back(photon); };
  std::vector<OpticalPhoton*> &OrphanPhotons( void ) { return m_OrphanPhotons; };

 private:
  // Insertion-ordered on purpose: iterating a std::set of pointers exposes heap addresses,
  // which makes downstream output ordering vary between runs and thread counts.
  std::vector<ChargedParticle*> m_ChargedParticles;

  std::vector<OpticalPhoton*> m_OrphanPhotons; 

#ifndef DISABLE_ROOT_IO
  ClassDef(CherenkovEvent, 3);
#endif
};

} // namespace IRT2
