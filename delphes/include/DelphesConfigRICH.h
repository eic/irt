
#ifndef _DELPHES_CONFIG_RICH_
#define _DELPHES_CONFIG_RICH_

#include <DelphesConfig.h>

class DelphesConfigRICH: public DelphesConfig {
 public:
 DelphesConfigRICH(const char *dname): DelphesConfig(dname), m_Rindex(0.0), 
    m_AdditionalSmearing(0.0) {};
 ~DelphesConfigRICH() {};

  void SetRefractiveIndex   (double rindex) { m_Rindex = rindex; }
  void SetAdditionalSmearing(double value)  { m_AdditionalSmearing = value; }

  int  Calculate();

 private:
  double m_Rindex, m_AdditionalSmearing;
};

#endif
