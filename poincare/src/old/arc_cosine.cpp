#include <poincare/layout.h>
#include <poincare/old/arc_cosine.h>
#include <poincare/old/complex.h>
#include <poincare/old/conjugate.h>
#include <poincare/old/derivative.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/square_root.h>
#include <poincare/old/subtraction.h>

#include <cmath>

namespace Poincare {

int ArcCosineNode::numberOfChildren() const {
  return ArcCosine::s_functionHelper.numberOfChildren();
}

size_t ArcCosineNode::serialize(char* buffer, size_t bufferSize,
                                Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ArcCosine::s_functionHelper.aliasesList().mainAlias());
}

OExpression ArcCosineNode::shallowReduce(
    const ReductionContext& reductionContext) {
  ArcCosine e = ArcCosine(this);
  return Trigonometry::ShallowReduceInverseFunction(e, reductionContext);
}

bool ArcCosineNode::derivate(const ReductionContext& reductionContext,
                             Symbol symbol, OExpression symbolValue) {
  return ArcCosine(this).derivate(reductionContext, symbol, symbolValue);
}

OExpression ArcCosineNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return ArcCosine(this).unaryFunctionDifferential(reductionContext);
}

bool ArcCosine::derivate(const ReductionContext& reductionContext,
                         Symbol symbol, OExpression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue,
                                    reductionContext);
  return true;
}

OExpression ArcCosine::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Power::Builder(
      Multiplication::Builder(
          Rational::Builder(-1),
          Trigonometry::UnitConversionFactor(reductionContext.angleUnit(),
                                             Preferences::AngleUnit::Radian),
          SquareRoot::Builder(Subtraction::Builder(
              Rational::Builder(1),
              Power::Builder(childAtIndex(0).clone(), Rational::Builder(2))))),
      Rational::Builder(-1));
}

}  // namespace Poincare
