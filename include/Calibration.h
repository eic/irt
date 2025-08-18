
#include <map>

class TDatabasePDG;
#include <TRandom.h>

#ifndef _CALIBRATION_
#define _CALIBRATION_

#include <GeantImport.h>

// 5 degree binning for calibration purposes suffices?;
#define _THETA_BIN_COUNT_ (180/5)
#define _THETA_BIN_WIDTH_ (M_PI / _THETA_BIN_COUNT_)

class Calibration : public virtual GeantImport {
 public:
  Calibration ();
  ~Calibration() {};

  TDatabasePDG *m_DatabasePDG;

  void PerformCalibration(unsigned stat = 0);
  //?void UseAutomaticCalibration( void )        { m_AutomaticCalibrationRequired = true; };
  CherenkovEvent *GetNextEvent(bool calibration = false);

  void ImportTrackingSmearing(const char *ftheta, const char *fphi);

  //?void UseActsTracking( void ) { m_UseActsTracking = true; };
  
  void SetDefaultSinglePhotonThetaResolution(double value) {
    m_DefaultSinglePhotonThetaResolution = value;
  };
  
 protected:
  void CalibratePhotonEmissionPoints( void );
  virtual CherenkovEvent *GetEvent(unsigned ev, bool calibration = false) = 0;
  void UpdateYields( void );

  bool AutomaticCalibrationRequired( void ) const { return m_AutomaticCalibrationRequired; };

  double GetDefaultSinglePhotonThetaResolution( void ) const {
    return m_DefaultSinglePhotonThetaResolution;
  };
  
 private:
  bool m_AutomaticCalibrationRequired;

  unsigned m_CurrentEvent;

  // FIXME: remove;
  unsigned m_CalibrationBinStat[_THETA_BIN_COUNT_]; 

  TRandom m_rndm; 

  bool m_UseActsTracking;

#if _OBSOLETE_
  std::map<std::pair<double, double>, std::pair<double, double>> m_ThetaSmearing, m_PhiSmearing;

  std::pair<double, double> GetTrackingSmearing(double momentum, double eta);
  std::pair<double, double> GetTrackingSmearing(const TVector3 &momentum) {
    return GetTrackingSmearing(momentum.Mag(), momentum.Eta());
  };
#endif
  
  double m_DefaultSinglePhotonThetaResolution;
};


#endif
