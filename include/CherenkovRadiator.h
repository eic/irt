
#include <TObject.h>
#include <TVector3.h>

#ifndef _CHERENKOV_RADIATOR_
#define _CHERENKOV_RADIATOR_

class G4LogicalVolume;
class G4RadiatorMaterial;

class CherenkovRadiator: public TObject {
 public:
  // NB: do not want to use physical volume here because a particle can cross more than one of them
  // (at the sector boundary), while there is no good reason to separate these contributions;
 CherenkovRadiator(const G4LogicalVolume *volume = 0, const G4RadiatorMaterial *material = 0): 
  m_LogicalVolume(volume), m_Material(material), 
    m_ReferenceRefractiveIndex(0.0), //+m_ReferenceAbsorbtionLength(0.0), 
    m_Stat(0), m_AverageTheta(0.0) {};
  ~CherenkovRadiator() {};

  double n( void ) const { return m_ReferenceRefractiveIndex; };

  void SetReferenceRefractiveIndex(double n) { m_ReferenceRefractiveIndex = n; };

  const G4RadiatorMaterial *GetMaterial( void ) const { return m_Material; };

 private:
  // Run-time variables for the GEANT pass;
  const G4LogicalVolume *m_LogicalVolume;          //!
  const G4RadiatorMaterial *m_Material;            //!

  // Refractive index calculated for some fixed reference wave length (supposedly the average 
  // one as seen on the detected photon wave length plot);
  double m_ReferenceRefractiveIndex;

 public:
  // Transient variables for the analysis script convenience;
  unsigned m_Stat;                                 //!
  double m_AverageTheta;                           //!
  TVector3 m_AverageVertexPosition;                //!
  double m_AverageRefractiveIndex;                 //!

  ClassDef(CherenkovRadiator, 1);
};

#endif
