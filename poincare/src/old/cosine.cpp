#include <poincare/layout.h>
#include <poincare/old/complex.h>
#include <poincare/old/cosine.h>
#include <poincare/old/derivative.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/sine.h>

#include <cmath>

namespace Poincare {

int CosineNode::numberOfChildren() const {
  return Cosine::s_functionHelper.numberOfChildren();
}

size_t CosineNode::serialize(char* buffer, size_t bufferSize,
                             Preferences::PrintFloatMode floatDisplayMode,
                             int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Cosine::s_functionHelper.aliasesList().mainAlias());
}

bool CosineNode::derivate(const ReductionContext& reductionContext,
                          Symbol symbol, OExpression symbolValue) {
  return Cosine(this).derivate(reductionContext, symbol, symbolValue);
}

OExpression CosineNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Cosine(this).unaryFunctionDifferential(reductionContext);
}

bool Cosine::derivate(const ReductionContext& reductionContext, Symbol symbol,
                      OExpression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue,
                                    reductionContext);
  return true;
}

OExpression Cosine::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Multiplication::Builder(
      Rational::Builder(-1),
      Trigonometry::UnitConversionFactor(reductionContext.angleUnit(),
                                         Preferences::AngleUnit::Radian),
      Sine::Builder(childAtIndex(0).clone()));
}

}  // namespace Poincare
