
#include <vector>
#include <map>

#include <TString.h>

#ifndef _CHERENKOV_DETECTOR_
#define _CHERENKOV_DETECTOR_

#include <CherenkovPhotonDetector.h>
class CherenkovMirrorGroup;
class OpticalBoundary;
class G4LogicalVolume;

class CherenkovDetector: public TObject {
 public:
 CherenkovDetector(const char *name = 0): m_ContainerVolume(0), m_Name(name ? name : "") {};
  ~CherenkovDetector() {};

  void AddOpticalBoundary(unsigned sector, OpticalBoundary *boundary) {
    _m_OpticalBoundaries[sector].push_back(boundary);
  };

  void AddRadiator(const char *name, CherenkovRadiator *radiator) { 
    _m_Radiators[name] = radiator; 
  };
  //+void AddMirrorGroup(CherenkovMirrorGroup *mgroup) { m_MirrorGroups.push_back(mgroup); };

  // FIXME: "sector" is in fact *some* index rather than an azimuthal segmentation index;
  void AddPhotonDetector(/*unsigned sector,*/ CherenkovPhotonDetector *pd) { 
    m_PhotonDetectors.push_back(pd); 

#if _WQ_
    if (_m_OpticalBoundaries.find(sector) != _m_OpticalBoundaries.end())
      for(auto boundary: _m_OpticalBoundaries[sector])
	pd->GetIRT()->AddOpticalBoundary(boundary);

    pd->AddItselfToOpticalBoundaries();
#endif
  };
  void CreatePhotonDetectorInstance(unsigned sector, CherenkovPhotonDetector *pd, unsigned icopy, 
				    ParametricSurface *surface) {
    auto irt = pd->AllocateIRT(icopy);
    //printf("@Q@ %ld\n", _m_OpticalBoundaries[0].size());
    if (_m_OpticalBoundaries.find(sector) != _m_OpticalBoundaries.end())
      for(auto boundary: _m_OpticalBoundaries[sector])
	/*pd->GetIRT()*/irt->AddOpticalBoundary(boundary);

    // The "0" implicitly says "there is no photon life beyond this boundary in IRT"; 
    // FIXME: will have to add the finite thickness window material as well;
    //pd->GetIRT()->AddOpticalBoundary(new OpticalBoundary(0, surface, true));
    pd->AddItselfToOpticalBoundaries(irt, surface);
  };

  //unsigned GetRadiatorCount( void ) const { return m_Radiators.size(); };
  // FIXME: this kind of denies 'private' access;
  //std::vector<CherenkovRadiator*> &Radiators( void ) { return m_Radiators; };
  std::map<TString, CherenkovRadiator*> &Radiators( void ) { return _m_Radiators; };

  //std::vector<std::pair<unsigned, OpticalBoundary*>> _m_OpticalBoundaries;
  std::map<unsigned, std::vector<OpticalBoundary*>> _m_OpticalBoundaries;
  std::vector<CherenkovPhotonDetector*> m_PhotonDetectors; 

  void SetContainerVolume(const G4LogicalVolume *lv) { m_ContainerVolume = lv; };

  const G4LogicalVolume *m_ContainerVolume; //!

  const char *GetName( void ) const { return m_Name.Data(); };

  CherenkovRadiator *GetRadiator(const char *name) {
    if (_m_Radiators.find(name) == _m_Radiators.end()) return 0;

    return _m_Radiators[name];
  };

 private:  
  TString m_Name;

  std::map<TString, CherenkovRadiator*> _m_Radiators;
  //std::vector<CherenkovRadiator*> m_Radiators;
  //+std::vector<CherenkovMirrorGroup*> m_MirrorGroups;

  ClassDef(CherenkovDetector, 3);
};

#endif
