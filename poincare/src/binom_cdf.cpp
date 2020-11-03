#include <poincare/binom_cdf.h>
#include <poincare/layout_helper.h>
#include <poincare/binomial_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper BinomCDF::s_functionHelper;

int BinomCDFNode::numberOfChildren() const { return BinomCDF::s_functionHelper.numberOfChildren(); }

Expression BinomCDFNode::setSign(Sign s, ReductionContext reductionContext) {
  assert(s == Sign::Positive);
  return BinomCDF(this);
}

Layout BinomCDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(BinomCDF(this), floatDisplayMode, numberOfSignificantDigits, BinomCDF::s_functionHelper.name());
}

int BinomCDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, BinomCDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> BinomCDFNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> xEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> nEvaluation = childAtIndex(1)->approximate(T(), approximationContext);
  Evaluation<T> pEvaluation = childAtIndex(2)->approximate(T(), approximationContext);

  const T x = xEvaluation.toScalar();
  const T n = nEvaluation.toScalar();
  const T p = pEvaluation.toScalar();

  // CumulativeDistributiveFunctionAtAbscissa handles bad mu and var values
  return Complex<T>::Builder(BinomialDistribution::CumulativeDistributiveFunctionAtAbscissa(x, n, p));
}

}
