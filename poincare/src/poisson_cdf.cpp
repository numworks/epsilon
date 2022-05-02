#include <poincare/poisson_cdf.h>
#include <poincare/layout_helper.h>
#include <poincare/poisson_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper PoissonCDF::s_functionHelper;

int PoissonCDFNode::numberOfChildren() const { return PoissonCDF::s_functionHelper.numberOfChildren(); }

Layout PoissonCDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(PoissonCDF(this), floatDisplayMode, numberOfSignificantDigits, PoissonCDF::s_functionHelper.name());
}

int PoissonCDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, PoissonCDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> PoissonCDFNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> xEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> lambdaEvaluation = childAtIndex(1)->approximate(T(), approximationContext);

  const T x = xEvaluation.toScalar();
  const T lambda = lambdaEvaluation.toScalar();

  // CumulativeDistributiveFunctionAtAbscissa handles bad lambda values
  return Complex<T>::Builder(PoissonDistribution::CumulativeDistributiveFunctionAtAbscissa(x, lambda));
}

}
