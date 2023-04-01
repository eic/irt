
#include <vector>

#include <TRef.h>

#ifndef _CHARGED_PARTICLE_
#define _CHARGED_PARTICLE_

#include "CherenkovRadiator.h"
#include "RadiatorHistory.h"
#include "TransientParticle.h"
#include "CherenkovPID.h"

class ChargedParticle: public TransientParticle {
 public:
 ChargedParticle(int pdg = 0): TransientParticle(pdg), m_StopTracing(false) {};
  ~ChargedParticle() {  
    for(auto radiator: m_RadiatorHistory)
      delete radiator.second;

    // Typically will be empty anyway;
    if (m_OrphanPhotons.size()) {
      for(auto photon: m_OrphanPhotons)
	delete photon;
      m_OrphanPhotons.clear(); 
    } //if
     
    m_RadiatorHistory.clear();
  };

  bool IsCharged( void ) const { return true; };
  
  // FIXME: TRef does not allow one to use std::map -> resort to a vector; then 
  // have to do a stupid search; a couple of elements, so more intelligent schemes
  // are not really needed;
  inline RadiatorHistory *FindRadiatorHistory(CherenkovRadiator *radiator) const {
    for(auto rpair: m_RadiatorHistory)
      if (rpair.first == radiator)
	return rpair.second;
      
    return 0;
  };

  inline void AddOrphanPhoton(OpticalPhoton *photon) { m_OrphanPhotons.push_back(photon); };
  inline void StopTracing( void ) { m_StopTracing = true; };
  inline bool TracingIsStopped( void ) const { return m_StopTracing; };
  inline void StartRadiatorHistory(const std::pair<CherenkovRadiator*, RadiatorHistory*> &history) {
    m_RadiatorHistory.push_back(history);
  };
  inline /*const*/ std::vector<std::pair<TRef, RadiatorHistory*> > &GetRadiatorHistory( void ) /*const*/ {
    return m_RadiatorHistory;
  };

  // FIXME: this interface should be improved; 
  CherenkovRadiator *GetRadiator(const std::pair<TRef, RadiatorHistory*> &entry) {
    return dynamic_cast<CherenkovRadiator*>(entry.first.GetObject());
  };
  // FIXME: one of the dd4hep interface hacks;
  void SetRadiator(std::pair<TRef, RadiatorHistory*> &entry, CherenkovRadiator *radiator) {
    entry.first = radiator;
  };
  RadiatorHistory *GetHistory(/*const*/ std::pair<TRef, RadiatorHistory*> &entry) {
    return entry.second;
  };

  // Single particle case for now;
  void PIDReconstruction(CherenkovPID &pid);

 private:
  // Optical photons produced elsewhere;
  std::vector<OpticalPhoton*> m_OrphanPhotons; 

  // Group steps by radiator of course; in the easiest case an entry and exit points;
  std::vector<std::pair<TRef, RadiatorHistory*> > m_RadiatorHistory;

  bool m_StopTracing; //!

#ifndef DISABLE_ROOT_IO
  ClassDef(ChargedParticle, 1);
#endif
};

#endif
