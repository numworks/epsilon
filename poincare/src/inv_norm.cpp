#include <poincare/inv_norm.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/layout_helper.h>
#include <poincare/normal_distribution.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper InvNorm::s_functionHelper;

int InvNormNode::numberOfChildren() const { return InvNorm::s_functionHelper.numberOfChildren(); }

Layout InvNormNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(InvNorm(this), floatDisplayMode, numberOfSignificantDigits, InvNorm::s_functionHelper.name());
}

int InvNormNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, InvNorm::s_functionHelper.name());
}

Expression InvNormNode::shallowReduce(ReductionContext reductionContext) {
  return InvNorm(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> InvNormNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> aEvaluation = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> muEvaluation = childAtIndex(1)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> varEvaluation = childAtIndex(2)->approximate(T(), context, complexFormat, angleUnit);

  T a = aEvaluation.toScalar();
  T mu = muEvaluation.toScalar();
  T var = varEvaluation.toScalar();

  // CumulativeDistributiveInverseForProbability handles bad mu and var values
  return Complex<T>::Builder(NormalDistribution::CumulativeDistributiveInverseForProbability<T>(a, mu, var));
}

Expression InvNorm::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c0 = childAtIndex(0);
  Expression c1 = childAtIndex(1);
  Expression c2 = childAtIndex(2);
  Context * context = reductionContext.context();

  // Check mu and var
  bool muAndVarOK = false;
  bool couldCheckMuAndVar = NormalDistribution::ExpressionParametersAreOK(&muAndVarOK, c1, c2, context);
  if (!couldCheckMuAndVar) {
    return *this;
  }
  if (!muAndVarOK) {
    return replaceWithUndefinedInPlace();
  }

  // Check a
  if (c0.deepIsMatrix(context)) {
    return replaceWithUndefinedInPlace();
  }
  if (c0.type() != ExpressionNode::Type::Rational) {
    return *this;
  }

  // Special values

  // Undef if x < 0 or x > 1
  Rational r0 = static_cast<Rational &>(c0);
  if (r0.isNegative()) {
    return replaceWithUndefinedInPlace();
  }
  Integer a = r0.unsignedIntegerNumerator();
  Integer b = r0.integerDenominator();
  if (b.isLowerThan(a)) {
    return replaceWithUndefinedInPlace();
  }

  // -INF if x == 0 and +INF if  x == 1
  bool is0 = r0.isZero();
  bool is1 = !is0 && r0.isOne();
  if (is0 || is1) {
    Expression result = Infinity::Builder(is0);
    replaceWithInPlace(result);
    return result;
  }

  // mu if x == 0.5
  if (r0.isHalf()) {
    replaceWithInPlace(c1);
    return c1;
  }

  return *this;
}

}
