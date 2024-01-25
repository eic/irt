
#include <TObject.h>
#include <TVector3.h>

#ifndef _TRANSIENT_PARTICLE_
#define _TRANSIENT_PARTICLE_

// Need a separate class because do not want to deal with G4VUserTrackInformation serialization;
class TransientParticle: public TObject {
 public:
 TransientParticle(int pdg, bool primary = true): m_PDG(pdg), m_VertexTime(0.0), 
    m_PrimaryParticle(primary) {};
  ~TransientParticle() {};

  virtual bool IsCharged( void ) const = 0;
  int GetPDG( void ) const { return m_PDG; };

  inline void SetVertexPosition(const TVector3 &position)      { m_VertexPosition = position; };
  inline void SetVertexMomentum(const TVector3 &momentum)      { m_VertexMomentum = momentum; };
  inline void SetVertexTime(double value)                      { m_VertexTime = value; };
  inline const TVector3 &GetVertexPosition( void )       const { return m_VertexPosition; };
  inline const TVector3 &GetVertexMomentum( void )       const { return m_VertexMomentum; };
  inline double GetVertexTime( void )                    const { return m_VertexTime; };
  inline bool IsPrimary( void )                          const { return m_PrimaryParticle; };

 private:
  int m_PDG;
  // Vertex and 3D at birth as given by GEANT; 
  TVector3 m_VertexPosition, m_VertexMomentum;

  // Global time at birth;
  double m_VertexTime;

  bool m_PrimaryParticle;

#ifndef DISABLE_ROOT_IO
  ClassDef(TransientParticle, 4);
#endif
};

#endif
