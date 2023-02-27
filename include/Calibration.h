
class TDatabasePDG;

#ifndef _CALIBRATION_
#define _CALIBRATION_

#include <Configuration.h>

// 5 degree binning for calibration purposes suffices?;
#define _THETA_BIN_COUNT_ (180/5)
#define _THETA_BIN_WIDTH_ (M_PI / _THETA_BIN_COUNT_)

class Calibration : /*public virtual GeantImport,*/ public virtual Configuration {
 public:
  Calibration ();
  ~Calibration() {};

  TDatabasePDG *m_DatabasePDG;

  void PerformCalibration(unsigned stat = 0);
  void UseAutomaticCalibration( void )        { m_AutomaticCalibrationRequired = true; };
  CherenkovEvent *GetNextEvent(bool calibration = false);

 protected:
  void CalibratePhotonEmissionPoints( void );
  virtual CherenkovEvent *GetEvent(unsigned ev, bool calibration = false) = 0;
  void UpdateYields( void );

  bool AutomaticCalibrationRequired( void ) const { return m_AutomaticCalibrationRequired; };

  //TH1D *hcalib()            const { return m_hcalib;   };

 private:
  bool m_AutomaticCalibrationRequired;

  unsigned m_CurrentEvent;

  bool m_CalibrationBinStat[_THETA_BIN_COUNT_]; 

  //TH1D *m_hcalib;
};


#endif
