#include <poincare/old/cosine.h>
#include <poincare/old/derivative.h>
#include <poincare/old/division.h>
#include <poincare/old/layout.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/sine.h>
#include <poincare/old/tangent.h>
#include <poincare/old/trigonometry.h>

#include <cmath>

namespace Poincare {

int TangentNode::numberOfChildren() const {
  return Tangent::s_functionHelper.numberOfChildren();
}

size_t TangentNode::serialize(char* buffer, size_t bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Tangent::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
std::complex<T> TangentNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit angleUnit) {
  /* We use std::sin/std::cos instead of std::tan for 4 reasons:
   * - we do not want tan(π/2) to be infinity
   * - we have the same approximation when computing sin/cos or tan
   * - we homogenize approximation across platforms
   * - we have a symmetrical computation to cotangent */
  std::complex<T> denominator =
      CosineNode::computeOnComplex<T>(c, complexFormat, angleUnit);
  std::complex<T> numerator =
      SineNode::computeOnComplex<T>(c, complexFormat, angleUnit);
  if (denominator == static_cast<T>(0.0)) {
    return complexNAN<T>();
  }
  return numerator / denominator;
}

OExpression TangentNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Tangent(this).shallowReduce(reductionContext);
}

bool TangentNode::derivate(const ReductionContext& reductionContext,
                           Symbol symbol, OExpression symbolValue) {
  return Tangent(this).derivate(reductionContext, symbol, symbolValue);
}

OExpression TangentNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Tangent(this).unaryFunctionDifferential(reductionContext);
}

OExpression Tangent::shallowReduce(ReductionContext reductionContext) {
  OExpression newExpression =
      Trigonometry::ShallowReduceDirectFunction(*this, reductionContext);
  if (newExpression.otype() == ExpressionNode::Type::Tangent) {
    Sine s = Sine::Builder(newExpression.childAtIndex(0).clone());
    Cosine c = Cosine::Builder(newExpression.childAtIndex(0));
    Division d = Division::Builder(s, c);
    s.shallowReduce(reductionContext);
    c.shallowReduce(reductionContext);
    newExpression.replaceWithInPlace(d);
    return d.shallowReduce(reductionContext);
  }
  return newExpression;
}

bool Tangent::derivate(const ReductionContext& reductionContext, Symbol symbol,
                       OExpression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue,
                                    reductionContext);
  return true;
}

OExpression Tangent::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Multiplication::Builder(
      Trigonometry::UnitConversionFactor(reductionContext.angleUnit(),
                                         Preferences::AngleUnit::Radian),
      Power::Builder(Cosine::Builder(childAtIndex(0).clone()),
                     Rational::Builder(-2)));
}

}  // namespace Poincare
