#ifndef POINCARE_IEEE754_H
#define POINCARE_IEEE754_H

#include <assert.h>
#include <stdint.h>
#include <stddef.h>

namespace Poincare {

/* This class describes the IEEE 754 float representation.
 * Float numbers are 32(64)-bit values, stored as follow:
 * sign: 1 bit (1 bit)
 * exponent: 8 bits (11 bits)
 * mantissa: 23 bits (52 bits)
 */

template<typename T>
class IEEE754 {
public:
  static uint16_t exponentOffset() {
    return ((1 <<(k_exponentNbBits-1))-1);
  }
  static uint16_t maxExponent() {
    return ((1<<k_exponentNbBits)-1);
  }
  static int size() {
    assert(k_totalNumberOfBits == 8*sizeof(T));
    return k_totalNumberOfBits;
  }
  static T buildFloat(bool sign, uint16_t exponent, uint64_t mantissa) {
    static uint64_t oneOnMantissaBits = ((uint64_t)1 << k_mantissaNbBits)-1;
    uint_float u;
    u.ui = 0;
    u.ui |= ((uint64_t)sign << (size()-k_signNbBits));
    u.ui |= ((uint64_t)exponent << k_mantissaNbBits);
    u.ui |= ((uint64_t)mantissa >> (size()-k_mantissaNbBits-1) & oneOnMantissaBits);
    return u.f;
  }
  static int exponent(T f) {
    uint_float u;
    u.f = f;
    uint16_t oneOnExponentsBits = (1 << k_exponentNbBits)-1;
    int exp = (u.ui >> k_mantissaNbBits) & oneOnExponentsBits;
    exp -= exponentOffset();
    return exp;
  }
private:
  union uint_float {
    uint64_t ui;
    T f;
  };
  constexpr static size_t k_signNbBits = 1;
  constexpr static size_t k_exponentNbBits = sizeof(T) == sizeof(float) ? 8 : 11;
  constexpr static size_t k_mantissaNbBits = sizeof(T) == sizeof(float) ? 23 : 52;
  constexpr static size_t k_totalNumberOfBits = k_signNbBits+k_exponentNbBits+k_mantissaNbBits;
};

}

#endif
