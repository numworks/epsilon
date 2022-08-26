#include <poincare/inv_norm.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/normal_distribution.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

int InvNormNode::numberOfChildren() const { return InvNorm::s_functionHelper.numberOfChildren(); }

Layout InvNormNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(InvNorm(this), floatDisplayMode, numberOfSignificantDigits, InvNorm::s_functionHelper.aliasesList().mainAlias(), context);
}

int InvNormNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, InvNorm::s_functionHelper.aliasesList().mainAlias());
}

Expression InvNormNode::shallowReduce(const ReductionContext& reductionContext) {
  return InvNorm(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> InvNormNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  return ApproximationHelper::Map<T>(
      this,
      approximationContext,
      [] (const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * ctx) {
        assert(numberOfComplexes == 3);
        T a = ComplexNode<T>::ToScalar(c[0]);
        T mu = ComplexNode<T>::ToScalar(c[1]);
        T sigma = ComplexNode<T>::ToScalar(c[2]);
        // CumulativeDistributiveInverseForProbability handles bad mu and var values
        return Complex<T>::Builder(NormalDistribution::CumulativeDistributiveInverseForProbability<T>(a, mu, sigma));
      });
}

Expression InvNorm::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  {
    bool stopReduction = false;
    Expression e = NormalDistributionFunction::shallowReduce(reductionContext, &stopReduction);
    if (stopReduction) {
      return e;
    }
  }
  Expression a = childAtIndex(0);
  Expression mu = childAtIndex(1);

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

  // -INF if a == 0 and +INF if a == 1
  bool is0 = rationalA.isZero();
  bool is1 = !is0 && rationalA.isOne();
  if (is0 || is1) {
    Expression result = Infinity::Builder(is0);
    replaceWithInPlace(result);
    return result;
  }

  // mu if a == 0.5
  if (rationalA.isHalf()) {
    replaceWithInPlace(mu);
    return mu;
  }

  return *this;
}

}
