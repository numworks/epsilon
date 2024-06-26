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
