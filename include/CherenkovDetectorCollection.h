
#include <map>

#ifndef _CHERENKOV_DETECTOR_COLLECTION_
#define _CHERENKOV_DETECTOR_COLLECTION_

class G4LogicalVolume;
class G4RadiatorMaterial;
class G4OpticalSurface;

class CherenkovMirror;
class CherenkovPhotonDetector;

#include <BitMask.h>
#include <CherenkovDetector.h>

#define _STORE_ORPHAN_PHOTONS_    (0x00000001)
#define _STORE_REFLECTION_POINTS_ (0x00000002)
#define _STORE_REFRACTION_POINTS_ (0x00000003)

class CherenkovDetectorCollection: public BitMask {
 public:
  CherenkovDetectorCollection() {};
  // FIXME: populate the dtor, for completeness;
  ~CherenkovDetectorCollection() {};

  CherenkovDetector *AddNewDetector( void ) {
    auto det = new CherenkovDetector();
    m_Detectors.push_back(det);

    return det;
  };

  CherenkovRadiator *AddRadiator(CherenkovDetector *det, const G4LogicalVolume *volume, 
				 const G4RadiatorMaterial *material) {
    auto radiator = new CherenkovRadiator(volume, material);
    det->AddRadiator(radiator);

    m_RadiatorLookup[volume] = radiator;
    return radiator;
  };
  void AddFlatRadiator(CherenkovDetector *det, const G4LogicalVolume *volume, 
		       const G4RadiatorMaterial *material, const FlatSurface *surface, double thickness) {
    auto radiator = AddRadiator(det, volume, material);

    // Make a pair of local copies; they are stored in their respective class instances, 
    // therefore need two separate ones;
    {
      auto boundary = surface->Clone(0.0, TVector3(0,0,1));
      boundary->Shift(( thickness/2)*surface->GetNormal());
      det->AddOpticalBoundary(new OpticalBoundary(radiator,                             boundary, true));
    }
    {
      auto boundary = surface->Clone(0.0, TVector3(0,0,1));
      boundary->Shift((-thickness/2)*surface->GetNormal());
      det->AddOpticalBoundary(new OpticalBoundary(FindRadiator(det->m_ContainerVolume), boundary, true));
    }    
  };
  void AddPhotonDetector(CherenkovDetector *det, const G4LogicalVolume *lv, CherenkovPhotonDetector *pd) {
    det->AddPhotonDetector(pd);

    m_PhotonDetectorLookup[lv] = pd;
  };

  inline unsigned GetDetectorCount( void ) const { return m_Detectors.size(); };
  inline CherenkovDetector *GetDetector(unsigned id) {
    return (id < m_Detectors.size() ? m_Detectors[id] : 0);
  };

  inline CherenkovRadiator *FindRadiator(const G4LogicalVolume *lv) {
    return (m_RadiatorLookup.find(lv) == m_RadiatorLookup.end() ? 0 : m_RadiatorLookup[lv]);
  };
  inline CherenkovMirror *FindMirror(const G4LogicalVolume *lv) {
    return (m_MirrorLookup.find(lv) == m_MirrorLookup.end() ? 0 : m_MirrorLookup[lv]);
  };
  inline void AddMirrorLookupEntry(const G4LogicalVolume *lv, CherenkovMirror *mirror) {
    m_MirrorLookup[lv] = mirror;
  };
  inline void AddPhotonDetectorLookupEntry(const G4LogicalVolume *lv, CherenkovPhotonDetector *pd) {
    m_PhotonDetectorLookup[lv] = pd;
  };
  inline CherenkovPhotonDetector *FindPhotonDetector(const G4LogicalVolume *lv) {
    return (m_PhotonDetectorLookup.find(lv) == m_PhotonDetectorLookup.end() ? 0 : m_PhotonDetectorLookup[lv]);
  };

  void SetContainerVolume(CherenkovDetector *det, const G4LogicalVolume *lv, const G4RadiatorMaterial *material, 
			  const ParametricSurface *surface) { 
    AddRadiator(det, lv, material);

    det->AddOpticalBoundary(new OpticalBoundary(FindRadiator(lv), surface, true));
    det->SetContainerVolume(lv);
  };

  // FIXME: do it more efficient later;
  CherenkovDetector *GetDetectorByRadiator(const CherenkovRadiator *radiator) {
    for(auto detector: m_Detectors)
      for(auto ptr: detector->Radiators())
	if (ptr == radiator)
	  return detector;
      
    return 0;
  };

  // The lookup tables are global of course since the same particle can hit radiators
  // in more than one detector;
 private:
  std::map<const G4LogicalVolume*, CherenkovRadiator*>       m_RadiatorLookup;         //!
  std::map<const G4LogicalVolume*, CherenkovMirror*>         m_MirrorLookup;           //!
  std::map<const G4LogicalVolume*, CherenkovPhotonDetector*> m_PhotonDetectorLookup;   //!

  std::vector<CherenkovDetector*> m_Detectors;
  
  ClassDef(CherenkovDetectorCollection, 1);
};

#endif
