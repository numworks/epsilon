#ifndef POINCARE_IEEE754_H
#define POINCARE_IEEE754_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <cmath>

namespace Poincare {

/* This class describes the IEEE 754 float representation.
 * Float numbers are 32(64)-bit values, stored as follow:
 * sign: 1 bit (1 bit)
 * exponent: 8 bits (11 bits)
 * mantissa: 23 bits (52 bits)
 */

template <typename T>
class IEEE754 final {
 public:
  constexpr static uint16_t exponentOffset() {
    return (1 << (k_exponentNbBits - 1)) - 1;
  }
  constexpr static uint16_t maxExponent() {
    return (1 << k_exponentNbBits) - 1;
  }
  static int size() {
    assert(k_totalNumberOfBits == 8 * sizeof(T));
    return k_totalNumberOfBits;
  }
  static T buildFloat(bool sign, uint16_t exponent, uint64_t mantissa) {
    constexpr uint64_t oneOnMantissaBits =
        (static_cast<uint64_t>(1) << k_mantissaNbBits) - 1;
    uint_float u;
    u.ui = 0;
    u.ui |= ((uint64_t)sign << (size() - k_signNbBits));
    u.ui |= ((uint64_t)exponent << k_mantissaNbBits);
    u.ui |= ((uint64_t)mantissa >> (size() - k_mantissaNbBits - 1) &
             oneOnMantissaBits);
    /* So far, we just cast the Integer in float. To round it to the closest
     * float, we increment the mantissa (and sometimes the exponent if the
     * mantissa was full of 1) if the next mantissa bit is 1. */
    if (((uint64_t)mantissa >> (size() - k_mantissaNbBits - 2)) & 1) {
      u.ui += 1;
    }
    return u.f;
  }
  static int exponent(T f) {
    uint_float u;
    u.f = f;
    constexpr uint16_t oneOnExponentsBits = maxExponent();
    int exp = (u.ui >> k_mantissaNbBits) & oneOnExponentsBits;
    exp -= exponentOffset();
    return exp;
  }
  static int exponentBase10(T f) {
    constexpr T k_log10base2 =
        3.321928094887362347870319429489390175864831393024580612054;
    if (f == static_cast<T>(0.0)) {
      return 0;
    }
    T exponentBase2 = exponent(f);
    /* Compute the exponent in base 10 from exponent in base 2:
     * f = m1*2^e1
     * f = m2*10^e2
     * --> f = m1*10^(e1/log(10,2))
     * --> f = m1*10^x*10^(e1/log(10,2)-x), with x in [-1,1]
     * Thus e2 = e1/log(10,2)-x,
     *   with x such as 1 <= m1*10^x < 9 and e1/log(10,2)-x is round.
     * Knowing that the equation 1 <= m1*10^x < 10 with 1<=m1<2 has its solution
     * in -0.31 < x < 1, we get:
     * e2 = [e1/log(10,2)]  or e2 = [e1/log(10,2)]-1 depending on m1. */
    int exponentBase10 = std::round(exponentBase2 / k_log10base2);
    if (std::pow(static_cast<T>(10.0), exponentBase10) > std::fabs(f)) {
      exponentBase10--;
    }
    return exponentBase10;
  }

 private:
  union uint_float {
    uint64_t ui;
    T f;
  };

  constexpr static size_t k_signNbBits = 1;
  constexpr static size_t k_exponentNbBits =
      sizeof(T) == sizeof(float) ? 8 : 11;
  constexpr static size_t k_mantissaNbBits =
      sizeof(T) == sizeof(float) ? 23 : 52;
  constexpr static size_t k_totalNumberOfBits =
      k_signNbBits + k_exponentNbBits + k_mantissaNbBits;
};

}  // namespace Poincare

#endif
