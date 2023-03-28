#include <assert.h>
#include <ion.h>
#include <poincare/complex.h>
#include <poincare/ieee754.h>
#include <poincare/layout_helper.h>
#include <poincare/random.h>
#include <poincare/serialization_helper.h>

#include <cmath>

namespace Poincare {

int RandomNode::numberOfChildren() const {
  return Random::s_functionHelper.numberOfChildren();
}

Layout RandomNode::createLayout(Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits,
                                Context *context) const {
  return LayoutHelper::Prefix(
      Random(this), floatDisplayMode, numberOfSignificantDigits,
      Random::s_functionHelper.aliasesList().mainAlias(), context);
}

int RandomNode::serialize(char *buffer, int bufferSize,
                          Preferences::PrintFloatMode floatDisplayMode,
                          int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Random::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
Evaluation<T> RandomNode::templateApproximate(
    const ApproximationContext &approximationContext) const {
  if (approximationContext.withinReduce()) {
    // Return NAN to prevent the reduction from assuming anything at this point
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(Random::random<T>());
}

template <typename T>
T Random::random() {
  /* source: Frédéric Goualard. Generating Random Floating-Point Numbers by
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

  constexpr size_t p = IEEE754<T>::k_mantissaNbBits + 1;
  if (sizeof(T) == sizeof(float)) {
    uint32_t r = Ion::random() & ((static_cast<uint32_t>(1) << p) - 1);
    return static_cast<float>(r) / static_cast<float>((1 << p));
  } else {
    assert(sizeof(T) == sizeof(double));
    uint64_t r;
    uint32_t *rAddress = (uint32_t *)&r;
    *rAddress = Ion::random();
    *(rAddress + 1) = Ion::random();
    r = r & ((static_cast<uint64_t>(1) << p) - 1);
    return static_cast<double>(r) /
           static_cast<double>((static_cast<uint64_t>(1) << p));
  }
}

template Evaluation<float> RandomNode::templateApproximate<float>(
    const ApproximationContext &approximationContext) const;
template Evaluation<double> RandomNode::templateApproximate<double>(
    const ApproximationContext &approximationContext) const;
template float Random::random();
template double Random::random();

}  // namespace Poincare
