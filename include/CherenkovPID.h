
#include <vector>

#ifndef _CHERENKOV_PID_
#define _CHERENKOV_PID_

class MassHypothesis {
 public:
 MassHypothesis(double mass): m_Mass(mass), m_Npe(0), m_Weight(0.0) {};
  ~MassHypothesis() {};

  double GetMass( void )   const { return m_Mass; };
  inline void Reset( void ) { m_Npe = m_Weight = 0.0; };
  inline void IncrementWeight(unsigned npe, double weight) { 
    m_Npe += npe; m_Weight += weight;
  };
  unsigned GetNpe( void )  const { return m_Npe; };
  double GetWeight( void ) const { return m_Weight; };

 private:
  double m_Mass;

  unsigned m_Npe;
  double m_Weight;
};

class CherenkovPID {
 public:
  CherenkovPID() {};
  ~CherenkovPID() {};

  void AddMassHypothesis(double mass) { m_Hypotheses.push_back(new MassHypothesis(mass)); };
  unsigned GetHypothesesCount( void ) const { return m_Hypotheses.size(); };
  MassHypothesis *GetHypothesis(unsigned id) { 
    return (id < m_Hypotheses.size() ? m_Hypotheses[id] : 0);
  };

 private:
  // FIXME: not dramatically efficient for a POD-like structure, but should 
  // suffice for now;
  std::vector<MassHypothesis*> m_Hypotheses;
};

#endif
