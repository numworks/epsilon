#include <poincare/student_cdf.h>
#include <poincare/layout_helper.h>
#include <poincare/student_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper StudentCDF::s_functionHelper;

int StudentCDFNode::numberOfChildren() const { return StudentCDF::s_functionHelper.numberOfChildren(); }

Layout StudentCDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(StudentCDF(this), floatDisplayMode, numberOfSignificantDigits, StudentCDF::s_functionHelper.name());
}

int StudentCDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, StudentCDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> StudentCDFNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> aEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> kEvaluation = childAtIndex(1)->approximate(T(), approximationContext);

  const T a = aEvaluation.toScalar();
  const T k = kEvaluation.toScalar();

  // CumulativeDistributiveFunctionAtAbscissa handles bad k values
  return Complex<T>::Builder(StudentDistribution::CumulativeDistributiveFunctionAtAbscissa(a, k));
}

}
