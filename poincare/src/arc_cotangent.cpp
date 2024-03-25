#include <poincare/addition.h>
#include <poincare/arc_tangent.h>
#include <poincare/complex.h>
#include <poincare/derivative.h>
#include <poincare/layout.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/trigonometry.h>

#include <cmath>

namespace Poincare {

int ArcCotangentNode::numberOfChildren() const {
  return ArcCotangent::s_functionHelper.numberOfChildren();
}

template <typename T>
std::complex<T> ArcCotangentNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit angleUnit) {
  if (c == static_cast<T>(0.0)) {
    return Trigonometry::ConvertRadianToAngleUnit(std::complex<T>(M_PI_2),
                                                  angleUnit);
  }
  return ArcTangentNode::computeOnComplex<T>(std::complex<T>(1) / c,
                                             complexFormat, angleUnit);
}

size_t ArcCotangentNode::serialize(char* buffer, size_t bufferSize,
                                   Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ArcCotangent::s_functionHelper.aliasesList().mainAlias());
}

OExpression ArcCotangentNode::shallowReduce(
    const ReductionContext& reductionContext) {
  ArcCotangent e = ArcCotangent(this);
  return Trigonometry::ShallowReduceInverseAdvancedFunction(e,
                                                            reductionContext);
}

bool ArcCotangentNode::derivate(const ReductionContext& reductionContext,
                                Symbol symbol, OExpression symbolValue) {
  return ArcCotangent(this).derivate(reductionContext, symbol, symbolValue);
}

OExpression ArcCotangentNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return ArcCotangent(this).unaryFunctionDifferential(reductionContext);
}

bool ArcCotangent::derivate(const ReductionContext& reductionContext,
                            Symbol symbol, OExpression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue,
                                    reductionContext);
  return true;
}

OExpression ArcCotangent::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Multiplication::Builder(
      Power::Builder(
          Multiplication::Builder(
              Trigonometry::UnitConversionFactor(
                  reductionContext.angleUnit(), Preferences::AngleUnit::Radian),
              Addition::Builder(Rational::Builder(1),
                                Power::Builder(childAtIndex(0).clone(),
                                               Rational::Builder(2)))),
          Rational::Builder(-1)),
      Rational::Builder(-1));
}

}  // namespace Poincare
