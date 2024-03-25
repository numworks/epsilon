#include <poincare/addition.h>
#include <poincare/complex.h>
#include <poincare/division_quotient.h>
#include <poincare/division_remainder.h>
#include <poincare/evaluation.h>
#include <poincare/layout.h>
#include <poincare/mixed_fraction.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

#include <cmath>

namespace Poincare {

OExpression MixedFractionNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return MixedFraction(this).shallowReduce(reductionContext);
}

size_t MixedFractionNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode,
                                    numberOfSignificantDigits, " ");
}

template <typename T>
Evaluation<T> MixedFractionNode::templateApproximate(
    const ApproximationContext& approximationContext) const {
  Evaluation<T> integerPart =
      childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> fractionPart =
      childAtIndex(1)->approximate(T(), approximationContext);
  if (integerPart.otype() != EvaluationNode<T>::Type::Complex ||
      fractionPart.otype() != EvaluationNode<T>::Type::Complex) {
    return Complex<T>::Undefined();
  }
  T evaluatedIntegerPart = integerPart.toScalar();
  T evaluatedFractionPart = fractionPart.toScalar();
  if (evaluatedFractionPart < 0.0 ||
      evaluatedIntegerPart != std::fabs(evaluatedIntegerPart)) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(evaluatedIntegerPart + evaluatedFractionPart);
}

OExpression MixedFraction::shallowReduce(ReductionContext context) {
  OExpression integerPart = childAtIndex(0);
  OExpression fractionPart = childAtIndex(1);
  if (integerPart.otype() != ExpressionNode::Type::Rational ||
      fractionPart.otype() != ExpressionNode::Type::Rational) {
    return replaceWithUndefinedInPlace();
  }
  Rational rationalIntegerPart = static_cast<Rational&>(integerPart);
  Rational rationalFractionPart = static_cast<Rational&>(fractionPart);
  if (!rationalIntegerPart.isInteger() || rationalFractionPart.isNegative()) {
    return replaceWithUndefinedInPlace();
  }
  Addition result = Addition::Builder(integerPart, fractionPart);
  replaceWithInPlace(result);
  return result.shallowReduce(context);
}

}  // namespace Poincare
