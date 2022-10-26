
#include <TObject.h>
#include <TVector3.h>
#include <TRef.h>

#ifndef _REFLECTION_POINT_
#define _REFLECTION_POINT_

#include <CherenkovMirror.h>

class ReflectionPoint: public TObject {
 public:
 ReflectionPoint(): m_Mirror(0)/*, m_VolumeCopy(0)*/ {};
 ReflectionPoint(CherenkovMirror *mirror, unsigned vcopy, const TVector3 &position, const TVector3 &momentum):
  m_Mirror(mirror), /*m_VolumeCopy(vcopy),*/ m_Position(position), m_Momentum(momentum) {};
  ~ReflectionPoint() {};

 private:
  TRef m_Mirror;
  //unsigned m_VolumeCopy; // FIXME: unused
  TVector3 m_Position, m_Momentum;

  ClassDef(ReflectionPoint, 1);
};

#endif
