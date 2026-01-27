
#include <map>
#include <vector>

#include <TObject.h>

#ifndef _RADIATOR_HISTORY_
#define _RADIATOR_HISTORY_

#include "OpticalPhoton.h"
#include "ChargedParticleStep.h"

class RadiatorHistory: public TObject {
 public:
  RadiatorHistory(): m_TrajectoryBinCount(1) {};
  ~RadiatorHistory() {
    for(auto photon: m_Photons)
      delete photon;
    m_Photons.clear(); 

    for(auto trace: m_Steps)
      delete trace;
    m_Steps.clear();
  };

  inline unsigned GetStepCount( void ) const { return m_Steps.size(); };
  inline void AddStep(ChargedParticleStep *step) { m_Steps.push_back(step); };
  inline void AddOpticalPhoton(OpticalPhoton *photon) { m_Photons.push_back(photon); };

  inline std::vector<OpticalPhoton*> &Photons( void )     { return m_Photons; };
  inline const ChargedParticleStep *GetStep(unsigned id) const { 
    return (id < m_Steps.size() ? m_Steps[id] : 0); 
  };
  inline unsigned StepCount( void ) { return m_Steps.size(); };

  // Trajectory location management (moved from CherenkovRadiator for thread safety)
  void SetTrajectoryBinCount(unsigned bins) { m_TrajectoryBinCount = bins; };
  unsigned GetTrajectoryBinCount( void ) const { return m_TrajectoryBinCount; };
  void ResetLocations( void ) { m_Locations.clear(); }
  void AddLocation(const TVector3 &x, const TVector3 &n) { 
    m_Locations.push_back(std::make_pair(x, n)); 
  };
  const std::vector<std::pair<TVector3, TVector3>>& GetLocations( void ) const { return m_Locations; }; 

  void AddStepBufferPoint(double time, const TVector3 &x) {
    // Will be in ascending order of time;
    m_StepBuffer[time] = x;
  };

  void CalculateSteps(bool dirty = true) {
    // FIXME: memory leak;
    m_Steps.clear();

    // Add origin to the top of the buffer; FIXME: can times be negative 
    // as provided by GEANT?;
    if (m_StepBuffer.size() == 1) {
      if (!dirty) return;

      m_StepBuffer[0.0] = TVector3(0,0,0);
    } //if

    // FIXME: efficiency sucks here;
    std::vector<TVector3> buffer;
    for(auto entry: m_StepBuffer) 
      buffer.push_back(entry.second);

    for(unsigned iq=1; iq<buffer.size(); iq++) {
      m_Steps.push_back(new ChargedParticleStep(buffer[iq-1], 
						(buffer[iq] - buffer[iq-1]).Unit()));
      // Well, would not hurt to add the last point in the same direction;
      if (iq == buffer.size()-1)
	m_Steps.push_back(new ChargedParticleStep(buffer[iq], 
						  (buffer[iq] - buffer[iq-1]).Unit()));
    } //for iq
  };

  // Charged particle trajectory and optical photons generated in this radiator;
 private:
  std::vector<OpticalPhoton*> m_Photons;
  std::vector<ChargedParticleStep*> m_Steps;

  std::map<double, TVector3> m_StepBuffer; //!

  // Per-event trajectory data (moved from CherenkovRadiator for thread safety)
  unsigned m_TrajectoryBinCount; //!
  std::vector<std::pair<TVector3, TVector3>> m_Locations; //!

#ifndef DISABLE_ROOT_IO
  ClassDef(RadiatorHistory, 2);
#endif
};

#endif
