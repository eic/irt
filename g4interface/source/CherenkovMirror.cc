#include "G4LogicalBorderSurface.hh"

#define _GEANT_SOURCE_CODE_
//#include <tuning.h>

#include <CherenkovMirror.h>
//#include <EicSandboxDetectorConstruction.h>
#include <G4RadiatorMaterial.h>

//#include <G4Object.h>

// -------------------------------------------------------------------------------------

void CherenkovMirror::SetReflectivity( void )
{
  // Mirror surface; assume either a metal one with a constant reflectivity or 
  // a dielectric one as parameterized by Evaristo;
#ifdef _MIRROR_REFLECTIVITY_ 
  m_MirrorSurface = new G4OpticalSurface(m_Solid->GetName());
  m_MirrorSurface->SetType(dielectric_metal);
  // NB: assume specular reflection, right?;
  m_MirrorSurface->SetFinish(polished);
  m_MirrorSurface->SetModel(unified);    
    
  G4double table[_QGDIM_];
  for(int iq=0; iq<_QGDIM_; iq++) 
    table[iq] = _MIRROR_REFLECTIVITY_;
  
  G4MaterialPropertiesTable *mirrorST = new G4MaterialPropertiesTable();
  mirrorST->AddProperty("REFLECTIVITY", gPhotonEnergy, table, _QGDIM_);
  m_MirrorSurface->SetMaterialPropertiesTable(mirrorST);
#else
  auto mirror = new g4dRIChMirror(m_Solid->GetName().data());
  mirror->setOpticalParams(m_Solid->GetName().data());
  m_MirrorSurface = mirror->m_MirrorSurface;
#endif
} // CherenkovMirror::SetReflectivity()

#if _TODAY_
void EicSandboxMirror::AdjustWedgeCopies(G4VPhysicalVolume *mother) 
{
  printf("@@@@@@@@@@@@@@@@@@\n");// %d\n", mcopy->m_Surface);
  //if (m_MirrorSurface)
  // Do I really need them separately?;
  //char buffer[128]; snprintf(buffer, 128-1, "SphericalMirror");//Surface");//%2d%02d", io, iq);
  //new G4LogicalBorderSurface(m_Solid->GetName(), mother, phys, m_MirrorSurface);
  
  if (m_MirrorSurface)
    for(int iq=0; iq<6; iq++) {
      auto mcopy = m_Copies[iq];
      
      // Do I really need them separately?;
      //char buffer[128]; snprintf(buffer, 128-1, "SphericalMirror");//Surface");//%2d%02d", io, iq);
      new G4LogicalBorderSurface(m_Solid->GetName(), mother, mcopy->m_PhysicalVolume, m_MirrorSurface);
    } //if..for iq
} // EicSandboxMirror::AdjustWedgeCopies()
#endif
// -------------------------------------------------------------------------------------

#if 0
G4VPhysicalVolume *Mirror::PlaceCopy(G4VPhysicalVolume *mother, G4RotationMatrix *rotation, 
				     const G4ThreeVector &translation)
{
  //exit(0);
  auto phys = G4Object::PlaceCopy(mother, rotation, translation);

#if 1//_OK_  
  if (m_MirrorSurface)
    // Do I really need them separately?;
    //char buffer[128]; snprintf(buffer, 128-1, "SphericalMirror");//Surface");//%2d%02d", io, iq);
    new G4LogicalBorderSurface(m_Solid->GetName(), mother, phys, m_MirrorSurface);
#endif  
} // Mirror::PlaceCopy()
#endif
#if 0
void Mirror::ExtraCall(G4VPhysicalVolume *mother, G4RotationMatrix *rotation, 
				     const G4ThreeVector &translation, G4VPhysicalVolume *phys)
{
  //exit(0);
  //auto phys = G4Object::PlaceCopy(mother, rotation, translation);


  if (m_MirrorSurface)
    // Do I really need them separately?;
    //char buffer[128]; snprintf(buffer, 128-1, "SphericalMirror");//Surface");//%2d%02d", io, iq);
    new G4LogicalBorderSurface(m_Solid->GetName(), mother, phys, m_MirrorSurface);
} // Mirror::ExtraCall()
#endif
// -------------------------------------------------------------------------------------
//const G4VPhysicalVolume *phys
