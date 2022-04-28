#include <poincare/norm_cdf_range.h>
#include <poincare/layout_helper.h>
#include <poincare/normal_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper NormCDFRange::s_functionHelper;

int NormCDFRangeNode::numberOfChildren() const { return NormCDFRange::s_functionHelper.numberOfChildren(); }

Layout NormCDFRangeNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(NormCDFRange(this), floatDisplayMode, numberOfSignificantDigits, NormCDFRange::s_functionHelper.name());
}

int NormCDFRangeNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NormCDFRange::s_functionHelper.name());
}

template<typename T>
Evaluation<T> NormCDFRangeNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> aEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> bEvaluation = childAtIndex(1)->approximate(T(), approximationContext);
  Evaluation<T> muEvaluation = childAtIndex(2)->approximate(T(), approximationContext);
  Evaluation<T> sigmaEvaluation = childAtIndex(3)->approximate(T(), approximationContext);

  T a = aEvaluation.toScalar();
  T b = bEvaluation.toScalar();
  T mu = muEvaluation.toScalar();
  T sigma = sigmaEvaluation.toScalar();

  if (std::isnan(a) || std::isnan(b) || !NormalDistribution::MuAndSigmaAreOK(mu,sigma)) {
    return Complex<T>::Undefined();
  }
  if (b <= a) {
    return Complex<T>::Builder(static_cast<T>(0.0));
  }
  return Complex<T>::Builder(NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(b, mu, sigma) - NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(a, mu, sigma));
}

}
