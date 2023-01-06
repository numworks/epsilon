#ifndef POINCARE_EXPRESSION_FLOAT_H
#define POINCARE_EXPRESSION_FLOAT_H

#include <utils/bit.h>

namespace Poincare {

class Float {
public:
  union FloatMemory {
    float m_float;
    uint32_t m_int;
  };
  constexpr static uint8_t SubFloatAtIndex(float value, int index) {
    FloatMemory f = {.m_float = value};
    return Bit::getByteAtIndex(f.m_int, index);
  }
};

}

#endif
