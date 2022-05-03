#include <poincare/geom_cdf.h>
#include <poincare/layout_helper.h>
#include <poincare/geometric_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper GeomCDF::s_functionHelper;

int GeomCDFNode::numberOfChildren() const { return GeomCDF::s_functionHelper.numberOfChildren(); }

Layout GeomCDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(GeomCDF(this), floatDisplayMode, numberOfSignificantDigits, GeomCDF::s_functionHelper.name());
}

int GeomCDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, GeomCDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> GeomCDFNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> xEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> pEvaluation = childAtIndex(1)->approximate(T(), approximationContext);

  const T x = xEvaluation.toScalar();
  const T p = pEvaluation.toScalar();

  // CumulativeDistributiveFunctionAtAbscissa handles bad p values
  return Complex<T>::Builder(GeometricDistribution::CumulativeDistributiveFunctionAtAbscissa(x, p));
}

}
