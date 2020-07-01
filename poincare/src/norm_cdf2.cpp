#include <poincare/norm_cdf2.h>
#include <poincare/layout_helper.h>
#include <poincare/normal_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper NormCDF2::s_functionHelper;

int NormCDF2Node::numberOfChildren() const { return NormCDF2::s_functionHelper.numberOfChildren(); }

Expression NormCDF2Node::setSign(Sign s, ReductionContext reductionContext) {
  assert(s == Sign::Positive);
  return NormCDF2(this);
}

Layout NormCDF2Node::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(NormCDF2(this), floatDisplayMode, numberOfSignificantDigits, NormCDF2::s_functionHelper.name());
}

int NormCDF2Node::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NormCDF2::s_functionHelper.name());
}

template<typename T>
Evaluation<T> NormCDF2Node::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> aEvaluation = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> bEvaluation = childAtIndex(1)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> muEvaluation = childAtIndex(2)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> sigmaEvaluation = childAtIndex(3)->approximate(T(), context, complexFormat, angleUnit);

  T a = aEvaluation.toScalar();
  T b = bEvaluation.toScalar();
  T mu = muEvaluation.toScalar();
  T sigma = sigmaEvaluation.toScalar();

  if (std::isnan(a) || std::isnan(b) || !NormalDistribution::MuAndSigmaAreOK(mu,sigma)) {
    return Complex<T>::Undefined();
  }
  if (b <= a) {
    return Complex<T>::Builder((T)0.0);
  }
  return Complex<T>::Builder(NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(b, mu, sigma) - NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(a, mu, sigma));
}

}
