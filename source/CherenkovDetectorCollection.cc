
#include <CherenkovDetectorCollection.h>

namespace IRT2 {

CherenkovDetectorCollection *CherenkovDetectorCollection::m_Instance = 0;

// -------------------------------------------------------------------------------------

CherenkovDetectorCollection::CherenkovDetectorCollection()
{
  if (m_Instance) {
    printf("Attempt to instantiate CherenkovDetectorCollection singleton twice!\n");
    exit(0);
  } //if
  
  m_Instance = this;
} // CherenkovDetectorCollection::CherenkovDetectorCollection()

} // namespace IRT2

// -------------------------------------------------------------------------------------
