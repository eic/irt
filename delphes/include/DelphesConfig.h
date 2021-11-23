
#include <string>

class TDatabasePDG;
#include <TParticlePDG.h>

#ifndef _DELPHES_CONFIG_
#define _DELPHES_CONFIG_

class MassHypothesis {
 public:
  MassHypothesis(TParticlePDG *pdg, double max_contamination_left, 
		 double max_contamination_right): m_PDG(pdg),
    m_MaxContaminationLeft(max_contamination_left), 
    m_MaxContaminationRight(max_contamination_right), m_Threshold(0.0) {};
  ~ MassHypothesis() {};

  double Mass( void ) const { return m_PDG->Mass(); };
  int PdgCode( void ) const { return m_PDG->PdgCode(); };

  void SetThreshold(double value) { m_Threshold = value; };
  double GetThreshold( void ) const { return m_Threshold; };

 private:
  TParticlePDG *m_PDG;

  double m_MaxContaminationLeft, m_MaxContaminationRight;

  double m_Threshold;
};

class MomentumRange {
 public:
  enum range {undefined, below_pion_threshold, below_kaon_threshold, below_proton_threshold};

 MomentumRange(double min, double max): m_Min(min), m_Max(max), m_Matrix(0), 
    m_Range(MomentumRange::undefined) {
    if (min > max) std::swap(m_Min, m_Max);
  }; 

  // Just fill the buffer;
  void AddSigmaValue(double value) { m_SigmaValues.push_back(value); };

  double GetMin( void )             const { return m_Min; };
  double GetMax( void )             const { return m_Max; };
  unsigned GetSigmaCount( void )    const { return m_SigmaValues.size(); }
  double GetSigma(unsigned ih) const { 
    return ih < m_SigmaValues.size() ? m_SigmaValues[ih] : 0.0; 
  };

  double *m_Matrix;

 private:
  double m_Min, m_Max;

  std::vector<double> m_SigmaValues;
  MomentumRange::range m_Range;
};

class EtaRange {
 public:
 EtaRange(double min, double max): m_Min(min), m_Max(max) {
    if (min > max) std::swap(m_Min, m_Max);
  };

  // A hack-like method, to populate PDG entries one by one;
  MomentumRange *GetMomentumRange(double min, double max);

  // This will be the userland AddMomentumRange() wrapper;  
  template<typename... Args> MomentumRange *AddMomentumRange(double min, double max, Args... args) {
    MomentumRange *prev = m_MomentumRanges.size() ? m_MomentumRanges.back() : 0;
    if (prev && min != prev->GetMax()) {
      printf("Momentum ranges sequence issue: %6.2f is followed by %6.2f\n", 
	     prev->GetMax(), min);
      exit(1);
    } //for if
    auto mrange = new MomentumRange(min, max);
    m_MomentumRanges.push_back(mrange);

    ArgumentRecursion(mrange, args...);

    return mrange;
  };           

  double GetMin( void ) const { return m_Min; };
  double GetMax( void ) const { return m_Max; };

  unsigned GetMomentumRangeCount( void ) const { return m_MomentumRanges.size(); }
  const MomentumRange *FirstMomentumRange( void ) const { 
    return m_MomentumRanges.empty() ? 0 : m_MomentumRanges.front();
  };
  const MomentumRange *LastMomentumRange( void ) const { 
    return m_MomentumRanges.empty() ? 0 : m_MomentumRanges.back();
  };

  std::vector<MomentumRange*> m_MomentumRanges;

 private:
  template <typename T, typename... Args> 
    void ArgumentRecursion(MomentumRange *mrange, const T& firstArg, const Args&... args) { 
    // NB: c++11 static_assert() here guarantees that analysis.C does not *compile* 
    // rather than fail at run-time;
    static_assert(std::is_same<T, double>::value, "'const char *' parameter(s) expected!");
      
    mrange->AddSigmaValue(firstArg);

    // Recursively process 'fnum' and the rest of detector name arguments; 
    ArgumentRecursion(mrange, args...); 
  };
  // Once no more arguments left, call this dummy;
  void ArgumentRecursion(MomentumRange *mrange) {};

  double m_Min, m_Max;
};

class DelphesConfig {
 public:
  DelphesConfig(const char *dname);
  ~DelphesConfig() {};

  MassHypothesis *AddMassHypothesis(int pdg, double max_contamination_left = 1.0, 
				    double max_contamination_right = 1.0);
  MassHypothesis *AddMassHypothesis(const char *pname, double max_contamination_left = 1.0, 
				    double max_contamination_right = 1.0);

  EtaRange *AddEtaRange(double min, double max);
  
  bool StoreSigmaEntry(MomentumRange *mrange, int pdg, double sigma);

  void UsePtMode( void ) { m_PtMode = true; };
  void AddZeroSigmaEntries( void );
  void Print();
  int  Check();
  virtual int  Calculate() = 0;
  void Write();

 protected:
  std::string m_Name;

  TDatabasePDG *m_DatabasePDG; 

  // Eta ranges in ascending order;
  std::vector<EtaRange*> m_EtaRanges;

  // Assume all masses are different and provided in ascending order;
  std::vector<MassHypothesis*> m_MassHypotheses;

 private:
  MassHypothesis *AddMassHypothesisCore(TParticlePDG *pdg, double max_contamination_left, 
				    double max_contamination_right);

  void DetermineThresholds( void );
  void WriteMassHypothesis(FILE *fout, unsigned ih);

  double m_EtaMin, m_EtaMax;
  double m_MomentumMin, m_MomentumMax;
  // These only make sense for pi/K/p case;
  //double m_PionThreshold, m_KaonThreshold, m_ProtonThreshold;

 protected:
  // Yes, this is a global parameter, so that all entries in the output table have 
  // a consistent meaning;
  bool m_EfficiencyContaminationMode;

  bool m_PtMode;
};

#endif
