#include <cmath>
#include <complex>

#include "approximation.h"

namespace Poincare::Internal::Approximation::Private {

template <typename T>
struct BaseAndExponentApproximation {
  std::complex<T> base;
  T exponent;
};

template <typename T>
BaseAndExponentApproximation<T> ApproximateLnOfPower(
    const Tree* e, const Approximation::Context* ctx) {
  assert(e->isLogarithm());
  const Tree* lnChild = e->child(0);
  if (!lnChild->isPow()) {
    return BaseAndExponentApproximation<T>{PrivateToComplex<T>(lnChild, ctx),
                                           1.0};
  }
  // ln(a^b)
  // For a more precise approximation, we compute b*ln(a)
  std::complex<T> a = PrivateToComplex<T>(lnChild->child(0), ctx);
  std::complex<T> b = PrivateToComplex<T>(lnChild->child(1), ctx);
  if ((a.imag() == 0) && (b.imag() == 0) && !std::isnan(a.real()) &&
      !std::isnan(b.real()) && (a.real() >= 0)) {
    return BaseAndExponentApproximation<T>{.base = a, .exponent = b.real()};
  }
  return BaseAndExponentApproximation<T>{
      .base = PrivateToComplex<T>(lnChild, ctx), .exponent = 1.0};
}

template <typename T>
std::complex<T> ApproximateUserLogarithm(const Tree* e,
                                         const Approximation::Context* ctx) {
  assert(e->isLnUser());
  BaseAndExponentApproximation<T> approximationElements =
      ApproximateLnOfPower<T>(e, ctx);
  if (ctx && ctx->m_complexFormat == ComplexFormat::Real &&
      (approximationElements.base.real() < 0 ||
       approximationElements.base.imag() != 0)) {
    return NonReal<T>();
  }
  if (approximationElements.base == std::complex<T>(0.0)) {
    return NAN;
  }
  return approximationElements.exponent * std::log(approximationElements.base);
}

template <typename T>
std::complex<T> ApproximateSystemLogarithm(const Tree* e,
                                           const Approximation::Context* ctx) {
  assert(e->isLn() || e->isLog());
  BaseAndExponentApproximation<T> approximationElements =
      ApproximateLnOfPower<T>(e, ctx);
  return approximationElements.exponent *
         Private::ComplexLogarithm<T>(approximationElements.base, e->isLog());
}

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

template std::complex<float> ApproximateUserLogarithm(
    const Tree*, const Approximation::Context*);
template std::complex<double> ApproximateUserLogarithm(
    const Tree*, const Approximation::Context*);
template std::complex<float> ApproximateSystemLogarithm(
    const Tree*, const Approximation::Context*);
template std::complex<double> ApproximateSystemLogarithm(
    const Tree*, const Approximation::Context*);

template std::complex<float> ComplexLogarithm(std::complex<float>, bool);
template std::complex<double> ComplexLogarithm(std::complex<double>, bool);

}  // namespace Poincare::Internal::Approximation::Private
