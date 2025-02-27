#include <ion.h>
#include <omg/ieee754.h>
#include <poincare/random.h>

namespace Poincare {

template <typename T>
T random() {
  /* Source: Frédéric Goualard. Generating Random Floating-Point Numbers by
   * Dividing Integers: a Case Study. Proceedings of ICCS 2020, Jun 2020,
   * Amsterdam, Netherlands. ffhal-02427338
   *
   * Random number in [0,1) is obtained by dividing a random integer x in
   * [0,2^k) by 2^k.
   * - Assumption (1): k has to be a power of 2
   * - Assumption (2): k <= p with p the number of bits in mantissa + 1 (p = 24
   *   for float, p = 53 for double)
   * Otherwise the division x/2^k can be unrepresentable (because not dyadic (1)
   * or in-between two consecutive representables float (2)). If so, the
   * rounding to obtain floats lead to bias in the distribution.
   * The major con of this method is that we sample only among 1.5% (or 0.2%
   * for double) of representable floats in [0,1)...
   * TODO: find a way to be able to draw any float in [0,1)? */

  constexpr size_t p = OMG::IEEE754<T>::k_mantissaNbBits + 1;
  if (sizeof(T) == sizeof(float)) {
    uint32_t r = Ion::random() & ((static_cast<uint32_t>(1) << p) - 1);
    return static_cast<float>(r) / static_cast<float>((1 << p));
  } else {
    assert(sizeof(T) == sizeof(double));
    uint64_t r;
    uint32_t* rAddress = (uint32_t*)&r;
    *rAddress = Ion::random();
    *(rAddress + 1) = Ion::random();
    r = r & ((static_cast<uint64_t>(1) << p) - 1);
    return static_cast<double>(r) /
           static_cast<double>((static_cast<uint64_t>(1) << p));
  }
}

template float random();
template double random();

}  // namespace Poincare
