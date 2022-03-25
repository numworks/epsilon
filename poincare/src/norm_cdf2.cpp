#include <poincare/norm_cdf2.h>
#include <poincare/layout_helper.h>
#include <poincare/normal_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper NormCDF2::s_functionHelper;

int NormCDF2Node::numberOfChildren() const { return NormCDF2::s_functionHelper.numberOfChildren(); }

Layout NormCDF2Node::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(NormCDF2(this), floatDisplayMode, numberOfSignificantDigits, NormCDF2::s_functionHelper.name());
}

int NormCDF2Node::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NormCDF2::s_functionHelper.name());
}

template<typename T>
Evaluation<T> NormCDF2Node::templatedApproximate(ApproximationContext approximationContext) const {
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
