#include <poincare/inv_binom.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/binomial_distribution.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper InvBinom::s_functionHelper;

int InvBinomNode::numberOfChildren() const { return InvBinom::s_functionHelper.numberOfChildren(); }

Layout InvBinomNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(InvBinom(this), floatDisplayMode, numberOfSignificantDigits, InvBinom::s_functionHelper.name());
}

int InvBinomNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, InvBinom::s_functionHelper.name());
}

Expression InvBinomNode::shallowReduce(ReductionContext reductionContext) {
  return InvBinom(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> InvBinomNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> aEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> nEvaluation = childAtIndex(1)->approximate(T(), approximationContext);
  Evaluation<T> pEvaluation = childAtIndex(2)->approximate(T(), approximationContext);

  T a = aEvaluation.toScalar();
  T n = nEvaluation.toScalar();
  T p = pEvaluation.toScalar();

  // CumulativeDistributiveInverseForProbability handles bad n and p values
  return Complex<T>::Builder(BinomialDistribution::CumulativeDistributiveInverseForProbability<T>(a, n, p));
}

Expression InvBinom::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    bool stopReduction = false;
    Expression e = BinomialDistributionFunction::shallowReduce(reductionContext, &stopReduction);
    if (stopReduction) {
      return e;
    }
  }
  Expression a = childAtIndex(0);
  Expression n = childAtIndex(1);
  Context * context = reductionContext.context();

  // Check a
  if (a.deepIsMatrix(context)) {
    return replaceWithUndefinedInPlace();
  }
  if (a.type() != ExpressionNode::Type::Rational) {
    return *this;
  }

  // Special values

  // Undef if a < 0 or a > 1
  Rational rationalA = static_cast<Rational &>(a);
  if (rationalA.isNegative()) {
    return replaceWithUndefinedInPlace();
  }
  Integer num = rationalA.unsignedIntegerNumerator();
  Integer den = rationalA.integerDenominator();
  if (den.isLowerThan(num)) {
    return replaceWithUndefinedInPlace();
  }

  // If a == 0, check p
  if (rationalA.isZero()) {
    Expression p = childAtIndex(2);
    if (p.type() != ExpressionNode::Type::Rational) {
      return *this;
    }
    if (static_cast<Rational &>(p).isOne()) {
      Expression result = Rational::Builder(0);
      replaceWithInPlace(result);
      return result;
    }
    return replaceWithUndefinedInPlace();
  }
  // n if a == 1
  if (rationalA.isOne()) {
    replaceWithInPlace(n);
    return n;
  }

  return *this;
}

}
