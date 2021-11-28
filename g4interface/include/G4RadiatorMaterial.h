
#include <G4Material.hh>

#ifndef _G4_RADIATOR_MATERIAL_
#define _G4_RADIATOR_MATERIAL_

#include <g4dRIChOptics.hh>

class G4RadiatorMaterial: public G4Material {
 public:
 G4RadiatorMaterial(const char *name, double density, unsigned ncomp, double n = 0.0):
  G4Material(name, density, ncomp), m_ConstantRefractiveIndex(n), m_RIChOptics(0) {};
  ~G4RadiatorMaterial() {};

  //void SetRIChOptics(/*int mode = 0*/);
  double RefractiveIndex(double energy) const { 
    bool ret;
    // FIXME: how about to check the return code?;
    return (m_RIChOptics ? m_RIChOptics->pTable->GetProperty("RINDEX")->GetValue(energy, ret) :
	    m_ConstantRefractiveIndex);
  };

  // Either a constant refractive index or a connection to Evaristo's tables;
  double m_ConstantRefractiveIndex;
  g4dRIChOptics *m_RIChOptics;
};

#endif
