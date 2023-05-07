
#include <set>

#ifndef _CHERENKOV_EVENT_
#define _CHERENKOV_EVENT_

#include "ChargedParticle.h"

class CherenkovEvent: public TObject {
 public:
  CherenkovEvent() {};
  ~CherenkovEvent() { Reset(); };

  void Reset( void ) { 
    for(auto particle: m_ChargedParticles)
      delete particle;

    m_ChargedParticles.clear();
  };

  inline void AddChargedParticle(ChargedParticle *particle) { m_ChargedParticles.insert(particle); };
  std::set<ChargedParticle*> &ChargedParticles( void ) { return m_ChargedParticles; };

 private:
  std::set<ChargedParticle*> m_ChargedParticles;

#ifndef DISABLE_ROOT_IO
  ClassDef(CherenkovEvent, 1);
#endif
};

#endif
