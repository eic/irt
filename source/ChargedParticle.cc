
//
//  This code is independent of the details of how the trajectory points were 
//  evaluated; it just expects m_Locations[] vector to be populated for every radiator;
//
//  It can also take a pre-mixed vector of all photons rather than using MC truth 
// information about which photn was produced in which radiator;
//

#include <TRandom.h>

#include <ChargedParticle.h>

// -------------------------------------------------------------------------------------

double ChargedParticle::GetRecoCherenkovPhotonTheta(unsigned id)
{
  if (id >= m_Hits.size()) 
    return -1.0;
  else
    return m_Hits[id]->m_Solutions[this].m_Best->GetTheta();
} // ChargedParticle::GetRecoCherenkovPhotonTheta()

// -------------------------------------------------------------------------------------

double ChargedParticle::GetRecoCherenkovPhotonPhi(unsigned id)
{
  if (id >= m_Hits.size()) 
    return -1.0;
  else
    return m_Hits[id]->m_Solutions[this].m_Best->GetPhi();
} // ChargedParticle::GetRecoCherenkovPhotonPhi()

// -------------------------------------------------------------------------------------

double ChargedParticle::GetRecoCherenkovAverageTheta(CherenkovRadiator *radiator)
{
  unsigned stat = 0;
  double sum = 0.0;

  for(auto hit: m_Hits) {
    auto solution = hit->m_Solutions[this].m_Best;
    if (radiator && hit->m_Solutions[this].GetRadiator() != radiator) continue;
    
    stat++;
    sum += solution->GetTheta();
  } //for hit
  
  if (stat) sum /= stat;

  return sum;
} // ChargedParticle::GetRecoCherenkovAverageTheta()

// -------------------------------------------------------------------------------------

double ChargedParticle::GetMocaCherenkovAverageTheta(CherenkovRadiator *radiator)
{
  unsigned stat = 0;
  double sum = 0.0;

  for(auto rhptr: GetRadiatorHistory()) {
    auto rptr = GetRadiator(rhptr);

    if (rptr != radiator) continue; 

    for(auto photon: GetHistory(rhptr)->Photons()) {    
      auto n1 = photon->GetVertexParentMomentum().Unit(), n2 = photon->GetVertexMomentum().Unit();

      stat++;
      sum += acos(n1.Dot(n2));
    } //for photon
  } //for rhptr
  
  if (stat) sum /= stat;

  return sum;
} // ChargedParticle::GetMocaCherenkovAverageTheta()

// -------------------------------------------------------------------------------------

unsigned ChargedParticle::GetRecoCherenkovPhotonCount(CherenkovRadiator *radiator)
{
  unsigned stat = 0;

  for(auto hit: m_Hits) {
    if (radiator && hit->m_Solutions[this].GetRadiator() != radiator) continue;
    
    stat++;
  } //for hit

  return stat;
} // ChargedParticle::GetRecoCherenkovPhotonCount()

// -------------------------------------------------------------------------------------
