#include <poincare/inv_geom.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/geometric_distribution.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper InvGeom::s_functionHelper;

int InvGeomNode::numberOfChildren() const { return InvGeom::s_functionHelper.numberOfChildren(); }

Layout InvGeomNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(InvGeom(this), floatDisplayMode, numberOfSignificantDigits, InvGeom::s_functionHelper.name());
}

int InvGeomNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, InvGeom::s_functionHelper.name());
}

Expression InvGeomNode::shallowReduce(ReductionContext reductionContext) {
  return InvGeom(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> InvGeomNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> aEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> pEvaluation = childAtIndex(1)->approximate(T(), approximationContext);

  T a = aEvaluation.toScalar();
  T p = pEvaluation.toScalar();

  // CumulativeDistributiveInverseForProbability handles bad n and p values
  return Complex<T>::Builder(GeometricDistribution::CumulativeDistributiveInverseForProbability<T>(a, p));
}

Expression InvGeom::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    bool stopReduction = false;
    Expression e = GeometricDistributionFunction::shallowReduce(reductionContext.context(), &stopReduction);
    if (stopReduction) {
      return e;
    }
  }
  Expression a = childAtIndex(0);
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

  // If a is 1, check if p is also 1
  if (rationalA.isOne()) {
    Expression p = childAtIndex(1);
    if (p.type() != ExpressionNode::Type::Rational) {
      return *this;
    }
    if (static_cast<Rational &>(p).isOne()) {
      Expression result = Rational::Builder(1);
      replaceWithInPlace(result);
      return result;
    }
    Expression result = Infinity::Builder(false);
    replaceWithInPlace(result);
    return result;

  }

  if (rationalA.isZero()) {
    return replaceWithUndefinedInPlace();
  }

  return *this;
}

}
