
#include <CherenkovDetectorCollection.h>

CherenkovDetectorCollection *CherenkovDetectorCollection::m_Instance = 0;

// -------------------------------------------------------------------------------------

CherenkovDetectorCollection::CherenkovDetectorCollection()
{
#if 1//_BACK_
  if (m_Instance) {
    printf("Attempt to instantiate CherenkovDetectorCollection singleton twice!\n");
    exit(0);
  } //if
  
  m_Instance = this;
#endif
} // CherenkovDetectorCollection::CherenkovDetectorCollection()

// -------------------------------------------------------------------------------------
