
#include <TFile.h>

#include <CherenkovDetectorCollection.h>
#include <CherenkovEvent.h>
#include <GeantImport.h>

// -------------------------------------------------------------------------------------

GeantImport::GeantImport(const char *dfname, const char *cfname, const char *dname):
  m_RICH(0),
  m_Tree(0),
  m_Event(0),
  m_PurgeSecondaries(true),
  m_MomentumCutoff(0.010)
{
  // FIXME: exception handling;
  if (!dfname || !dname) return;

  auto fcfg = new TFile(cfname ? cfname : dfname);
  if (!fcfg) return;
  auto geometry = dynamic_cast<CherenkovDetectorCollection*>(fcfg->Get("CherenkovDetectorCollection"));
  auto fdata = new TFile(dfname);
  m_Tree = dynamic_cast<TTree*>(fdata->Get("t")); 
  m_Tree->SetBranchAddress("e", &m_Event);

  m_RICH = geometry->GetDetector(dname);

  m_Event = new CherenkovEvent();
} // GeantImport::GeantImport()

// -------------------------------------------------------------------------------------

void GeantImport::GetInputTreeEntry(unsigned ev) const 
{ 
  if (!m_Tree) return;

  m_Tree->GetEntry(ev); 

  if (m_PurgeSecondaries) {
    auto &particles = m_Event->ChargedParticles();

    for(auto it = particles.begin(); it != particles.end(); )
      if (!(*it)->IsPrimary() || (*it)->GetVertexMomentum().Mag() < m_MomentumCutoff)
	it = particles.erase(it);
      else
	it++;
  } //if
} // GeantImport::GetInputTreeEntry()

// -------------------------------------------------------------------------------------

