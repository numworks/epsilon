#include <poincare/mixed_fraction.h>
#include <poincare/addition.h>
#include <poincare/complex.h>
#include <poincare/division.h>
#include <poincare/division_quotient.h>
#include <poincare/division_remainder.h>
#include <poincare/evaluation.h>
#include <poincare/layout_helper.h>
#include <poincare/parenthesis.h>
#include <poincare/opposite.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <cmath>

namespace Poincare {

Expression MixedFractionNode::shallowReduce(const ReductionContext& reductionContext) {
  return MixedFraction(this).shallowReduce(reductionContext);
}

Layout MixedFractionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Infix(MixedFraction(this), floatDisplayMode, numberOfSignificantDigits, "");
}

int MixedFractionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, " ");
}

template <typename T> Evaluation<T> MixedFractionNode::templateApproximate(const ApproximationContext& approximationContext) const {
  Evaluation<T> integerPart = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> fractionPart = childAtIndex(1)->approximate(T(), approximationContext);
  if (integerPart.type() != EvaluationNode<T>::Type::Complex || integerPart.type() != EvaluationNode<T>::Type::Complex) {
    return Complex<T>::Undefined();
  }
  T evaluatedIntegerPart = integerPart.toScalar();
  T evaluatedFractionPart = fractionPart.toScalar();
  if (evaluatedFractionPart < 0.0 || evaluatedIntegerPart != std::fabs(evaluatedIntegerPart)) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(evaluatedIntegerPart + evaluatedFractionPart);

}

Expression MixedFraction::shallowReduce(const ExpressionNode::ReductionContext& context) {
  Expression integerPart = childAtIndex(0);
  Expression fractionPart = childAtIndex(1);
  if (integerPart.type() != ExpressionNode::Type::Rational || fractionPart.type() != ExpressionNode::Type::Rational) {
    return replaceWithUndefinedInPlace();
  }
  Rational rationalIntegerPart = static_cast<Rational &>(integerPart);
  Rational rationalFractionPart = static_cast<Rational &>(fractionPart);
  if (!rationalIntegerPart.isInteger() || rationalFractionPart.isNegative()) {
    return replaceWithUndefinedInPlace();
  }
  Addition result = Addition::Builder(integerPart, fractionPart);
  replaceWithInPlace(result);
  return result.shallowReduce(context);
}

Expression MixedFraction::CreateMixedFractionFromIntegers(const Integer & num, const Integer & denom) {
  Integer numPositive(num), denomPositive(denom);
  numPositive.setNegative(false);
  denomPositive.setNegative(false);
  Expression quo = DivisionQuotient::Reduce(numPositive, denomPositive);
  Expression rem = DivisionRemainder::Reduce(numPositive, denomPositive);
  Expression mixedFraction = Builder(quo, Division::Builder(rem, Rational::Builder(denomPositive)));
  if (num.isNegative() != denom.isNegative()) {
    mixedFraction = Opposite::Builder(mixedFraction);
  }
  return mixedFraction;
}

}
