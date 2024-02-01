#include <poincare/approximation_helper.h>

#include "approximation.h"
#include "rational.h"

using Poincare::ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable;

namespace PoincareJ {

template <typename T>
std::complex<T> computeNotPrincipalRealRootOfRationalPow(
    const std::complex<T> c, T p, T q) {
  // Assert p and q are in fact integers
  assert(std::round(p) == p);
  assert(std::round(q) == q);
  /* Try to find a real root of c^(p/q) with p, q integers. We ignore cases
   * where the principal root is real as these cases are handled generically
   * later (for instance 1232^(1/8) which has a real principal root is not
   * handled here). */
  if (c.imag() == static_cast<T>(0.0) &&
      std::pow(static_cast<T>(-1.0), q) < static_cast<T>(0.0)) {
    /* If c real and q odd integer (q odd if (-1)^q = -1), a real root does
     * exist (which is not necessarily the principal root)!
     * For q even integer, a real root does not necessarily exist (example:
     * -2 ^(1/2)). */
    std::complex<T> absc = c;
    absc.real(std::fabs(absc.real()));
    // compute |c|^(p/q) which is a real
    std::complex<T> absCPowD =
        computeOnComplex<T>(absc, std::complex<T>(p / q), ComplexFormat::Real);
    /* As q is odd, c^(p/q) = (sign(c)^(1/q))^p * |c|^(p/q)
     *                      = sign(c)^p         * |c|^(p/q)
     *                      = -|c|^(p/q) iff c < 0 and p odd */
    return c.real() < static_cast<T>(0.0) &&
                   std::pow(static_cast<T>(-1.0), p) < static_cast<T>(0.0)
               ? -absCPowD
               : absCPowD;
  }
  return NAN;
}

template <typename T>
std::complex<T> computeOnComplex(const std::complex<T> c,
                                 const std::complex<T> d,
                                 ComplexFormat complexFormat) {
  if (c.imag() == static_cast<T>(0.0) && c.real() < static_cast<T>(0.0) &&
      ((d.real() == INFINITY && c.real() <= static_cast<T>(-1.0)) ||
       (d.real() == -INFINITY && c.real() >= static_cast<T>(-1.0)))) {
    /* x^inf with x <= -1 and x^(-inf) with -1 <= x <= 0 are approximated to
     * complex infinity, which we don't handle. We decide to return undef. */
    return NAN;
  }
  std::complex<T> result;
  if (c.imag() == static_cast<T>(0.0) && d.imag() == static_cast<T>(0.0) &&
      c.real() != static_cast<T>(0.0) &&
      (c.real() > static_cast<T>(0.0) || std::round(d.real()) == d.real())) {
#if !PLATFORM_DEVICE
    if (std::fabs(c.real()) == static_cast<T>(1.0) &&
        std::fabs(d.real()) == INFINITY) {
      /* On simulator, std::pow(1,Inf) is approximated to 1, which is not the
       * behavior we want. */
      return NAN;
    }
#endif
    /* pow: (R+, R) -> R+ (2^1.3 ~ 2.46)
     * pow: (R-, N) -> R+ ((-2)^3 = -8)
     * In these cases we rather use std::pow(double, double) because:
     * - pow on complexes is not as precise as pow on double: for instance,
     *   pow(complex<double>(2.0,0.0), complex<double>(3.0,0.0) =
     * complex(7.9999999999999982,0.0) and pow(2.0,3.0) = 8.0
     * - Using complex pow, std::pow(2.0, 1000) = (INFINITY, NAN).
     *   Openbsd pow of a positive real and another real has a undefined
     *   imaginary when the real result is infinity.
     * However, we exclude c == 0 because std:pow(0.0, 0.0) = 1.0 and we would
     * rather have 0^0 = undef. */
    result = std::complex<T>(std::pow(c.real(), d.real()));
  } else {
    result = std::pow(c, d);
  }
  /* Openbsd trigonometric functions are numerical implementation and thus are
   * approximative.
   * The error epsilon is ~1E-7 on float and ~1E-15 on double. In order to
   * avoid weird results as e(i*pi) = -1+6E-17*i, we compute the argument of
   * the result of c^d and if arg ~ 0 [Pi], we discard the residual imaginary
   * part and if arg ~ Pi/2 [Pi], we discard the residual real part.
   * Let's determine when the arg [Pi] (or arg [Pi/2]) is negligeable:
   * With c = r*e^(iθ) and d = x+iy, c^d = r^x*e^(yθ)*e^i(yln(r)+xθ)
   * so arg(c^d) = y*ln(r)+xθ.
   * We consider that arg[π] is negligeable if it is negligeable compared to
   * norm(d) = sqrt(x^2+y^2) and ln(r) = ln(norm(c)).*/
  if (complexFormat != ComplexFormat::Real && c.real() < static_cast<T>(0.0) &&
      std::round(d.real()) != d.real()) {
    /* Principal root of a negative base and non-integer index is always complex
     * Neglecting it could cause visual artefacts when plotting x^x with a
     * cartesian complex format. The issue is still visible when x is so small
     * that result is 0, which is plotted even though it is "complex". */
    return result;
  }
  std::complex<T> precision =
      d.real() < static_cast<T>(0.0) ? std::pow(c, static_cast<T>(-1.0)) : c;
  return NeglectRealOrImaginaryPartIfNeglectable(result, precision, d, false);
}

template <typename T>
std::complex<T> Approximation::ApproximatePower(const Tree *power,
                                                ComplexFormat complexFormat) {
  const Tree *base = power->child(0);
  const Tree *exponent = power->child(1);
  std::complex<T> c = ToComplex<T>(base);
  /* Special case: c^(p/q) with p, q integers
   * In real mode, c^(p/q) might have a real root which is not the principal
   * root. We return this value in that case to avoid returning "nonreal". */
  if (complexFormat == ComplexFormat::Real) {
    T p = NAN;
    T q = NAN;
    // If the power has been reduced, we look for a rational index
    if (exponent->isRational()) {
      p = Rational::Numerator(exponent).to<T>();
      q = Rational::Denominator(exponent).to<T>();
    }
    /* If the power has been simplified (reduced + beautified), we look for an
     * index of the for Division(Rational,Rational). */
    if (exponent->isDivision() && exponent->child(0)->isInteger() &&
        exponent->child(1)->isInteger()) {
      p = To<T>(exponent->child(0));
      q = To<T>(exponent->child(1));
    }
    /* We don't handle power that haven't been reduced or simplified as the
     * index can take to many forms and still be equivalent to p/q,
     * with p, q integers. */
    if (std::isnan(p) || std::isnan(q)) {
      goto defaultApproximation;
    }
    return computeNotPrincipalRealRootOfRationalPow(c, p, q);
  }
defaultApproximation:
  return computeOnComplex<T>(c, ToComplex<T>(exponent), complexFormat);
}

template std::complex<float> Approximation::ApproximatePower(
    const Tree *child, ComplexFormat complexFormat);
template std::complex<double> Approximation::ApproximatePower(
    const Tree *child, ComplexFormat complexFormat);

}  // namespace PoincareJ
