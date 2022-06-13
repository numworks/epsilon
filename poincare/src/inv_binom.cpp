#include <poincare/inv_binom.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/binomial_distribution.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

int InvBinomNode::numberOfChildren() const { return InvBinom::s_functionHelper.numberOfChildren(); }

Layout InvBinomNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(InvBinom(this), floatDisplayMode, numberOfSignificantDigits, InvBinom::s_functionHelper.name());
}

int InvBinomNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, InvBinom::s_functionHelper.name());
}

Expression InvBinomNode::shallowReduce(const ReductionContext& reductionContext) {
  return InvBinom(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> InvBinomNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  return ApproximationHelper::Map<T>(
      this,
      approximationContext,
      [] (const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * ctx) {
        assert(numberOfComplexes == 3);
        T a = ComplexNode<T>::ToScalar(c[0]);
        T n = ComplexNode<T>::ToScalar(c[1]);
        T p = ComplexNode<T>::ToScalar(c[2]);
        // CumulativeDistributiveInverseForProbability handles bad n and p values
        return Complex<T>::Builder(BinomialDistribution::CumulativeDistributiveInverseForProbability<T>(a, n, p));
      });
}

Expression InvBinom::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::undefinedOnMatrix(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::distributeReductionOverLists(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  {
    bool stopReduction = false;
    Expression e = BinomialDistributionFunction::shallowReduce(reductionContext, &stopReduction);
    if (stopReduction) {
      return e;
    }
  }
  Expression a = childAtIndex(0);
  Expression n = childAtIndex(1);

  // Check a
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
