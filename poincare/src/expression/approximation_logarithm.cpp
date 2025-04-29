#include <cmath>

#include "approximation.h"

namespace Poincare::Internal::Approximation::Private {

template <typename T>
std::complex<T> ComplexLogarithm(std::complex<T> c, bool isLog10) {
  /* The log function has a branch cut on ]-inf, 0]: it is then multi-valued on
   * this cut. We follow the same convention as std::log(std::complex) on
   * (]-inf, 0], +0i) (warning: std::log takes the other side of the cut values
   on (]-inf, 0], -0i).
   *
   We manually handle cases for which std::log(std::complex) can raise a
   floating point exception:
   * - (-0, +0) and (+0, +0) will raise FE_DIVBYZERO. The returned value of
   * std::log(std::complex) is (-∞,π) for (-0,+0) and (-∞,+0) for (+0,+0),
   however we return (-∞,+0) for both cases.
   * - (x,NaN) or (NaN,y) for finite values of x or y may raise FE_INVALID.
   * std::log(std::complex) returns (NaN, NaN) is both cases and we do the same.
   * - all other values are garanteed not to raise floating point exceptions.
   *
   * std::log10(c) behaves like std::log(c) / std::log(T(10)), so the same
   * special cases apply. */

  if ((c.real() == 0.0) && (c.imag() == 0.0)) {
    return std::complex<T>(-INFINITY, 0.0);
  }
  if ((std::isnan(c.real()) && std::isfinite(c.imag())) ||
      (std::isnan(c.imag()) && std::isfinite(c.real()))) {
    return std::complex<T>(NAN, NAN);
  }
  return isLog10 ? std::log10(c) : std::log(c);
}

template std::complex<float> ComplexLogarithm(std::complex<float>, bool);
template std::complex<double> ComplexLogarithm(std::complex<double>, bool);

}  // namespace Poincare::Internal::Approximation::Private
