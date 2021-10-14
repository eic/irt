
#include <TObject.h>
#include <TVector3.h>
#include <TRef.h>

#ifndef _REFRACTION_POINT_
#define _REFRACTION_POINT_

#include <CherenkovRadiator.h>

class RefractionPoint: public TObject {
 public:
  RefractionPoint()/*: m_Mirror(0), m_VolumeCopy(0)*/ { m_Radiators[0] = 0; m_Radiators[1] = 0; };
 RefractionPoint(CherenkovRadiator *rfrom, CherenkovRadiator *rto, /*unsigned vcopy*/
		 const TVector3 &position, const TVector3 &mfrom, const TVector3 &mto):
  //m_Mirror(mirror), m_VolumeCopy(vcopy), 
  m_Position(position) {//, m_Momenta(momentum) {
  m_Radiators[0] = rfrom; m_Radiators[1] = rto; 
  m_Momenta  [0] = mfrom; m_Momenta  [1] = mto;
};
  ~RefractionPoint() {};

  //private:
  TRef m_Radiators[2];
  //unsigned m_VolumeCopy;
  TVector3 m_Position, m_Momenta[2];

  ClassDef(RefractionPoint, 1);
};

#endif
