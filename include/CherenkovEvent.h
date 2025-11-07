
#include <set>

#ifndef _CHERENKOV_EVENT_
#define _CHERENKOV_EVENT_

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

  inline void AddChargedParticle(ChargedParticle *particle) { m_ChargedParticles.insert(particle); };
  std::set<ChargedParticle*> &ChargedParticles( void ) { return m_ChargedParticles; };

  inline void AddOrphanPhoton(OpticalPhoton *photon) { m_OrphanPhotons.push_back(photon); };
  std::vector<OpticalPhoton*> &OrphanPhotons( void ) { return m_OrphanPhotons; };

 private:
  std::set<ChargedParticle*> m_ChargedParticles;

  std::vector<OpticalPhoton*> m_OrphanPhotons; 

#ifndef DISABLE_ROOT_IO
  ClassDef(CherenkovEvent, 2);
#endif
};

} // namespace IRT2

#endif
