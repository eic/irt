
#include <vector>

#ifndef _CHERENKOV_PID_
#define _CHERENKOV_PID_

class MassHypothesis {
 public:
 MassHypothesis(double mass): m_Mass(mass), m_Nph(0.0), m_Weight(0.0) {};
  ~MassHypothesis() {};

  double GetMass( void )   const { return m_Mass; };
  inline void Reset( void ) { m_Nph = m_Weight = 0.0; };
  inline void IncrementWeight(double nph, double weight) { 
    m_Nph += nph; m_Weight += weight;
  };
  double GetNph( void )    const { return m_Nph; };
  double GetWeight( void ) const { return m_Weight; };

 private:
  double m_Mass;

  double m_Nph, m_Weight;
};

class CherenkovPID {
 public:
  CherenkovPID() {};
  ~CherenkovPID() {};

  void AddMassHypothesis(double mass) { m_Hypotheses.push_back(MassHypothesis(mass)); };
  unsigned GetHypothesesCount( void ) const { return m_Hypotheses.size(); };
  MassHypothesis *GetHypothesis(unsigned id) { 
    return (id < m_Hypotheses.size() ? &m_Hypotheses[id] : 0);
  };

 private:
  // FIXME: not dramatically efficient for a POD-like structure, but should 
  // suffice for now;
  std::vector<MassHypothesis> m_Hypotheses;
};

#endif
