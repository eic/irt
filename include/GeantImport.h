
#include <TTree.h>

#ifndef _GEANT_IMPORT_
#define _GEANT_IMPORT_

#include "CherenkovEvent.h"
class CherenkovDetector;

class GeantImport {
 public:
  GeantImport(const char *dfname = 0, const char *cfname = 0, const char *dname = 0);
  ~GeantImport() {};

  CherenkovDetector *GetMyRICH( void )    const { return m_RICH; }
  CherenkovEvent *Event( void )           const { return m_Event; };
  void GetInputTreeEntry(unsigned ev)     const;// { if (m_Tree) m_Tree->GetEntry(ev); };
  unsigned GetInputTreeEntryCount( void ) const { return m_Tree ? m_Tree->GetEntries() : 0; };
 
  void AccountSecondaries( void )               { m_PurgeSecondaries = false; };
  void SetMomentumCutoff(double value)          { m_MomentumCutoff = value; };

 private:
  // "pfRICH" or "dRICH" structure pointer;
  CherenkovDetector *m_RICH;

  // Input TTree with GEANT simulated data;
  TTree *m_Tree;

  // Input (GEANT) / output(Reco) event structure;
  CherenkovEvent *m_Event;

  bool m_PurgeSecondaries; 
  double m_MomentumCutoff;
};

#endif
