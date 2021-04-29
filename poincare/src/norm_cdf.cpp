#include <poincare/norm_cdf.h>
#include <poincare/layout_helper.h>
#include <poincare/normal_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper NormCDF::s_functionHelper;

int NormCDFNode::numberOfChildren() const { return NormCDF::s_functionHelper.numberOfChildren(); }

Expression NormCDFNode::setSign(Sign s, ReductionContext reductionContext) {
  assert(s == sign(reductionContext.context()));
  return NormCDF(this);
}

Layout NormCDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(NormCDF(this), floatDisplayMode, numberOfSignificantDigits, NormCDF::s_functionHelper.name());
}

int NormCDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NormCDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> NormCDFNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> aEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> muEvaluation = childAtIndex(1)->approximate(T(), approximationContext);
  Evaluation<T> sigmaEvaluation = childAtIndex(2)->approximate(T(), approximationContext);

  const T a = aEvaluation.toScalar();
  const T mu = muEvaluation.toScalar();
  const T sigma = sigmaEvaluation.toScalar();

  // CumulativeDistributiveFunctionAtAbscissa handles bad mu and var values
  return Complex<T>::Builder(NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(a, mu, sigma));
}

}
