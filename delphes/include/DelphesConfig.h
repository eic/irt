
#include <string>

#include <TObject.h>
#include <TVector3.h>

class TDatabasePDG;
#include <TParticlePDG.h>
#include <TRandom.h>

#ifndef _DELPHES_CONFIG_
#define _DELPHES_CONFIG_

class MassHypothesis: public TObject {
 public:
  MassHypothesis(TParticlePDG *pdg = 0, double max_contamination_left = 0.0, 
		 double max_contamination_right = 0.0): m_Diff(0.0), m_ChiSquare(0.0), m_PDG(pdg),
    m_MaxContaminationLeft(max_contamination_left), 
    m_MaxContaminationRight(max_contamination_right), m_Threshold(0.0) {};
  ~ MassHypothesis() {};

  double Mass( void ) const { return m_PDG->Mass(); };
  int PdgCode( void ) const { return m_PDG->PdgCode(); };

  void SetThreshold(double value) { m_Threshold = value; };
  double GetThreshold( void ) const { return m_Threshold; };

  double GetMeasurementOffset( void ) const { return m_Diff; };

  double m_Diff;       //!
  double m_ChiSquare;  //!

 private:
  TParticlePDG *m_PDG;

  double m_MaxContaminationLeft, m_MaxContaminationRight;

  double m_Threshold;

  ClassDef(MassHypothesis, 1)
};

class MomentumRange: public TObject {
 public:
  enum range {undefined, below_pion_threshold, below_kaon_threshold, below_proton_threshold};

 MomentumRange(double min = 0.0, double max = 0.0): m_Min(min), m_Max(max), m_MatrixDim(0), m_Matrix(0), 
    m_Range(MomentumRange::undefined) {
    if (min > max) std::swap(m_Min, m_Max);
  }; 

  // Just fill the buffer; FIXME: eventually will mess up with these two calls;
  void AddSigmaValue(double value) { m_SigmaValues.push_back(value); };
  void AddSigmaValue(double measurement, double sigma) { 
    m_MeasurementValues.push_back(measurement); 
    m_SigmaValues.push_back(sigma); 
  };

  double GetMin( void )             const { return m_Min; };
  double GetMax( void )             const { return m_Max; };
  unsigned GetSigmaCount( void )    const { return m_SigmaValues.size(); }
  double GetSigma(unsigned ih) const { 
    return ih < m_SigmaValues.size() ? m_SigmaValues[ih] : 0.0; 
  };
  double GetMeasurement(unsigned ih) const { 
    return ih < m_MeasurementValues.size() ? m_MeasurementValues[ih] : 0.0; 
  };

  unsigned m_MatrixDim;
  double *m_Matrix; //[m_MatrixDim]

 private:
  double m_Min, m_Max;

  std::vector<double> m_MeasurementValues;
  std::vector<double> m_SigmaValues;
  MomentumRange::range m_Range;

  ClassDef(MomentumRange, 2)
};

class EtaRange: public TObject {
 public:
 EtaRange(double min = 0.0, double max = 0.0): m_Min(min), m_Max(max) {
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

  MomentumRange *GetMomentumRange(double p) const {
    for(auto mrange: m_MomentumRanges)
      // Prefer to use '<=' here (help max value?);
      if (mrange->GetMin() <= p && p <= mrange->GetMax())
	return mrange;

    return 0;
  };

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

  ClassDef(EtaRange, 1)
};

class DelphesConfig: public TObject {
 public:
  DelphesConfig( void );
  DelphesConfig(const char *dname);
  virtual ~DelphesConfig() {};

  MassHypothesis *AddMassHypothesis(int pdg, double max_contamination_left = 1.0, 
				    double max_contamination_right = 1.0);
  MassHypothesis *AddMassHypothesis(const char *pname, double max_contamination_left = 1.0, 
				    double max_contamination_right = 1.0);

  EtaRange *AddEtaRange(double min, double max);
  
  bool StoreSigmaEntry(MomentumRange *mrange, int pdg, double sigma);
  bool StoreSigmaEntry(MomentumRange *mrange, int pdg, double measurement, double sigma);

  void UsePtMode( void ) { m_PtMode = true; };
  void AddZeroSigmaEntries( void );
  void Print();
  int  Check(bool rigorous = true);
  virtual int  Calculate()               = 0;
  //virtual bool ApplyThresholdModeLogic() = 0;
  void WriteTcl(bool check = true);

  unsigned GetMassHypothesisCount( void ) const { return m_MassHypotheses.size(); };
  void GetMassHypotheses(int harr[]) const {
    unsigned id = 0;

    for(auto hypo: m_MassHypotheses) 
      harr[id++] = hypo->PdgCode();
  };
  MassHypothesis *GetMassHypothesis(int pdg, bool ignore_sign = true);

  double GetEtaMin( void ) {
    if (!m_EtaRanges.size()) return -99;

    return m_EtaRanges[0]->GetMin();
  };
  double GetEtaMax( void ) {
    if (!m_EtaRanges.size()) return -99;

    return m_EtaRanges[m_EtaRanges.size()-1]->GetMax();
  };

  EtaRange *GetEtaRange(double eta) const {
    for(auto erange: m_EtaRanges)
      // Prefer to use '<=' here (help max value?);
      if (erange->GetMin() <= eta && eta <= erange->GetMax())
	return erange;

    return 0;
  };
  
  int GetSmearingMatrix(double eta, double p, double hmtx[]) const {
    for(unsigned iq=0; iq<m_MassHypotheses.size()*m_MassHypotheses.size(); iq++)
      hmtx[iq] = 0.0;

    auto erange = GetEtaRange(eta);
    if (!erange) return -1;

    auto mrange = erange->GetMomentumRange(p);
    if (!mrange) return -2;

    for(unsigned iq=0; iq<mrange->m_MatrixDim; iq++)
      hmtx[iq] = mrange->m_Matrix[iq];
    
    return 0;
  };
  MomentumRange *GetEtaMomentumRange(double eta, double p) const {
    auto erange = GetEtaRange(eta);
    if (!erange) return 0;

    return erange->GetMomentumRange(p);
  };

  int GetSmearingMatrix(const TVector3 p, double hmtx[]) const {
    return GetSmearingMatrix(p.Eta(), p.Mag(), hmtx);
  };

 protected:
  std::string m_Name;

  TDatabasePDG *m_DatabasePDG; 

  // Eta ranges in ascending order;
  std::vector<EtaRange*> m_EtaRanges;

  // Assume all masses are different and provided in ascending order;
  std::vector<MassHypothesis*> m_MassHypotheses;

  // FIXME: need a std::set, eventually?;
  unsigned GetHypoIndex(MassHypothesis *hypo) {
    for(unsigned ih=0; ih<m_MassHypotheses.size(); ih++)
      if (m_MassHypotheses[ih] == hypo)
	return ih;

    return 0;
  };

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

  // FIXME: need seed setting method;
  TRandom m_rndm; //!

  ClassDef(DelphesConfig, 1)
};

#endif
