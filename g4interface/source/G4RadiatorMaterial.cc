
#include <G4RadiatorMaterial.h>

// -------------------------------------------------------------------------------------

#if 0
void G4RadiatorMaterial::SetRIChOptics(/*int mode*/)
{
  // Kind of ugly, but should work; cannot place this in the constructor since 
  // element definition happens afterwards; FIXME: inherit from g4dRIChGas as well?; 
  // the aerogel will have a different class then though;
  //m_RIChOptics = new g4dRIChGas(this);//mC2F6);

  m_RIChOptics->setOpticalParams();
  m_RIChOptics->pTable->GetProperty("RINDEX")->SetSpline(true);
} // G4RadiatorMaterial::SetRIChOptics()
void G4RadiatorMaterial::SetRIChOpticsCore(/*int mode*/)
{
  // Kind of ugly, but should work; cannot place this in the constructor since 
  // element definition happens afterwards; FIXME: inherit from g4dRIChGas as well?; 
  // the aerogel will have a different class then though;
  m_RIChOptics = new g4dRIChGas(this);//mC2F6);

  m_RIChOptics->setOpticalParams();
  m_RIChOptics->pTable->GetProperty("RINDEX")->SetSpline(true);
} // G4RadiatorMaterial::SetRIChOpticsCore()
#endif

// -------------------------------------------------------------------------------------
