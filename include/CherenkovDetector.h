#include <vector>

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

  void AddOpticalBoundary(OpticalBoundary *boundary) {
    m_OpticalBoundaries.push_back(boundary);
  };

  void AddRadiator(CherenkovRadiator *radiator) { m_Radiators.push_back(radiator); };
  //+void AddMirrorGroup(CherenkovMirrorGroup *mgroup) { m_MirrorGroups.push_back(mgroup); };

  void AddPhotonDetector(CherenkovPhotonDetector *pd) { 
    m_PhotonDetectors.push_back(pd); 

    for(auto boundary: m_OpticalBoundaries)
      pd->GetIRT()->AddOpticalBoundary(boundary);

    pd->AddItselfToOpticalBoundaries();
  };

  unsigned GetRadiatorCount( void ) const { return m_Radiators.size(); };
  // FIXME: this kind of denies 'private' access;
  std::vector<CherenkovRadiator*> &Radiators( void ) { return m_Radiators; };

  std::vector<OpticalBoundary*> m_OpticalBoundaries;
  std::vector<CherenkovPhotonDetector*> m_PhotonDetectors; 

  void SetContainerVolume(const G4LogicalVolume *lv) { m_ContainerVolume = lv; };

  const G4LogicalVolume *m_ContainerVolume; //!

 private:  
  std::vector<CherenkovRadiator*> m_Radiators;
  //+std::vector<CherenkovMirrorGroup*> m_MirrorGroups;

  ClassDef(CherenkovDetector, 1);
};

#endif
