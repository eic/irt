
#include <map>

#ifndef _DELPHES_CONFIG_RICH_
#define _DELPHES_CONFIG_RICH_

#include <DelphesConfig.h>

class DelphesConfigRICH: public DelphesConfig {
 public:
 DelphesConfigRICH( void ): DelphesConfig() {};
 DelphesConfigRICH(const char *dname): DelphesConfig(dname), m_Rindex(0.0)//, 
    /*m_AdditionalSmearing(0.0)*/ {};
 ~DelphesConfigRICH() {};

  void SetRefractiveIndex   (double rindex) { m_Rindex = rindex; }
  //void SetAdditionalSmearing(double value)  { m_AdditionalSmearing = value; }

  //bool ApplyThresholdModeLogic() { return true; };
  int  Calculate();

  double GenerateMeasurement(int pdg, const TVector3 &momentum);
  MassHypothesis *FindBestHypothesis(const TVector3 &momentum, double theta);

  void ImportTrackingSmearing(const char *ftheta, const char *fphi);

 private:
  double m_Rindex;//, m_AdditionalSmearing;

  std::map<std::pair<double, double>, std::pair<double, double>> m_ThetaSmearing, m_PhiSmearing;
  double GetTrackingSmearing(double momentum, double eta);
  double GetTrackingSmearing(const TVector3 &momentum) {
    return GetTrackingSmearing(momentum.Mag(), momentum.Eta());
  };

  ClassDef(DelphesConfigRICH, 3)
};

#endif
