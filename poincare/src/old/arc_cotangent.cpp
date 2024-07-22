#include <poincare/layout.h>
#include <poincare/old/addition.h>
#include <poincare/old/arc_tangent.h>
#include <poincare/old/complex.h>
#include <poincare/old/derivative.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/trigonometry.h>

#include <cmath>

namespace Poincare {

int ArcCotangentNode::numberOfChildren() const {
  return ArcCotangent::s_functionHelper.numberOfChildren();
}

size_t ArcCotangentNode::serialize(char* buffer, size_t bufferSize,
                                   Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ArcCotangent::s_functionHelper.aliasesList().mainAlias());
}

// TODO_PCJ: Delete this method
OExpression ArcCotangentNode::shallowReduce(
    const ReductionContext& reductionContext) {
  assert(false);
  return this;
  // ArcCotangent e = ArcCotangent(this);
  // return Trigonometry::ShallowReduceInverseAdvancedFunction(e,
  //                                                           reductionContext);
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
