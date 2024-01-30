#ifndef POINCARE_EXPRESSION_APPROXIMATION_H
#define POINCARE_EXPRESSION_APPROXIMATION_H

#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/tree.h>

#include <cmath>

#include "random.h"

namespace PoincareJ {

/* Approximation is implemented on all block types.
 * We could have asserted that we reduce before approximating (and thus
 * implemented the approximation only on internal types) but this increases the
 * number of operations (for instance, 2 / 3 VS 2 * 3 ^ -1) and decreases the
 * precision. We rather beautify before approximating.
 */

class Approximation final {
 public:
  // Approximate an entire tree, isolated from any outer context.
  template <typename T>
  static T RootTreeTo(const Tree* node);
  // Approximate any tree. With a nullptr context, seeded random will be undef.
  template <typename T>
  static T To(const Tree* node, Random::Context* context);

  template <typename T>
  static T FloatAddition(T a, T b) {
    return a + b;
  }
  template <typename T>
  static T FloatMultiplication(T a, T b) {
    return a * b;
  }
  template <typename T>
  static T FloatPower(T a, T b) {
    return std::pow(a, b);
  }
  template <typename T>
  static T FloatPowerReal(T a, T b) {
    /* PowerReal could not be reduced, b's reductions cannot be safely
     * interpreted as a rational. As a consequence, return NAN a is negative and
     *  b isn't an integer. */
    return (a < static_cast<T>(0.0) && b != std::round(b)) ? NAN
                                                           : FloatPower(a, b);
  }
  template <typename T>
  static T FloatSubtraction(T a, T b) {
    return a - b;
  }
  template <typename T>
  static T FloatDivision(T a, T b) {
    return a / b;
  }
  template <typename T>
  static T FloatLnReal(T a) {
    // TODO: Unreal.
    return a < 0 ? NAN : FloatLn(a);
  }
  template <typename T>
  static T FloatLn(T a) {
    return a == 0 ? NAN : std::log(a);
  }
  template <typename T>
  static T FloatLog(T a, T b) {
    return a == 0 ? NAN : std::log2(a) / std::log2(b);
  }
  template <typename T>
  static T FloatTrig(T a, T b) {
    // Otherwise, handle any b, multiply by -1 if b%4 >= 2 then use b%2.
    assert(b == 0.0 || b == 1.0);
    return (b == 0.0) ? std::cos(a) : std::sin(a);
  }
  template <typename T>
  static T FloatATrig(T a, T b) {
    assert(b == 0.0 || b == 1.0);
    return (b == 0.0) ? std::acos(a) : std::asin(a);
  }
  // If collapse is true, approximate parents if all children have approximated.
  static bool ApproximateAndReplaceEveryScalar(Tree* tree,
                                               bool collapse = true) {
    return ApproximateAndReplaceEveryScalarT<double>(tree, collapse);
  }
  EDITION_REF_WRAP_1D(ApproximateAndReplaceEveryScalar, bool, true)

 private:
  template <typename T>
  using Reductor = T (*)(T, T);
  template <typename T>
  static T MapAndReduce(const Tree* node, Reductor<T> reductor,
                        Random::Context* context);
  template <typename T>
  static bool ApproximateAndReplaceEveryScalarT(Tree* tree, bool collapse);
};

}  // namespace PoincareJ

#endif
