#include <poincare/layout.h>
#include <poincare/old/cosine.h>
#include <poincare/old/derivative.h>
#include <poincare/old/division.h>
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

bool TangentNode::derivate(const ReductionContext& reductionContext,
                           Symbol symbol, OExpression symbolValue) {
  return Tangent(this).derivate(reductionContext, symbol, symbolValue);
}

OExpression TangentNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Tangent(this).unaryFunctionDifferential(reductionContext);
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
