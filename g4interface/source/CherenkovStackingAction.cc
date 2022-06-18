
#include "CherenkovStackingAction.h"

#include <TTree.h>

#include <CherenkovEvent.h>

//CherenkovStackingAction *CherenkovStackingAction::m_Instance = 0;

// -------------------------------------------------------------------------------------

CherenkovStackingAction::CherenkovStackingAction(CherenkovEvent *event, TTree *tree): 
  G4UserStackingAction(), m_EventPtr(event), m_Tree(tree)
{
  //m_Instance = this;  
} //CherenkovStackingAction::CherenkovStackingAction()

// -------------------------------------------------------------------------------------

void CherenkovStackingAction::NewStage()
{
  static unsigned counter;
  printf("@@@ Calling NewStage() ... %5d\n", counter++);
  if (m_Tree) m_Tree->Fill();
} // CherenkovStackingAction::NewStage()

// -------------------------------------------------------------------------------------

void CherenkovStackingAction::PrepareNewEvent()
{
  if (m_EventPtr) m_EventPtr->Reset();
} // CherenkovStackingAction::PrepareNewEvent()

// -------------------------------------------------------------------------------------
