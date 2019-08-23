#include <poincare/inv_norm.h>
#include <poincare/layout_helper.h>
#include <poincare/normal_distribution.h>
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
  Evaluation<T> sigmaEvaluation = childAtIndex(2)->approximate(T(), context, complexFormat, angleUnit);

  T a = aEvaluation.toScalar();
  T mu = muEvaluation.toScalar();
  T sigma = sigmaEvaluation.toScalar();

  if (std::isnan(a) || std::isnan(mu) || std::isnan(sigma)) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(NormalDistribution::CumulativeDistributiveInverseForProbability<T>(a, mu, sigma));
}

Expression InvNorm::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  //TODO LEA
  return *this;
}

}
