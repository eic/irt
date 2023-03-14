
#ifndef _DELPHES_CONFIG_TOF_
#define _DELPHES_CONFIG_TOF_

#include "DelphesConfig.h"

class DelphesConfigTOF: public DelphesConfig {
 public:
 DelphesConfigTOF(const char *dname): DelphesConfig(dname), 
    m_T0Resolution(0.0), m_DetectorResolution(0.0), m_InstallationRadius(0.0),
    m_EtaMin(0.0), m_EtaMax(0.0), m_MomentumMin(0.0), m_MomentumMax(0.0),
    m_EtaBinCount(0), m_MomentumBinCount(0), m_MagneticField(0.0), 
    m_MomentumResolutionA(0.0), m_MomentumResolutionB(0.0), m_PathLengthResolution(0.0) {};
  ~DelphesConfigTOF() {};
  
  void SetT0Resolution        (double value)     { m_T0Resolution         = value; }
  // Assume a constant one; [mm];
  void SetPathLengthResolution(double value)     { m_PathLengthResolution = value; }
  // dp/p ~ a*p + b; either in momentum or in Pt;
  void SetMomentumResolution(double a, double b) { 
    m_MomentumResolutionA = a; m_MomentumResolutionB = b;
  };
  void SetDetectorResolution  (double value)     { m_DetectorResolution   = value; }
  void SetInstallationRadius  (double value)     { m_InstallationRadius   = value; }
  void SetMagneticField       (double value)     { m_MagneticField        = value; }
  // Well, would be more natural to give Z-range along the beam line;
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

  // TOF-specific call (input parameters -> sigma counts);
  int DoSigmaCalculations( void );

  // In this case TOF measurement always exists -> no zero entries expected;
  //bool ApplyThresholdModeLogic() { return false; };
  // Generic call (sigma -> efficiency);
  int Calculate();
  
 private:
  double m_T0Resolution, m_DetectorResolution, m_InstallationRadius;
  double m_EtaMin, m_EtaMax;
  double m_MomentumMin, m_MomentumMax;
  unsigned m_EtaBinCount, m_MomentumBinCount;
  double m_MagneticField;

  double m_MomentumResolutionA, m_MomentumResolutionB, m_PathLengthResolution;

  double tof(double m, double p, double l);

  ClassDef(DelphesConfigTOF, 1)
};

#endif
