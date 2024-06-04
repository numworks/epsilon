#include <poincare/layout.h>
#include <poincare/old/arc_sine.h>
#include <poincare/old/complex.h>
#include <poincare/old/conjugate.h>
#include <poincare/old/derivative.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/square_root.h>
#include <poincare/old/subtraction.h>

#include <cmath>

namespace Poincare {

int ArcSineNode::numberOfChildren() const {
  return ArcSine::s_functionHelper.numberOfChildren();
}

size_t ArcSineNode::serialize(char* buffer, size_t bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ArcSine::s_functionHelper.aliasesList().mainAlias());
}

OExpression ArcSineNode::shallowReduce(
    const ReductionContext& reductionContext) {
  ArcSine e = ArcSine(this);
  return Trigonometry::ShallowReduceInverseFunction(e, reductionContext);
}

bool ArcSineNode::derivate(const ReductionContext& reductionContext,
                           Symbol symbol, OExpression symbolValue) {
  return ArcSine(this).derivate(reductionContext, symbol, symbolValue);
}

OExpression ArcSineNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return ArcSine(this).unaryFunctionDifferential(reductionContext);
}

bool ArcSine::derivate(const ReductionContext& reductionContext, Symbol symbol,
                       OExpression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue,
                                    reductionContext);
  return true;
}

OExpression ArcSine::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Power::Builder(
      Multiplication::Builder(
          Trigonometry::UnitConversionFactor(reductionContext.angleUnit(),
                                             Preferences::AngleUnit::Radian),
          SquareRoot::Builder(Subtraction::Builder(
              Rational::Builder(1),
              Power::Builder(childAtIndex(0).clone(), Rational::Builder(2))))),
      Rational::Builder(-1));
}

}  // namespace Poincare
