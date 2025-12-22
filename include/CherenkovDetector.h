
#pragma once

#include <vector>
#include <map>

#include <TRef.h>
#include <TString.h>

#ifndef _CHERENKOV_DETECTOR_
#define _CHERENKOV_DETECTOR_

#include "CherenkovPhotonDetector.h"
class CherenkovMirrorGroup;
class OpticalBoundary;
class G4LogicalVolume;

class CherenkovDetector: public TObject {
 public:
 CherenkovDetector(const char *name = 0): /*m_ContainerVolume(0),*/ m_Name(name ? name : ""), 
					  m_ReadoutCellMask(0x0), m_SectorCount(0), m_SectorPhase(0.0)
					  /*, m_SectorBoundaryOffset(0.0)*/ {};
  ~CherenkovDetector() {};

  enum ud {Upstream, Downstream};

  void AddOpticalBoundary(CherenkovDetector::ud where, unsigned sector, OpticalBoundary *boundary) {
    m_OpticalBoundaries[where][sector].push_back(boundary);
  };

  void AddRadiator(const char *name, CherenkovRadiator *radiator) { 
    _m_Radiators[name] = radiator; 
  };

  void SetSectorCount(unsigned count) { m_SectorCount = count; };
  void SetSectorPhase(double phase)   {m_SectorPhase = phase; };
  
  // FIXME: "sector" is in fact *some* index rather than an azimuthal segmentation index;
  void AddPhotonDetector(CherenkovPhotonDetector *pd) { 
    m_PhotonDetectors.push_back(pd); 
  };
  void CreatePhotonDetectorInstance(unsigned sector, CherenkovPhotonDetector *pd, 
				    uint64_t icopy, ParametricSurface *surface) {
    auto irt = pd->AllocateIRT(sector, icopy);

    for(unsigned ud=0; ud<2; ud++)
      if (m_OpticalBoundaries[ud].find(sector) != m_OpticalBoundaries[ud].end())
	for(auto boundary: m_OpticalBoundaries[ud][sector])
	  irt->AddOpticalBoundary(boundary);
 
    pd->AddItselfToOpticalBoundaries(irt, surface);
  };

  //unsigned GetRadiatorCount( void ) const { return m_Radiators.size(); };
  // FIXME: this kind of denies 'private' access;
  std::map<TString, CherenkovRadiator*> &Radiators( void ) { return _m_Radiators; };

  std::map<unsigned, std::vector<OpticalBoundary*>> m_OpticalBoundaries[2];
  std::vector<CherenkovPhotonDetector*> m_PhotonDetectors; 

  //void SetContainerVolume(const G4LogicalVolume *lv) { m_ContainerVolume = lv; };
  //const G4LogicalVolume *m_ContainerVolume; //!
  void SetContainerVolume(CherenkovRadiator *radiator) { m_ContainerVolume = radiator; };
  CherenkovRadiator *GetContainerVolume( void ) const { 
    return dynamic_cast<CherenkovRadiator*>(m_ContainerVolume.GetObject()); };
  TRef m_ContainerVolume; 

  const char *GetName( void ) const { return m_Name.Data(); };

  CherenkovRadiator *GetRadiator(const char *name) {
    if (_m_Radiators.find(name) == _m_Radiators.end()) return 0;

    return _m_Radiators[name];
  };
  bool RadiatorRegistered(const CherenkovRadiator *radiator) {
    for(auto &ptr: _m_Radiators)
      if (ptr.second == radiator)
	return true;

    return false;
  };
  // FIXME: create a lookup table;
  const char *GetRadiatorName(const CherenkovRadiator *radiator) {
    for(auto &ptr: _m_Radiators)
      if (ptr.second == radiator)
	return ptr.first.Data();

    return 0;
  };

  void SetReadoutCellMask(uint64_t mask) { m_ReadoutCellMask = mask; };
  inline uint64_t GetReadoutCellMask( void ) const { return m_ReadoutCellMask; };

  unsigned GetSector(const TVector3 &pt) {
    // Either a single "sector" or sector structure not defined yet -> return 0;
    if (m_SectorCount <= 1) return 0;

    double bin = 2*M_PI/m_SectorCount;
    
    return (unsigned)floor((pt.Phi() + 4*M_PI - m_SectorPhase)/bin) % m_SectorCount;
  };

  // FIXME: get rid of the second argument here;
  CherenkovRadiator *GuessRadiator(const TVector3 &x0, const TVector3 &n0) {
    // Determine sector (in EIC DRICH terminology);
    unsigned isec = GetSector(x0);
    
    // FIXME: may want to do a better check; 
    if (m_OpticalBoundaries[CherenkovDetector::Upstream][isec].empty()) return 0;

    // Now loop through all radiators, and check boundaries in this sector;
    for(auto rptr: _m_Radiators) {
      const auto radiator = rptr.second;
      
      // Front and rear surfaces for this particular sector;
      auto s1 = radiator->GetFrontSide(isec);
      auto s2 = radiator->GetRearSide (isec);

      TVector3 from, to;
      // Go backwards and ignore surface orientation mismatch;
      bool b1 = s1->GetCrossing(x0, -1*n0, &from, false);
      bool b2 = s2->GetCrossing(x0,    n0, &to);
      if (!b1 || !b2) continue;

      if ((x0 - from).Dot(to - x0) > 0.0) return radiator;
    } //for radiator
    
    // Seemingly this 3D point does not belong to any radiator;
    return 0;
  };

  void StoreOpticalBoundary(OpticalBoundary *boundary) {
    m_OpticalBoundaryStorage.push_back(boundary);
  };

  // readout ID -> pixel position converter (for external usage)
  std::function<TVector3(long long int)> m_ReadoutIDToPosition; //!

  void ExportJsonFormatCalibrations(const char *fname);
  
 private:  
  TString m_Name;
  // This is needed for dd4hep cell index decoding;
  uint64_t m_ReadoutCellMask;

  // IRT has a TRef by (unfortunate) design -> need a serialized storage buffer to refer to;
  std::vector<OpticalBoundary*> m_OpticalBoundaryStorage;

  // These are needed to calculate to which detector sector (think of a dRICH) a
  // particular 3D point belongs; a respective call is used in two different cases:
  //  (1) to define to which radiator a photon production vertex belongs
  //  (2) to define to which sector a particular sensor hit belongs
  //
  // In principle a photon can be produced in one sector, but be detected in a different
  // one, but this seemingly does not cause any confusion since each radiator is treated
  // "globally", so effectively case #1 is only about using a proper section of a spherical
  // mirror in case of dRICH to define whether a photon vertex was inside or outside of
  // the gas volume;
  unsigned m_SectorCount;
  double m_SectorPhase;
  
  std::map<TString, CherenkovRadiator*> _m_Radiators;

#ifndef DISABLE_ROOT_IO
  ClassDef(CherenkovDetector, 7);
#endif
};

#endif
