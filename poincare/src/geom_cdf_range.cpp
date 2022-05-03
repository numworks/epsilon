#include <poincare/geom_cdf_range.h>
#include <poincare/layout_helper.h>
#include <poincare/geometric_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper GeomCDFRange::s_functionHelper;

int GeomCDFRangeNode::numberOfChildren() const { return GeomCDFRange::s_functionHelper.numberOfChildren(); }

Layout GeomCDFRangeNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(GeomCDFRange(this), floatDisplayMode, numberOfSignificantDigits, GeomCDFRange::s_functionHelper.name());
}

int GeomCDFRangeNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, GeomCDFRange::s_functionHelper.name());
}

template<typename T>
Evaluation<T> GeomCDFRangeNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> xEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> yEvaluation = childAtIndex(1)->approximate(T(), approximationContext);
  Evaluation<T> pEvaluation = childAtIndex(2)->approximate(T(), approximationContext);

  const T x = xEvaluation.toScalar();
  const T y = yEvaluation.toScalar();
  const T p = pEvaluation.toScalar();

  if (y < x) {
    return Complex<T>::Builder(static_cast<T>(0.0));
  }
  // CumulativeDistributiveFunctionAtAbscissa handles bad p values
  // TODO: use a for loop via Solver
  return Complex<T>::Builder(GeometricDistribution::CumulativeDistributiveFunctionAtAbscissa(y, p) - GeometricDistribution::CumulativeDistributiveFunctionAtAbscissa(x - 1, p));
}

}
