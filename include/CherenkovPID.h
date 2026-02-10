#pragma once

#include <algorithm>
#include <map>
#include <vector>

namespace IRT2 {

class CherenkovRadiator;

class RadiatorMassHypothesis {
 public:
 RadiatorMassHypothesis(): m_Npe(0.0), m_Weight(0.0) {};
  ~RadiatorMassHypothesis() {};

  inline void IncrementWeight(double npe, double weight) { 
    m_Npe += npe; m_Weight += weight;
  };

  inline void Reset( void ) { m_Npe = m_Weight = 0.0; };
  double GetNpe   ( void ) const { return m_Npe; };
  double GetWeight( void ) const { return m_Weight; };

 private:
  // Well, yes number of p.e.'s can be fractional depending on a PDF shape;
  double m_Npe;
  double m_Weight;
};

class MassHypothesis {
 public:
 MassHypothesis(double mass): m_Mass(mass) {};
  ~MassHypothesis() {};

  double GetMass( void )   const { return m_Mass; };
  inline void IncrementWeight(CherenkovRadiator *radiator, double npe, double weight) {
    m_RadiatorBreakdown[radiator].IncrementWeight(npe, weight);
  };

  void Reset( void ) {
    for(auto rhm: m_RadiatorBreakdown)
      rhm.second.Reset();
  };
  double GetNpe   (CherenkovRadiator *radiator) { 
    return m_RadiatorBreakdown[radiator].GetNpe(); 
  };
  double GetWeight(CherenkovRadiator *radiator) { 
    return m_RadiatorBreakdown[radiator].GetWeight(); 
  };

 private:
  double m_Mass;

  std::map<CherenkovRadiator*, RadiatorMassHypothesis> m_RadiatorBreakdown;
};

class CherenkovPID {
 public:
 CherenkovPID( void ) {};
  ~CherenkovPID() {
    std::for_each(m_Hypotheses.begin(), m_Hypotheses.end(), [](auto* h){ delete h; });
  };

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

} // namespace IRT2
