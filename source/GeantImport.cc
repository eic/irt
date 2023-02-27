
#include <TFile.h>

#include <CherenkovDetectorCollection.h>
#include <CherenkovEvent.h>
#include <GeantImport.h>

// -------------------------------------------------------------------------------------

GeantImport::GeantImport(const char *dfname, const char *cfname, const char *dname):
  m_RICH(0),
  m_Tree(0),
  m_Event(0)
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
