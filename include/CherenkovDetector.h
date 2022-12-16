
#include <vector>
#include <map>

#include <TRef.h>
#include <TString.h>

#ifndef _CHERENKOV_DETECTOR_
#define _CHERENKOV_DETECTOR_

#include <CherenkovPhotonDetector.h>
class CherenkovMirrorGroup;
class OpticalBoundary;
class G4LogicalVolume;

class CherenkovDetector: public TObject {
 public:
 CherenkovDetector(const char *name = 0): /*m_ContainerVolume(0),*/ m_Name(name ? name : ""), 
    m_ReadoutCellMask(0x0)/*, m_SectorBoundaryOffset(0.0)*/ {};
  ~CherenkovDetector() {};

  void AddOpticalBoundary(unsigned sector, OpticalBoundary *boundary) {
    _m_OpticalBoundaries[sector].push_back(boundary);
  };

  void AddRadiator(const char *name, CherenkovRadiator *radiator) { 
    _m_Radiators[name] = radiator; 
  };
  //+void AddMirrorGroup(CherenkovMirrorGroup *mgroup) { m_MirrorGroups.push_back(mgroup); };

  // FIXME: "sector" is in fact *some* index rather than an azimuthal segmentation index;
  void AddPhotonDetector(CherenkovPhotonDetector *pd) { 
    m_PhotonDetectors.push_back(pd); 
  };
  void CreatePhotonDetectorInstance(unsigned sector, CherenkovPhotonDetector *pd, 
				    uint64_t icopy, ParametricSurface *surface) {
    auto irt = pd->AllocateIRT(sector, icopy);

    if (_m_OpticalBoundaries.find(sector) != _m_OpticalBoundaries.end())
      for(auto boundary: _m_OpticalBoundaries[sector])
	irt->AddOpticalBoundary(boundary);
 
    pd->AddItselfToOpticalBoundaries(irt, surface);
  };

  //unsigned GetRadiatorCount( void ) const { return m_Radiators.size(); };
  // FIXME: this kind of denies 'private' access;
  std::map<TString, CherenkovRadiator*> &Radiators( void ) { return _m_Radiators; };

  //std::vector<std::pair<unsigned, OpticalBoundary*>> _m_OpticalBoundaries;
  std::map<unsigned, std::vector<OpticalBoundary*>> _m_OpticalBoundaries;
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

  void SetReadoutCellMask(uint64_t mask) { m_ReadoutCellMask = mask; };
  inline uint64_t GetReadoutCellMask( void ) const { return m_ReadoutCellMask; };

  // FIXME: not at all clean (uses implicit phase assumptions); 
  unsigned GetSector(const TVector3 &pt) {
    // FIXME: may require tuning for a dual-mirror setup;
    unsigned nSectors = _m_OpticalBoundaries.size();

    // Either a single "sector" or sector structure not define yet -> return 0;
    if (nSectors <= 1) return 0;

    // FIXME: this offset is only defined by the way Chris positions sector #0; 
    double bin = 2*M_PI/nSectors, offset = -bin/2;
    
    return (unsigned)floor((pt.Phi() + 4*M_PI - offset)/bin) % nSectors;
    //return 0;
  };

  // FIXME: get rid of the second argument here;
  CherenkovRadiator *GuessRadiator(const TVector3 &x0, const TVector3 &n0) {
    // FIXME: may want to do a better check;
    if (_m_OpticalBoundaries.empty()) return 0;

    // Determine sector (in EIC DRICH terminology);
    unsigned isec = GetSector(x0);

    // Now loop through all radiators, and check boundaries in this sector;
    for(auto rptr: _m_Radiators) {
      const auto radiator = rptr.second;
 
      // Front and rear surfaces for this particular sector;
      auto s1 = radiator->GetFrontSide(isec);
      auto s2 = radiator->GetRearSide (isec);

      //printf("Here-A! %f %f %f\n", x0[0], x0[1], x0[2]);
      TVector3 from, to;
      // Go backwards and ignore surface orientation mismatch;
      bool b1 = s1->GetCrossing(x0, -1*n0, &from, false);
      bool b2 = s2->GetCrossing(x0,    n0, &to);
      //printf("Here-B %d %d\n", b1, b2);
      if (!b1 || !b2) continue;

      if ((x0 - from).Dot(to - x0) > 0.0) return radiator;
    } //for radiator
   
    // Seemingly this 3D point does not belong to any radiator;
    return 0;
  };

  // readout ID -> pixel position converter (overridable externally)
  std::function<TVector3(long long int)> m_ReadoutIDToPosition = [] (long long int i) {
    fprintf(stderr,"ERROR: CherenkovRadiator::m_ReadoutIDToPosition not defined\n");
    return TVector3(0.,0.,0.);
  };

 private:  
  TString m_Name;
  // This is needed for dd4hep cell index decoding;
  uint64_t m_ReadoutCellMask;

  //+double m_SectorBoundaryOffset;

  std::map<TString, CherenkovRadiator*> _m_Radiators;
  //+std::vector<CherenkovMirrorGroup*> m_MirrorGroups;

  ClassDef(CherenkovDetector, 5);
};

#endif
