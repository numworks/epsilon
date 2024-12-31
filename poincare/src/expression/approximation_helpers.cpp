#include <omg/float.h>

#include "approximation.h"

namespace Poincare::Internal::Approximation::Private {

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

template <typename T>
T PositiveIntegerApproximation(T c) {
  T s = std::abs(c);
  /* Conversion from uint32 to float changes UINT32_MAX from 4294967295 to
   * 4294967296. */
  if (std::isnan(s) || s != std::round(s) || s >= static_cast<T>(UINT32_MAX) ||
      !IsIntegerRepresentationAccurate(s)) {
    /* PositiveIntegerApproximation returns undefined result if scalar cannot be
     * accurately represented as an unsigned integer. */
    return NAN;
  }
  return s;
}

template <typename T>
bool isNegligible(T x, T precision, T norm1, T norm2) {
  T absX = std::fabs(x);
  return absX <= precision && absX / norm1 <= precision &&
         absX / norm2 <= precision;
}

template <typename T>
T minimalNonNullMagnitudeOfParts(std::complex<T> c) {
  T absRealInput = std::fabs(c.real());
  T absImagInput = std::fabs(c.imag());
  // If the magnitude of one part is null, ignore it
  if (absRealInput == static_cast<T>(0.0) ||
      (absImagInput > static_cast<T>(0.0) && absImagInput < absRealInput)) {
    return absImagInput;
  }
  return absRealInput;
}

template <typename T>
std::complex<T> NeglectRealOrImaginaryPartIfNegligible(std::complex<T> result,
                                                       std::complex<T> input1,
                                                       std::complex<T> input2,
                                                       bool enableNullResult) {
  /* Cheat: openbsd  functions (cos, sin, tan, cosh, acos, pow...) are
   * numerical implementation and thus are approximative.
   * The error epsilon is ~1E-7 on float and ~1E-15 on double. In order to avoid
   * weird results as acos(1) = 6E-17 or cos(π/2) = 4E-17, we round the result
   * to its 1E-6 or 1E-14 precision when its ratio with the argument (π/2 in the
   * example) is smaller than epsilon. This way, we have sin(π) ~ 0 and
   * sin(1E-15)=1E-15.
   * We can't do that for all evaluation as the user can operate on values as
   * small as 1E-308 (in double) and most results still be correct. */
  if (!enableNullResult && (result.imag() == 0.0 || result.real() == 0.0)) {
    return result;
  }
  T magnitude1 = minimalNonNullMagnitudeOfParts(input1);
  T magnitude2 = minimalNonNullMagnitudeOfParts(input2);
  T precision = 10.0 * OMG::Float::EpsilonLax<T>();
  if (isNegligible(result.imag(), precision, magnitude1, magnitude2)) {
    result.imag(0);
  }
  if (isNegligible(result.real(), precision, magnitude1, magnitude2)) {
    result.real(0);
  }
  return result;
}

template <typename T>
std::complex<T> MakeResultRealIfInputIsReal(std::complex<T> result,
                                            std::complex<T> input) {
  return input.imag() == static_cast<T>(0.0)
             ? std::complex<T>(result.real(), static_cast<T>(0.0))
             : result;
}

template bool IsIntegerRepresentationAccurate(float);
template bool IsIntegerRepresentationAccurate(double);

template float PositiveIntegerApproximation(float);
template double PositiveIntegerApproximation(double);

template std::complex<float> NeglectRealOrImaginaryPartIfNegligible(
    std::complex<float>, std::complex<float>, std::complex<float>, bool);
template std::complex<double> NeglectRealOrImaginaryPartIfNegligible(
    std::complex<double>, std::complex<double>, std::complex<double>, bool);

template std::complex<float> MakeResultRealIfInputIsReal(std::complex<float>,
                                                         std::complex<float>);
template std::complex<double> MakeResultRealIfInputIsReal(std::complex<double>,
                                                          std::complex<double>);

}  // namespace Poincare::Internal::Approximation::Private
