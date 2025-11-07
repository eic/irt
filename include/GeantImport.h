#pragma once

#include <TTree.h>

#include "CherenkovEvent.h"

#define _MOMENTUM_CUTOFF_DEFAULT_ (0.010)

namespace IRT2 {

class CherenkovDetectorCollection;
class CherenkovDetector;

class GeantImport {
 public:
  GeantImport(const char *dfname = 0, const char *cfname = 0, const char *dname = 0);
  // FIXME: do it all better later;
  GeantImport(CherenkovDetectorCollection *geometry, CherenkovDetector *cdet, CherenkovEvent *event):
    m_Tree(0),
    m_IrtGeometry(geometry),
    m_RICH(cdet),
    m_Event(event),
    m_PurgeSecondaries(true),
    m_MomentumCutoff(_MOMENTUM_CUTOFF_DEFAULT_) {};
  ~GeantImport() {};

  CherenkovDetector *GetMyRICH( void )    const { return m_RICH; }
  CherenkovEvent *Event( void )           const { return m_Event; };
  void GetInputTreeEntry(unsigned ev)     const;// { if (m_Tree) m_Tree->GetEntry(ev); };
  unsigned GetInputTreeEntryCount( void ) const { return m_Tree ? m_Tree->GetEntries() : 0; };
 
  void AccountSecondaries( void )               { m_PurgeSecondaries = false; };
  void SetMomentumCutoff(double value)          { m_MomentumCutoff = value; };

  CherenkovDetectorCollection *GetIrtGeometry( void ) const { return m_IrtGeometry; };

protected:
  // Input TTree with GEANT simulated data;
  TTree *m_Tree;
  
 private:
  CherenkovDetectorCollection *m_IrtGeometry;
  
  // "pfRICH" or "dRICH" structure pointer;
  CherenkovDetector *m_RICH;

  // Input (GEANT) / output(Reco) event structure;
  CherenkovEvent *m_Event;

  bool m_PurgeSecondaries;
  double m_MomentumCutoff;
};

} // namespace IRT2
