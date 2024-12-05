#include <omg/float.h>
#include <omg/unreachable.h>

#include <bit>
#include <cmath>
#include <complex>

#include "approximation.h"
#include "float.h"

namespace Poincare::Internal {

template <typename T>
T ConvertToRadian(T angle, AngleUnit angleUnit) {
  switch (angleUnit) {
    case AngleUnit::Radian:
      return angle;
    case AngleUnit::Degree:
      return angle * static_cast<T>(M_PI / 180.0);
    case AngleUnit::Gradian:
      return angle * static_cast<T>(M_PI / 200.0);
    default:
      OMG::unreachable();
  }
}

template <typename T>
T Approximation::ConvertFromRadian(T angle, AngleUnit angleUnit) {
  switch (angleUnit) {
    case AngleUnit::Radian:
      return angle;
    case AngleUnit::Degree:
      return angle * static_cast<T>(180.0 / M_PI);
    case AngleUnit::Gradian:
      return angle * static_cast<T>(200.0 / M_PI);
    default:
      OMG::unreachable();
  }
}

template <typename T>
std::complex<T> Approximation::TrigonometricToComplex(TypeBlock type,
                                                      std::complex<T> value,
                                                      AngleUnit angleUnit) {
  if (IsNonReal(value)) {
    /* TODO_PCJ: better handle nonreal values, we are losing information
     * (result could be undef here) */
    return NonReal<T>();
  }
  switch (type) {
    case Type::Cos:
    case Type::Sin: {
      std::complex<T> angleInput = ConvertToRadian(value, angleUnit);
      std::complex<T> res =
          type.isCos() ? std::cos(angleInput) : std::sin(angleInput);
      return NeglectRealOrImaginaryPartIfNegligible(res, angleInput);
    }
    case Type::Tan:
    case Type::Cot:
    case Type::Sec:
    case Type::Csc: {
      /* We use std::sin/std::cos instead of std::tan for 4 reasons:
       * - we do not want tan(π/2) to be infinity
       * - we have the same approximation when computing sin/cos or tan
       * - we homogenize approximation across platforms
       * - we have a symmetrical computation to cotangent
       * Same comment for cotangent. */
      std::complex<T> denominator = TrigonometricToComplex(
          type.isTan() || type.isSec() ? Type::Cos : Type::Sin, value,
          angleUnit);
      std::complex<T> numerator =
          type.isTan() || type.isCot()
              ? TrigonometricToComplex(type.isTan() ? Type::Sin : Type::Cos,
                                       value, angleUnit)
              : 1;
      if (denominator == static_cast<T>(0.0)) {
        return std::complex<T>(NAN, NAN);
      }
      return numerator / denominator;
    }

    case Type::ACos:
    case Type::ASin: {
      std::complex<T> result;
      if (value.imag() == 0 && std::fabs(value.real()) <= static_cast<T>(1.0)) {
        /* asin/acos: [-1;1] -> R
         * In these cases we rather use reals because std::asin/acos is not as
         * precise on complexes.
         * For instance,
         * - asin(complex<double>(0.03,0.0) = complex(0.0300045,1.11022e-16)
         * - asin(0.03) = 0.0300045
         * - acos(complex<double>(0.03,0.0) = complex(1.54079,-1.11022e-16)
         * - acos(0.03) = 1.54079 */
        result =
            type.isASin() ? std::asin(value.real()) : std::acos(value.real());
      } else {
        result = type.isASin() ? std::asin(value) : std::acos(value);
        /* asin and acos have a branch cut on ]-inf, -1[U]1, +inf[
         * We followed the convention chosen by the lib c++ of llvm on
         * ]-inf+0i, -1+0i[ (warning: it takes the other side of the cut values
         * on * ]-inf-0i, -1-0i[) and choose the values on ]1+0i, +inf+0i[ to
         * comply with :
         *   asin(-x) = -asin(x) and tan(asin(x)) = x/sqrt(1-x^2)     for asin
         *   acos(-x) = π - acos(x) and tan(acos(x)) = sqrt(1-x^2)/x  for acos
         */
        if (value.imag() == 0 && !std::signbit(value.imag()) &&
            value.real() > 1) {
          result.imag(-result.imag());  // other side of the cut
        }
      }
      result = NeglectRealOrImaginaryPartIfNegligible(result, value);
      return ConvertFromRadian(result, angleUnit);
    }
    case Type::ATan: {
      std::complex<T> result;
      if (value.imag() == static_cast<T>(0.) &&
          std::fabs(value.real()) <= static_cast<T>(1.)) {
        /* atan: R -> R
         * In these cases we rather use std::atan(double) because atan on
         * complexes is not as precise as atan on double in std library.
         * For instance,
         * - atan(complex<double>(0.01,0.0) =
         *       complex(9.9996666866652E-3,5.5511151231258E-17)
         * - atan(0.03) = 9.9996666866652E-3 */
        result = std::atan(value.real());
      } else if (value.real() == static_cast<T>(0.) &&
                 std::abs(value.imag()) == static_cast<T>(1.)) {
        /* The case value = ±i is caught here because std::atan(i) return i*inf
         * when it should be undef. (same as log(0))*/
        result = std::complex<T>(NAN, NAN);
      } else {
        result = std::atan(value);
        /* atan has a branch cut on ]-inf*i, -i[U]i, +inf*i[: it is then
         * multivalued on this cut. We followed the convention chosen by the lib
         * c++ of llvm on ]-i+0, -i*inf+0[ (warning: atan takes the other side
         * of the cut values on ]-i+0, -i*inf+0[) and choose the values on
         * ]-inf*i, -i[ to comply with atan(-x) = -atan(x) and sin(atan(x)) =
         * x/sqrt(1+x^2). */
        if (value.real() == 0 && !std::signbit(value.real()) &&
            value.imag() < -1) {
          result.real(-result.real());  // other side of the cut
        }
      }
      result = NeglectRealOrImaginaryPartIfNegligible(result, value);
      return ConvertFromRadian(result, angleUnit);
    }
    case Type::ASec:
    case Type::ACsc:
      if (value == static_cast<T>(0)) {
        return NAN;
      }
      return TrigonometricToComplex(type.isASec() ? Type::ACos : Type::ASin,
                                    static_cast<T>(1) / value, angleUnit);
    case Type::ACot: {
      std::complex<T> piOverTwo = ConvertFromRadian(M_PI_2, angleUnit);
      std::complex<T> arctanValue =
          TrigonometricToComplex(Type::ATan, value, angleUnit);
      return piOverTwo - arctanValue;
    }
    default:
      OMG::unreachable();
  }
}

template <typename T>
std::complex<T> Approximation::HyperbolicToComplex(TypeBlock type,
                                                   std::complex<T> value) {
  switch (type) {
    case Type::CosH:
    case Type::SinH:
      /* If c is real and large (over 100.0), the float evaluation of std::cosh
       * and std::sinh will return image = NaN when it should be 0.0. */
      return MakeResultRealIfInputIsReal<T>(
          NeglectRealOrImaginaryPartIfNegligible(
              type.isSinH() ? std::sinh(value) : std::cosh(value), value),
          value);
    case Type::TanH:
      return NeglectRealOrImaginaryPartIfNegligible(std::tanh(value), value);

    case Type::ArSinH: {
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
      return NeglectRealOrImaginaryPartIfNegligible(result, value);
    }
    case Type::ArCosH: {
      std::complex<T> result = std::acosh(value);
      /* acosh has a branch cut on ]-inf, 1]: it is then multivalued on this
       * cut. We followed the convention chosen by the lib c++ of llvm on
       * ]-inf+0i, 1+0i] (warning: atanh takes the other side of the cut values
       * on ]-inf-0i, 1-0i[).*/
      return NeglectRealOrImaginaryPartIfNegligible(result, value);
    }
    case Type::ArTanH: {
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
      return NeglectRealOrImaginaryPartIfNegligible(result, value);
    }

    default:
      OMG::unreachable();
  }
}

template float Approximation::ConvertFromRadian(float, AngleUnit);
template double Approximation::ConvertFromRadian(double, AngleUnit);

template std::complex<float> Approximation::TrigonometricToComplex(
    TypeBlock, std::complex<float>, AngleUnit);
template std::complex<double> Approximation::TrigonometricToComplex(
    TypeBlock, std::complex<double>, AngleUnit);

template std::complex<float> Approximation::HyperbolicToComplex(
    TypeBlock, std::complex<float>);
template std::complex<double> Approximation::HyperbolicToComplex(
    TypeBlock, std::complex<double>);

}  // namespace Poincare::Internal
