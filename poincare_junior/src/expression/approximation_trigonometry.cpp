#include <math.h>
#include <poincare/approximation_helper.h>
#include <poincare/float.h>

#include <bit>
#include <complex>

#include "approximation.h"
#include "float.h"

using Poincare::ApproximationHelper::MakeResultRealIfInputIsReal;
using Poincare::ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable;

namespace PoincareJ {

template <typename T>
T Approximation::ConvertToRadian(T angle) {
  if (angleUnit == AngleUnit::Radian) {
    return angle;
  }
  return angle * (angleUnit == AngleUnit::Degree
                      ? static_cast<T>(M_PI / 180.0)
                      : static_cast<T>(M_PI / 200.0));
}

template <typename T>
T Approximation::ConvertFromRadian(T angle) {
  if (angleUnit == AngleUnit::Radian) {
    return angle;
  }
  return angle * (angleUnit == AngleUnit::Degree
                      ? static_cast<T>(180.0 / M_PI)
                      : static_cast<T>(200.0 / M_PI));
}

template <typename T>
std::complex<T> Approximation::TrigonometricToComplex(TypeBlock type,
                                                      std::complex<T> value) {
  switch (type) {
    case BlockType::Cosine:
    case BlockType::Sine: {
      std::complex<T> angleInput = ConvertToRadian(value);
      std::complex<T> res =
          type.isCosine() ? std::cos(angleInput) : std::sin(angleInput);
      return NeglectRealOrImaginaryPartIfNeglectable(res, angleInput);
    }
    case BlockType::Tangent: {
      std::complex<T> angleInput = ConvertToRadian(value);
      /* tan should be undefined at (2n+1)*pi/2 for any integer n.
       * std::tan is not reliable at these values because it is diverging and
       * any approximation errors on pi could easily yield a finite result. At
       * these values, cos yields 0, but is also greatly affected by
       * approximation error and could yield a non-null value : cos(pi/2+e) ~=
       * -e On the other hand, sin, which should yield either 1 or -1 around
       * these values is much more resilient : sin(pi/2+e) ~= 1 - (e^2)/2. We
       * therefore use sin to identify values at which tan should be undefined.
       */
      std::complex<T> sin = std::sin(angleInput);
      if (sin == std::complex<T>(1) || sin == std::complex<T>(-1)) {
        return NAN;
      }
      std::complex<T> res = std::tan(angleInput);
      return NeglectRealOrImaginaryPartIfNeglectable(res, angleInput);
    }
    case BlockType::Secant:
    case BlockType::Cosecant:
    case BlockType::Cotangent: {
      std::complex<T> denominator = TrigonometricToComplex(
          type.isSecant() ? BlockType::Cosine : BlockType::Sine, value);
      std::complex<T> numerator =
          type.isCotangent() ? TrigonometricToComplex(BlockType::Cosine, value)
                             : 1;
      if (denominator == static_cast<T>(0.0)) {
        return NAN;
      }
      return numerator / denominator;
    }

    case BlockType::ArcCosine:
    case BlockType::ArcSine: {
      std::complex<T> c = value;
      std::complex<T> result;
      if (c.imag() == 0 && std::fabs(c.real()) <= static_cast<T>(1.0)) {
        /* asin/acos: [-1;1] -> R
         * In these cases we rather use reals because asin/acos on
         * complexes is not as precise in std library.
         * For instance,
         * - asin(complex<double>(0.03,0.0) = complex(0.0300045,1.11022e-16)
         * - asin(0.03) = 0.0300045
         * - acos(complex<double>(0.03,0.0) = complex(1.54079,-1.11022e-16)
         * - acos(0.03) = 1.54079 */
        result = type.isArcSine() ? std::asin(c.real()) : std::acos(c.real());
      } else {
        result = type.isArcSine() ? std::asin(c) : std::acos(c);
        /* asin and acos have a branch cut on ]-inf, -1[U]1, +inf[
         * We followed the convention chosen by the lib c++ of llvm on
         * ]-inf+0i, -1+0i[ (warning: it takes the other side of the cut values
         * on * ]-inf-0i, -1-0i[) and choose the values on ]1+0i, +inf+0i[ to
         * comply with :
         *   asin(-x) = -asin(x) and tan(asin(x)) = x/sqrt(1-x^2)     for asin
         *   acos(-x) = π - acos(x) and tan(acos(x)) = sqrt(1-x^2)/x  for acos
         */
        if (c.imag() == 0 && !std::signbit(c.imag()) && c.real() > 1) {
          result.imag(-result.imag());  // other side of the cut
        }
      }
      result = NeglectRealOrImaginaryPartIfNeglectable(result, c);
      return ConvertFromRadian(result);
    }
    case BlockType::ArcTangent: {
      std::complex<T> c = value;
      std::complex<T> result;
      if (c.imag() == static_cast<T>(0.) &&
          std::fabs(c.real()) <= static_cast<T>(1.)) {
        /* atan: R -> R
         * In these cases we rather use std::atan(double) because atan on
         * complexes is not as precise as atan on double in std library.
         * For instance,
         * - atan(complex<double>(0.01,0.0) =
         *       complex(9.9996666866652E-3,5.5511151231258E-17)
         * - atan(0.03) = 9.9996666866652E-3 */
        result = std::atan(c.real());
      } else if (c.real() == static_cast<T>(0.) &&
                 std::abs(c.imag()) == static_cast<T>(1.)) {
        /* The case c = ±i is caught here because std::atan(i) return i*inf when
         * it should be undef. (same as log(0) in Logarithm::computeOnComplex)*/
        result = std::complex<T>(NAN, NAN);
      } else {
        result = std::atan(c);
        /* atan has a branch cut on ]-inf*i, -i[U]i, +inf*i[: it is then
         * multivalued on this cut. We followed the convention chosen by the lib
         * c++ of llvm on ]-i+0, -i*inf+0[ (warning: atan takes the other side
         * of the cut values on ]-i+0, -i*inf+0[) and choose the values on
         * ]-inf*i, -i[ to comply with atan(-x) = -atan(x) and sin(atan(x)) =
         * x/sqrt(1+x^2). */
        if (c.real() == 0 && !std::signbit(c.real()) && c.imag() < -1) {
          result.real(-result.real());  // other side of the cut
        }
      }
      result = NeglectRealOrImaginaryPartIfNeglectable(result, c);
      return ConvertFromRadian(result);
    }
    case BlockType::ArcSecant:
    case BlockType::ArcCosecant:
      if (value == static_cast<T>(0)) {
        return NAN;
      }
      return TrigonometricToComplex(
          type.isArcSecant() ? BlockType::ArcCosine : BlockType::ArcSine,
          static_cast<T>(1) / value);
    case BlockType::ArcCotangent:
      if (value == static_cast<T>(0)) {
        return ConvertFromRadian(M_PI_2);
      }
      return TrigonometricToComplex(BlockType::ArcTangent,
                                    static_cast<T>(1) / value);
    default:
      assert(false);
  }
}

template <typename T>
std::complex<T> Approximation::HyperbolicToComplex(TypeBlock type,
                                                   std::complex<T> value) {
  switch (type) {
    case BlockType::HyperbolicCosine:
    case BlockType::HyperbolicSine:
      /* If c is real and large (over 100.0), the float evaluation of std::cosh
       * will return image = NaN when it should be 0.0. */
      return MakeResultRealIfInputIsReal<T>(
          NeglectRealOrImaginaryPartIfNeglectable(
              type.isHyperbolicSine() ? std::sinh(value) : std::cosh(value),
              value),
          value);
    case BlockType::HyperbolicTangent:
      return NeglectRealOrImaginaryPartIfNeglectable(std::tanh(value), value);

    case BlockType::HyperbolicArcSine: {
      std::complex<T> result = std::asinh(value);
      /* asinh has a branch cut on ]-inf*i, -i[U]i, +inf*i[: it is then
       * multivalued on this cut. We followed the convention chosen by the lib
       * c++ of llvm on ]+i+0, +i*inf+0[ (warning: atanh takes the other side of
       * the cut values on ]+i-0, +i*inf+0[) and choose the values on ]-inf*i,
       * -i[ to comply with asinh(-x) = -asinh(x). */
      if (value.real() == 0 && !std::signbit(value.real()) &&
          value.imag() < 1) {
        result.real(-result.real());  // other side of the cut
      }
      return NeglectRealOrImaginaryPartIfNeglectable(result, value);
    }
    case BlockType::HyperbolicArcCosine: {
      std::complex<T> result = std::acosh(value);
      /* acosh has a branch cut on ]-inf, 1]: it is then multivalued on this
       * cut. We followed the convention chosen by the lib c++ of llvm on
       * ]-inf+0i, 1+0i] (warning: atanh takes the other side of the cut values
       * on ]-inf-0i, 1-0i[).*/
      return NeglectRealOrImaginaryPartIfNeglectable(result, value);
    }
    case BlockType::HyperbolicArcTangent: {
      std::complex<T> result = std::atanh(value);
      /* atanh has a branch cut on ]-inf, -1[U]1, +inf[: it is then multivalued
       * on this cut. We followed the convention chosen by the lib c++ of llvm
       * on ]-inf+0i, -1+0i[ (warning: atanh takes the other side of the cut
       * values on ]-inf-0i, -1-0i[) and choose the values on ]1+0i, +inf+0i[ to
       * comply with atanh(-x) = -atanh(x) and sin(artanh(x)) = x/sqrt(1-x^2) */
      if (value.imag() == 0 && !std::signbit(value.imag()) &&
          value.real() > 1) {
        result.imag(-result.imag());  // other side of the cut
      }
      return NeglectRealOrImaginaryPartIfNeglectable(result, value);
    }

    default:
      assert(false);
  }
}

template std::complex<float> Approximation::TrigonometricToComplex(
    TypeBlock type, std::complex<float> value);
template std::complex<double> Approximation::TrigonometricToComplex(
    TypeBlock type, std::complex<double> value);

template std::complex<float> Approximation::HyperbolicToComplex(
    TypeBlock type, std::complex<float> value);
template std::complex<double> Approximation::HyperbolicToComplex(
    TypeBlock type, std::complex<double> value);

}  // namespace PoincareJ
