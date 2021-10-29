
#include <vector>
#include <map>

#ifndef _CHERENKOV_DETECTOR_
#define _CHERENKOV_DETECTOR_

#include <CherenkovPhotonDetector.h>
class CherenkovMirrorGroup;
class OpticalBoundary;
class G4LogicalVolume;

class CherenkovDetector: public TObject {
 public:
 CherenkovDetector(): m_ContainerVolume(0) {};
  ~CherenkovDetector() {};

  void AddOpticalBoundary(unsigned sector, OpticalBoundary *boundary) {
    _m_OpticalBoundaries[sector].push_back(boundary);
  };

  void AddRadiator(CherenkovRadiator *radiator) { m_Radiators.push_back(radiator); };
  //+void AddMirrorGroup(CherenkovMirrorGroup *mgroup) { m_MirrorGroups.push_back(mgroup); };

  // FIXME: "sector" is in fact *some* index rather than an azimuthal segmentation index;
  void AddPhotonDetector(unsigned sector, CherenkovPhotonDetector *pd) { 
    m_PhotonDetectors.push_back(pd); 

    if (_m_OpticalBoundaries.find(sector) != _m_OpticalBoundaries.end())
      for(auto boundary: _m_OpticalBoundaries[sector])
	pd->GetIRT()->AddOpticalBoundary(boundary);

    pd->AddItselfToOpticalBoundaries();
  };

  unsigned GetRadiatorCount( void ) const { return m_Radiators.size(); };
  // FIXME: this kind of denies 'private' access;
  std::vector<CherenkovRadiator*> &Radiators( void ) { return m_Radiators; };

  //std::vector<std::pair<unsigned, OpticalBoundary*>> _m_OpticalBoundaries;
  std::map<unsigned, std::vector<OpticalBoundary*>> _m_OpticalBoundaries;
  std::vector<CherenkovPhotonDetector*> m_PhotonDetectors; 

  void SetContainerVolume(const G4LogicalVolume *lv) { m_ContainerVolume = lv; };

  const G4LogicalVolume *m_ContainerVolume; //!

 private:  
  std::vector<CherenkovRadiator*> m_Radiators;
  //+std::vector<CherenkovMirrorGroup*> m_MirrorGroups;

  ClassDef(CherenkovDetector, 2);
};

#endif
