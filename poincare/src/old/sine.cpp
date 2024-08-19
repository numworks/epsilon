#include <poincare/layout.h>
#include <poincare/old/complex.h>
#include <poincare/old/cosine.h>
#include <poincare/old/derivative.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/sine.h>

#include <cmath>

namespace Poincare {

int SineNode::numberOfChildren() const {
  return Sine::s_functionHelper.numberOfChildren();
}

size_t SineNode::serialize(char* buffer, size_t bufferSize,
                           Preferences::PrintFloatMode floatDisplayMode,
                           int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Sine::s_functionHelper.aliasesList().mainAlias());
}

bool SineNode::derivate(const ReductionContext& reductionContext, Symbol symbol,
                        OExpression symbolValue) {
  return Sine(this).derivate(reductionContext, symbol, symbolValue);
}

OExpression SineNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Sine(this).unaryFunctionDifferential(reductionContext);
}

bool Sine::derivate(const ReductionContext& reductionContext, Symbol symbol,
                    OExpression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue,
                                    reductionContext);
  return true;
}

OExpression Sine::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Multiplication::Builder(
      Trigonometry::UnitConversionFactor(reductionContext.angleUnit(),
                                         Preferences::AngleUnit::Radian),
      Cosine::Builder(childAtIndex(0).clone()));
}

}  // namespace Poincare
