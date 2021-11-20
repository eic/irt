
#ifndef _DELPHES_CONFIG_RICH_
#define _DELPHES_CONFIG_RICH_

#include <DelphesConfig.h>

class DelphesConfigRICH: public DelphesConfig {
 public:
 DelphesConfigRICH(const char *dname): DelphesConfig(dname), m_Rindex(0.0) {};
 ~DelphesConfigRICH() {};

  void SetRefractiveIndex(double rindex) { m_Rindex = rindex; }

  int  Calculate();

 private:
  double m_Rindex;
};

#endif
