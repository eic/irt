
#include <TObject.h>

#ifndef _BIT_MASK_
#define _BIT_MASK_

class BitMask: public TObject {
 public:
 BitMask(ULong64_t mask = 0): m_Mask(mask) {};
  ~BitMask() {};

  // FIXME: no range check?;
  bool CheckBit(unsigned id)     const { return (m_Mask >> id) & 0x1; };
  bool CheckBits(ULong64_t mask) const { return (m_Mask & mask); };

  void SetMask(ULong64_t mask) { m_Mask  = mask; }; 
  void AddBits(ULong64_t mask) { m_Mask |= mask; }; 

 private:
  ULong64_t m_Mask;

#ifdef WITH_IRT_ROOT_IO
  ClassDef(BitMask,1);
#endif
};

#endif
