
#include <map>
#include <vector>

#include <TRef.h>
#include <TObject.h>
#include <TVector3.h>
#include <TString.h>
class TH1D;

#ifndef _CHERENKOV_RADIATOR_
#define _CHERENKOV_RADIATOR_

#include <ParametricSurface.h>
class G4LogicalVolume;
class G4RadiatorMaterial;

struct CherenkovRadiatorCalibration {
  //public:
CherenkovRadiatorCalibration(): m_Stat(0), m_AverageRefractiveIndex(0.0), 
    m_AverageZvtx(0.0), m_hcalib(0), m_Coffset(0.0), m_Csigma(0.0) {};
  ~CherenkovRadiatorCalibration() {};

  unsigned m_Stat;
  double m_AverageRefractiveIndex, m_AverageZvtx;

  TH1D *m_hcalib;
  double m_Coffset, m_Csigma;
};

class CherenkovRadiator: public TObject {
 public:
  // NB: do not want to use physical volume here because a particle can cross more than one of them
  // (at the sector boundary), while there is no good reason to separate these contributions;
 CherenkovRadiator(const G4LogicalVolume *volume = 0, const G4RadiatorMaterial *material = 0): 
  /*m_LogicalVolume(volume),*/ m_Material(material), m_OpticalPhotonGenerationEnabled(true),
    m_ReferenceRefractiveIndex(0.0), m_ReferenceAttenuationLength(0.0), 
    m_TrajectoryBinCount(1), m_Smearing(0.0), 
    m_GaussianSmearing(false), m_CalibrationPhotonCount(0), m_DetectedPhotonCount(0), 
    m_DetectedToCalibrationPhotonRatio(0.0), m_YieldStat(0), m_YieldCff(0.0) {
    m_LogicalVolumes.push_back(volume);
  };
  ~CherenkovRadiator() {};

  double n( void )                               const { return m_ReferenceRefractiveIndex; };
  double GetReferenceAttenuationLength( void )   const { return m_ReferenceAttenuationLength; };

  void SetReferenceRefractiveIndex(double n)   { m_ReferenceRefractiveIndex   = n; };
  void SetReferenceAttenuationLength(double l) { m_ReferenceAttenuationLength = l; };

  void AddLogicalVolume(const G4LogicalVolume *volume) { m_LogicalVolumes.push_back(volume); };

  const G4RadiatorMaterial *GetMaterial( void )  const { return m_Material; };

  ParametricSurface *GetFrontSide(unsigned path) {
    if (m_Borders.find(path) == m_Borders.end()) return 0;

    return dynamic_cast<ParametricSurface*>(m_Borders[path].first.GetObject());
  };
  ParametricSurface *GetRearSide(unsigned path) {
    if (m_Borders.find(path) == m_Borders.end()) return 0;

    return dynamic_cast<ParametricSurface*>(m_Borders[path].second.GetObject());
  };
  std::map<unsigned, std::pair<TRef, TRef>> m_Borders; 

  // Material name in dd4hep world;
  void SetAlternativeMaterialName(const char *name) { m_AlternativeMaterialName = name; };
  const char *GetAlternativeMaterialName( void ) const { 
    return m_AlternativeMaterialName.Data();
  };

  void DisableOpticalPhotonGeneration( void ) { m_OpticalPhotonGenerationEnabled = false; };
  bool OpticalPhotonGenerationEnabled( void ) const { return m_OpticalPhotonGenerationEnabled; };

 protected:
  // Run-time variables for the GEANT pass;
  //+const G4LogicalVolume *m_LogicalVolume;          //!
  std::vector<const G4LogicalVolume *> m_LogicalVolumes;          //!
  const G4RadiatorMaterial *m_Material;            //!

 private:
  bool m_OpticalPhotonGenerationEnabled;

  // Refractive index calculated for some fixed reference wave length (supposedly the average 
  // one as seen on the detected photon wave length plot);
  double m_ReferenceRefractiveIndex;
  double m_ReferenceAttenuationLength;

  TString m_AlternativeMaterialName;

 public:
  void SetTrajectoryBinCount(unsigned bins) { m_TrajectoryBinCount = bins; };
  double GetTrajectoryBinCount( void) const { return m_TrajectoryBinCount; };

  inline double GetSmearing( void ) const { return m_Smearing; };
  void SetGaussianSmearing(double sigma) { m_GaussianSmearing = true;  m_Smearing = sigma; }
  void SetUniformSmearing (double range) { m_GaussianSmearing = false; m_Smearing = range; }
  bool UseGaussianSmearing( void )  const { return m_GaussianSmearing; };

  // FIXME: memory leak;
  void ResetLocations( void ) { m_Locations.clear(); }
  void ResetTimes( void ) { m_Times.clear(); }
  void AddLocation(/*unsigned sector,*/ const TVector3 &x, const TVector3 &n) { 
    m_Locations/*[sector]*/.push_back(std::make_pair(x, n)); 
  };
  void AddTime(double value) { m_Times.push_back(value); };

  // Transient variables for the ReconstructionFactory convenience;
  unsigned m_TrajectoryBinCount;                            //!
  // This is a hack for now;
  double m_Smearing;                                        //!
  bool m_GaussianSmearing;                                  //!
  //std::map<unsigned, std::vector<std::pair<TVector3, TVector3>>> _m_Locations; //!
  std::vector<std::pair<TVector3, TVector3>> m_Locations;   //!
  std::vector<double> m_Times;                              //!

  std::vector<std::pair<double, double>> m_ri_lookup_table; //!

  // Overall counts of "calibration" (did not pass QE check) and "real" (passed)
  // photons; since they originate from the same parent distribution, one can choose 
  // basically any way to calculate their effective ratio for Poisson statistics purposes;
  unsigned m_CalibrationPhotonCount;                        //!
  unsigned m_DetectedPhotonCount;                           //!
  double m_DetectedToCalibrationPhotonRatio;                //!

  unsigned m_YieldStat;                                     //!
  double m_YieldCff;                                        //!
  
  std::vector<CherenkovRadiatorCalibration> m_Calibrations; //!

  ClassDef(CherenkovRadiator, 8);
};

#endif
