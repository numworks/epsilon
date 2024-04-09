#include <poincare/layout.h>
#include <poincare/old/addition.h>
#include <poincare/old/arc_tangent.h>
#include <poincare/old/complex.h>
#include <poincare/old/derivative.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/square_root.h>

#include <cmath>

namespace Poincare {

int ArcTangentNode::numberOfChildren() const {
  return ArcTangent::s_functionHelper.numberOfChildren();
}

size_t ArcTangentNode::serialize(char* buffer, size_t bufferSize,
                                 Preferences::PrintFloatMode floatDisplayMode,
                                 int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ArcTangent::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
std::complex<T> ArcTangentNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat,
    Preferences::AngleUnit angleUnit) {
  std::complex<T> result;
  if (c.imag() == static_cast<T>(0.) &&
      std::fabs(c.real()) <= static_cast<T>(1.)) {
    /* atan: R -> R
     * In these cases we rather use std::atan(double) because atan on complexes
     * is not as precise as atan on double in std library. For instance,
     * - atan(complex<double>(0.01,0.0) =
     * complex(9.9996666866652E-3,5.5511151231258E-17)
     * - atan(0.03) = 9.9996666866652E-3 */
    result = std::atan(c.real());
  } else if (c.real() == static_cast<T>(0.) &&
             std::abs(c.imag()) == static_cast<T>(1.)) {
    /* The case c = ±i is caught here because std::atan(i) return i*inf when it
     * should be undef. (same as log(0) in Logarithm::computeOnComplex)*/
    result = std::complex<T>(NAN, NAN);
  } else {
    result = std::atan(c);
    /* atan has a branch cut on ]-inf*i, -i[U]i, +inf*i[: it is then multivalued
     * on this cut. We followed the convention chosen by the lib c++ of llvm on
     * ]-i+0, -i*inf+0[ (warning: atan takes the other side of the cut values on
     * ]-i+0, -i*inf+0[) and choose the values on ]-inf*i, -i[ to comply with
     * atan(-x) = -atan(x) and sin(atan(x)) = x/sqrt(1+x^2). */
    if (c.real() == 0 && c.imag() < -1) {
      result.real(-result.real());  // other side of the cut
    }
  }
  result =
      ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result, c);
  return Trigonometry::ConvertRadianToAngleUnit(result, angleUnit);
}

OExpression ArcTangentNode::shallowReduce(
    const ReductionContext& reductionContext) {
  ArcTangent e = ArcTangent(this);
  return Trigonometry::ShallowReduceInverseFunction(e, reductionContext);
}

bool ArcTangentNode::derivate(const ReductionContext& reductionContext,
                              Symbol symbol, OExpression symbolValue) {
  return ArcTangent(this).derivate(reductionContext, symbol, symbolValue);
}

OExpression ArcTangentNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return ArcTangent(this).unaryFunctionDifferential(reductionContext);
}

bool ArcTangent::derivate(const ReductionContext& reductionContext,
                          Symbol symbol, OExpression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue,
                                    reductionContext);
  return true;
}

OExpression ArcTangent::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Power::Builder(
      Multiplication::Builder(
          Trigonometry::UnitConversionFactor(reductionContext.angleUnit(),
                                             Preferences::AngleUnit::Radian),
          Addition::Builder(
              Rational::Builder(1),
              Power::Builder(childAtIndex(0).clone(), Rational::Builder(2)))),
      Rational::Builder(-1));
}

}  // namespace Poincare
