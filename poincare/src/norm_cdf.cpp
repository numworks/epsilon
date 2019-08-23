#include <poincare/norm_cdf.h>
#include <poincare/layout_helper.h>
#include <poincare/normal_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper NormCDF::s_functionHelper;

int NormCDFNode::numberOfChildren() const { return NormCDF::s_functionHelper.numberOfChildren(); }

Expression NormCDFNode::setSign(Sign s, ReductionContext reductionContext) {
  assert(s == Sign::Positive);
  return NormCDF(this);
}

Layout NormCDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(NormCDF(this), floatDisplayMode, numberOfSignificantDigits, NormCDF::s_functionHelper.name());
}

int NormCDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NormCDF::s_functionHelper.name());
}

Expression NormCDFNode::shallowReduce(ReductionContext reductionContext) {
  return NormCDF(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> NormCDFNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> aEvaluation = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> muEvaluation = childAtIndex(1)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> sigmaEvaluation = childAtIndex(2)->approximate(T(), context, complexFormat, angleUnit);

  T a = aEvaluation.toScalar();
  T mu = muEvaluation.toScalar();
  T sigma = sigmaEvaluation.toScalar();

  if (std::isnan(a) || std::isnan(mu) || std::isnan(sigma)) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(a, mu, sigma));
}

Expression NormCDF::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
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
