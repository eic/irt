
#ifndef _DELPHES_CONFIG_DIRC_
#define _DELPHES_CONFIG_DIRC_

#include <DelphesConfig.h>

class DelphesConfigDIRC: public DelphesConfig {
 public:
 DelphesConfigDIRC(const char *dname): DelphesConfig(dname), m_InstallationRadius(0.0),
    m_EtaMin(0.0), m_EtaMax(0.0), m_MomentumMin(0.0), m_MomentumMax(0.0),
    m_EtaBinCount(0), m_MomentumBinCount(0), 
    m_TrackerAngularResolutionA(0.0), m_TrackerAngularResolutionB(0.0), 
    m_MagneticField(0.0) {};
  ~DelphesConfigDIRC() {};
  
  // d(theta) ~ a/p + b; 
  void SetTrackerAngularResolution(double a, double b) { 
    m_TrackerAngularResolutionA = a; 
    m_TrackerAngularResolutionB = b; 
  };

  // FIXME: unify TOF and DIRC;
  void SetInstallationRadius  (double value)     { m_InstallationRadius   = value; }
  void SetMagneticField       (double value)     { m_MagneticField        = value; }

  void SetEtaRange(double min, double max, unsigned ebins) {
    m_EtaMin = min;
    m_EtaMax = max;
    m_EtaBinCount = ebins;
  };  
  void SetMomentumRange(double min, double max, unsigned ebins) {
    m_MomentumMin = min;
    m_MomentumMax = max;
    m_MomentumBinCount = ebins;
  };   

  void SetParameterizationMap(const char *fmap) {
    m_ParameterizationMap = std::string(fmap);
  };

  int DoSigmaCalculations( void );
  // Generic call; here a direct interface to Roman's DrcPidFast;
  int Calculate();
  
 private:
  double m_InstallationRadius;
  double m_EtaMin, m_EtaMax;
  double m_MomentumMin, m_MomentumMax;
  unsigned m_EtaBinCount, m_MomentumBinCount;
  double m_MagneticField;

  double m_TrackerAngularResolutionA, m_TrackerAngularResolutionB;

  std::string m_ParameterizationMap;
};

#endif
