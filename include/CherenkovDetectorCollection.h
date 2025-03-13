
#include <map>

#ifndef _CHERENKOV_DETECTOR_COLLECTION_
#define _CHERENKOV_DETECTOR_COLLECTION_

class G4LogicalVolume;
class G4RadiatorMaterial;
class G4OpticalSurface;

//class OpticalPhoton;
class CherenkovMirror;
class CherenkovPhotonDetector;

#include "BitMask.h"
#include "CherenkovDetector.h"

#define _STORE_ORPHAN_PHOTONS_    (0x00000001)
#define _STORE_REFLECTION_POINTS_ (0x00000002)
#define _STORE_REFRACTION_POINTS_ (0x00000003)

class CherenkovDetectorCollection: public BitMask {
 public:
  CherenkovDetectorCollection() {};
  // FIXME: populate the dtor, for completeness;
  ~CherenkovDetectorCollection() {};

  CherenkovDetector *AddNewDetector(const char *name) {
    auto det = new CherenkovDetector(name);
    _m_Detectors[det->GetName()] = det;

    return det;
  };

  CherenkovRadiator *FindOrAddRadiator(CherenkovDetector *det, const char *name, const G4LogicalVolume *volume, 
				 const G4RadiatorMaterial *material) {
    auto radiator = FindRadiator(volume);
    // FIXME: check consistency;
    if (!radiator) {
      radiator = new CherenkovRadiator(volume, material);
      det->AddRadiator(name, radiator);

      m_RadiatorLookup[volume] = radiator;
    } //if

    return radiator;
  };
  CherenkovRadiator *AddFlatRadiator(CherenkovDetector *det, const char *name, CherenkovDetector::ud where, unsigned path, 
				     const G4LogicalVolume *volume, 
				     const G4RadiatorMaterial *material, const FlatSurface *surface, 
				     double thickness) {
    auto radiator = FindOrAddRadiator(det, name, volume, material);

    // Make a pair of local copies; they are stored in their respective class instances, 
    // therefore need two separate ones;
    {
      auto boundary = surface->_Clone(0.0, TVector3(0,0,1));
      boundary->Shift(( thickness/2)*surface->GetNormal());
      det->AddOpticalBoundary(where, path, new OpticalBoundary(radiator,                  boundary, true));
      radiator->m_Borders[path].first = boundary;

      // FIXME: this cannot work for dRICH; and it is a hack anyway;
      if (where == CherenkovDetector::Downstream && !det->GetContainerVolume()->GetRearSide(path)) {
	//printf("@@@ AddFlatRadiator()\n");
	det->GetContainerVolume()->m_Borders[path].second = boundary;
      } //if
    }
    {
      auto boundary = surface->_Clone(0.0, TVector3(0,0,1));
      boundary->Shift((-thickness/2)*surface->GetNormal());
      det->AddOpticalBoundary(where, path, new OpticalBoundary(det->GetContainerVolume(), boundary, true));
      radiator->m_Borders[path].second = boundary;

      // This will most likely be a temporary assignment; only "upstream" boundaries are of interest 
      // here since the "downstream" ones are essentially a sensor-side description;
      if (where == CherenkovDetector::Upstream) det->GetContainerVolume()->m_Borders[path].first = boundary;
    }    

    return radiator;
  };
  void AddRadiatorLogicalVolume(CherenkovRadiator *radiator, const G4LogicalVolume *lv) {
    radiator->AddLogicalVolume(lv);
    m_RadiatorLookup[lv] = radiator;
  };

  //inline void AddOrphanPhoton(OpticalPhoton *photon) { m_OrphanPhotons.push_back(photon); };

  void AddPhotonDetector(CherenkovDetector *det, const G4LogicalVolume *lv, 
			 CherenkovPhotonDetector *pd) {
    // FIXME: a consistency check!;
    if (FindPhotonDetector(lv)) return;

    det->AddPhotonDetector(pd);

    m_PhotonDetectorLookup[lv] = pd;
  };

  //inline unsigned GetDetectorCount( void ) const { return m_Detectors.size(); };

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

  CherenkovRadiator *SetContainerVolume(CherenkovDetector *det, const char *name, unsigned path, 
					const G4LogicalVolume *lv, 
					const G4RadiatorMaterial *material, 
					/*const*/ ParametricSurface *surface) { 
    auto radiator = FindOrAddRadiator(det, name, lv, material);
    // This is most likely a temporary assignment;
    radiator->m_Borders[path].first = surface;

    det->AddOpticalBoundary(CherenkovDetector::Upstream, path, new OpticalBoundary(FindRadiator(lv), surface, true));
    //det->SetContainerVolume(lv);
    det->SetContainerVolume(radiator);

    return radiator;
  };

  // FIXME: do it more efficient later;
  CherenkovDetector *GetDetectorByRadiator(const CherenkovRadiator *radiator) {
    for(auto detector: _m_Detectors)
      for(auto ptr: detector.second->Radiators())
	if (ptr.second == radiator)
	  return detector.second;
      
    return 0;
  };
  CherenkovDetector *GetDetector(const char *name) {
    if (_m_Detectors.find(name) == _m_Detectors.end()) return 0;

    return _m_Detectors[name];
  };
  const std::map<TString, CherenkovDetector*> &GetDetectors( void ) const {
    return _m_Detectors;
  }; 

  // The lookup tables are global of course since the same particle can hit radiators
  // in more than one detector;
 private:
  std::map<const G4LogicalVolume*, CherenkovRadiator*>       m_RadiatorLookup;         //!
  std::map<const G4LogicalVolume*, CherenkovMirror*>         m_MirrorLookup;           //!
  std::map<const G4LogicalVolume*, CherenkovPhotonDetector*> m_PhotonDetectorLookup;   //!

  std::map<TString, CherenkovDetector*> _m_Detectors;
  
  //std::vector<OpticalPhoton*> m_OrphanPhotons; 

  ClassDef(CherenkovDetectorCollection, 2);
};

#endif
