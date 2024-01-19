#ifndef POINCARE_EXPRESSION_APPROXIMATION_H
#define POINCARE_EXPRESSION_APPROXIMATION_H

#include <float.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/tree.h>

#include <cmath>
#include <complex>

#include "arithmetic.h"
#include "context.h"
#include "random.h"

namespace PoincareJ {

template <typename T>
bool IsIntegerRepresentationAccurate(T x) {
  /* Float and double's precision to represent integers is limited by the size
   * of their mantissa. If an integer requires more digits than there is in the
   * mantissa, there will be a loss on precision that can be fatal on operations
   * such as GCD and LCM. */
  int digits = 0;
  // Compute number of digits (base 2) required to represent x
  std::frexp(x, &digits);
  // Compare it to the maximal number of digits that can be represented with <T>
  return digits <= (sizeof(T) == sizeof(double) ? DBL_MANT_DIG : FLT_MANT_DIG);
}

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
  static std::complex<T> ComplexRootTreeTo(
      const Tree* node, AngleUnit angleUnit = AngleUnit::Radian);

  // Approximate an entire tree, isolated from any outer context.
  template <typename T>
  static T RootTreeTo(const Tree* node,
                      AngleUnit angleUnit = AngleUnit::Radian) {
    std::complex<T> value = ComplexRootTreeTo<T>(node);
    return value.imag() == 0 ? value.real() : NAN;
  }

  // Approximate any tree. With a nullptr context, seeded random will be undef.
  template <typename T>
  static std::complex<T> ComplexTo(const Tree* node, Random::Context* context);

  template <typename T>
  static T To(const Tree* node, Random::Context* context) {
    std::complex<T> value = ComplexTo<T>(node, context);
    return value.imag() == 0 ? value.real() : NAN;
  }

  template <typename T>
  static Tree* ToList(const Tree* node, AngleUnit angleUnit);

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
  static std::complex<T> FloatPowerReal(std::complex<T> a, std::complex<T> b) {
    /* PowerReal could not be reduced, b's reductions cannot be safely
     * interpreted as a rational. As a consequence, return NAN a is negative and
     *  b isn't an integer. */
    if (a.imag() != 0 || b.imag() != 0) {
      return NAN;
    }
    return (a.real() < static_cast<T>(0.0) && b.real() != std::round(b.real()))
               ? NAN
               : std::pow(a.real(), b.real());
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
    return a == static_cast<T>(0) ? NAN : std::log(a);
  }
  template <typename T>
  static T FloatLog(T a, T b) {
    return a == static_cast<T>(0) ? NAN : std::log(a) / std::log(b);
  }
  template <typename T>
  static T PositiveIntegerApproximation(std::complex<T> c) {
    T s = std::abs(c);
    /* Conversion from uint32 to float changes UINT32_MAX from 4294967295 to
     * 4294967296. */
    if (std::isnan(s) || s != std::round(s) ||
        s >= static_cast<T>(UINT32_MAX) ||
        !IsIntegerRepresentationAccurate(s)) {
      /* PositiveIntegerApproximationIfPossible returns undefined result if
       * scalar cannot be accurately represented as an unsigned integer. */
      return NAN;
    }
    return s;
  }

  template <typename T>
  static T FloatGCD(T a, T b) {
    T result = Arithmetic::GCD(a, b);
    if (!IsIntegerRepresentationAccurate(result)) {
      return NAN;
    }
    return result;
  }
  template <typename T>
  static T FloatLCM(T a, T b) {
    bool overflow = false;
    T result = Arithmetic::LCM(a, b, &overflow);
    if (overflow || !IsIntegerRepresentationAccurate(result)) {
      return NAN;
    }
    return result;
  }
  template <typename T>
  static T FloatTrig(T a, T b) {
    // Otherwise, handle any b, multiply by -1 if b%4 >= 2 then use b%2.
    assert(b == static_cast<T>(0.0) || b == static_cast<T>(1.0));
    return (b == static_cast<T>(0.0)) ? std::cos(a) : std::sin(a);
  }
  template <typename T>
  static T FloatATrig(T a, T b) {
    assert(b == static_cast<T>(0.0) || b == static_cast<T>(1.0));
    return (b == static_cast<T>(0.0)) ? std::acos(a) : std::asin(a);
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
  template <typename T, typename U>
  using Mapper = U (*)(T);
  template <typename T, typename U>
  static U MapAndReduce(const Tree* node, Reductor<U> reductor,
                        Random::Context* context,
                        Mapper<std::complex<T>, U> mapper = nullptr);
  template <typename T>
  static bool ApproximateAndReplaceEveryScalarT(Tree* tree, bool collapse);

  template <typename T>
  static T ConvertToRadian(T angle);

  template <typename T>
  static T ConvertFromRadian(T angle);
  static AngleUnit angleUnit;
};

}  // namespace PoincareJ

#endif
