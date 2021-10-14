
#include <TObject.h>

#ifndef _TRANSIENT_PARTICLE_
#define _TRANSIENT_PARTICLE_

// Need a separate class because do not want to deal with G4VUserTrackInformation serialization;
class TransientParticle: public TObject {
 public:
 TransientParticle(int pdg): m_PDG(pdg) {};
  ~TransientParticle() {};

  virtual bool IsCharged( void ) const = 0;
  int GetPDG( void ) const { return m_PDG; };

 private:
  int m_PDG;

  ClassDef(TransientParticle, 1);
};

#endif
