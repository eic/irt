
#include <vector>

#ifndef _CHERENKOV_PID_
#define _CHERENKOV_PID_

class MassHypothesis {
 public:
 MassHypothesis(double mass): m_Mass(mass), m_Npe(0.0), m_Weight(0.0) {};
  ~MassHypothesis() {};

  double GetMass( void )   const { return m_Mass; };
  inline void Reset( void ) { m_Npe = m_Weight = 0.0; };
  inline void IncrementWeight(double npe, double weight) { 
    m_Npe += npe; m_Weight += weight;
  };
  double GetNpe   ( void ) const { return m_Npe; };
  double GetWeight( void ) const { return m_Weight; };

 private:
  double m_Mass;

  // Well, yes number of p.e.'s can be fractional depending on a PDF shape;
  double m_Npe;
  double m_Weight;
};

class CherenkovPID {
 public:
 CherenkovPID( void ): m_Smearing(0.0), m_GaussianSmearing(false), m_TrajectoryBinCount(1) {};
  ~CherenkovPID() {};

  void AddMassHypothesis(double mass) { m_Hypotheses.push_back(new MassHypothesis(mass)); };
  unsigned GetHypothesesCount( void ) const { return m_Hypotheses.size(); };
  MassHypothesis *GetHypothesis(unsigned id) { 
    return (id < m_Hypotheses.size() ? m_Hypotheses[id] : 0);
  };
  inline double GetSmearing( void ) const { return m_Smearing; };
  void SetGaussianSmearing(double sigma) { m_GaussianSmearing = true;  m_Smearing = sigma; }
  void SetUniformSmearing (double range) { m_GaussianSmearing = false; m_Smearing = range; }
  bool UseGaussianSmearing( void )  const { return m_GaussianSmearing; };
  void SetTrajectoryBinCount(unsigned bins) { m_TrajectoryBinCount = bins; };
  double GetTrajectoryBinCount( void) const { return m_TrajectoryBinCount; };

 private:
  // This is a hack for now;
  double m_Smearing;
  bool m_GaussianSmearing;
  // FIXME: should be a per-radiator setting of course;
  unsigned m_TrajectoryBinCount;

  // FIXME: not dramatically efficient for a POD-like structure, but should 
  // suffice for now;
  std::vector<MassHypothesis*> m_Hypotheses;
};

#endif
