
#include <set>

class TH1D;

#ifndef _CONFIGURATION_
#define _CONFIGURATION_

#include "GeantImport.h"
class CherenkovRadiator;

class Configuration: public virtual GeantImport {
 public:
  Configuration();
  virtual ~Configuration() {};

  void SetDefaultSinglePhotonThetaResolution(double value) {
    m_DefaultSinglePhotonThetaResolution = value;
  };
  CherenkovRadiator *UseRadiator(const char *rname);//, double smearing);

  void SetQuietMode( void )       { m_VerboseMode = false; };

  TH1D *hthph()             const { return m_hthph;   };
  TH1D *hccdfph()           const { return m_hccdfph; };
  TH1D *hccdftr()           const { return m_hccdftr; };
  TH1D *hccdfev()           const { return m_hccdfev; };
  TH1D *hnpetr()            const { return m_hnpetr;  };
  TH1D *hdtph()             const { return m_hdtph;   };
  TH1D *hmatch()            const { return m_hmatch;  };
  TH1D *hwl()               const { return m_hwl;  };
  TH1D *hvtx()              const { return m_hvtx;  };
  TH1D *hri()               const { return m_hri;  };
  
  TH1D *m_hthph, *m_hccdfph, *m_hccdftr, *m_hccdfev, *m_hnpetr, *m_hdtph, *m_hmatch, *m_hwl, *m_hvtx, *m_hri;

 protected:
  bool BeVerbose( void )    const { return m_VerboseMode;; };
  bool IsSelected(CherenkovRadiator *radiator) const {
    return radiator && m_SelectedRadiators.find(radiator) != m_SelectedRadiators.end();
  };

  std::set<CherenkovRadiator*> m_SelectedRadiators;

  double GetDefaultSinglePhotonThetaResolution( void ) const {
    return m_DefaultSinglePhotonThetaResolution;
  };

 private:
  bool m_VerboseMode;
  double m_DefaultSinglePhotonThetaResolution;
};

#endif
