
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
 CherenkovDetector(const char *name = 0): /*m_ContainerVolume(0),*/ m_Name(name ? name : "") {};
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
  void CreatePhotonDetectorInstance(unsigned sector, CherenkovPhotonDetector *pd, unsigned icopy, 
				    ParametricSurface *surface) {
    auto irt = pd->AllocateIRT(icopy);

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

 private:  
  TString m_Name;

  std::map<TString, CherenkovRadiator*> _m_Radiators;
  //+std::vector<CherenkovMirrorGroup*> m_MirrorGroups;

  ClassDef(CherenkovDetector, 3);
};

#endif
