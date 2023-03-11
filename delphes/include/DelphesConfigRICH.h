
#ifndef _DELPHES_CONFIG_RICH_
#define _DELPHES_CONFIG_RICH_

#include <DelphesConfig.h>

class DelphesConfigRICH: public DelphesConfig {
 public:
 DelphesConfigRICH( void ): DelphesConfig() {};
 DelphesConfigRICH(const char *dname): DelphesConfig(dname), m_Rindex(0.0), 
    m_AdditionalSmearing(0.0) {};
 ~DelphesConfigRICH() {};

  void SetRefractiveIndex   (double rindex) { m_Rindex = rindex; }
  void SetAdditionalSmearing(double value)  { m_AdditionalSmearing = value; }

  //bool ApplyThresholdModeLogic() { return true; };
  int  Calculate();

  double GenerateMeasurement(int pdg, const TVector3 &momentum);
  MassHypothesis *FindBestHypothesis(const TVector3 &momentum, double theta);

 private:
  double m_Rindex, m_AdditionalSmearing;

  ClassDef(DelphesConfigRICH, 1)
};

#endif
