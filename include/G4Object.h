#pragma once

#include <TObject.h>

class G4Material;
class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Colour;
class G4VSolid;
class G4OpticalSurface;
// Want to help ROOT dictionary builder, but at the same time have "typedef CLHEP:: ..." working in the source code;
// FIXME: a better way to handle this?;
#ifdef _GEANT_SOURCE_CODE_
#include <G4RotationMatrix.hh>
#include <G4ThreeVector.hh>
#else
class G4RotationMatrix;
class G4ThreeVector;
#endif

namespace IRT2 {

class G4ObjectCopy: public TObject {
 public:
 G4ObjectCopy(G4VPhysicalVolume *phys = 0): m_PhysicalVolume(phys) {};
  ~G4ObjectCopy() {};

  G4VPhysicalVolume *m_PhysicalVolume; //!

#ifndef DISABLE_ROOT_IO
  ClassDef(G4ObjectCopy, 1);
#endif
};

class G4Object: public TObject {
public:
 G4Object(G4VSolid *solid = 0, G4Material *material = 0): m_Solid(solid), m_Material(material), 
    m_LogicalVolume(0)/*, m_CopyCounter(0)*/ {};
  ~G4Object() {};

  void DefineIntersectionCut(G4VSolid *cut, G4RotationMatrix *rotation, const G4ThreeVector &translation, bool flip);
  void DefineSubtractionCut (G4VSolid *cut, G4RotationMatrix *rotation, const G4ThreeVector &translation, bool flip);
  void DefineUnion(G4VSolid *solid);
  G4VSolid *GetSolid( void ) { return m_Solid; };

  void SetColor(const G4Colour &color);

  void PlaceWedgeCopies(G4VPhysicalVolume *mother);
  virtual void AdjustWedgeCopies(G4VPhysicalVolume * /*mother*/) {};

  G4LogicalVolume *GetLogicalVolume( void ) const { return m_LogicalVolume; };
  
  virtual G4ObjectCopy *CreateCopy(G4VPhysicalVolume *phys) { return new G4ObjectCopy(phys); };
  void AddCopy(G4ObjectCopy *copy) { m_Copies.push_back(copy); };
  G4ObjectCopy *GetCopy(unsigned id) const {
    return (id < m_Copies.size() ? m_Copies[id] : 0);
  }; 

  virtual G4OpticalSurface *GetMirrorSurface( void ) const { return 0; };

  G4Material *GetMaterial( void ) const { return m_Material; };

 protected:
  G4VSolid *m_Solid;                   //!
  std::vector<G4ObjectCopy*> m_Copies; //!

 private:
  G4Material *m_Material;              //!

  G4LogicalVolume *m_LogicalVolume;    //!
  //unsigned m_CopyCounter;              //!

 public:
  // This call is deferred to the moment after all cuts on this solid are applied;
  void DefineLogicalVolume( void );

#ifndef DISABLE_ROOT_IO
  ClassDef(G4Object, 2)
#endif
};

} // namespace IRT2
