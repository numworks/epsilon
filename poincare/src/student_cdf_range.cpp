#include <poincare/student_cdf_range.h>
#include <poincare/layout_helper.h>
#include <poincare/student_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper StudentCDFRange::s_functionHelper;

int StudentCDFRangeNode::numberOfChildren() const { return StudentCDFRange::s_functionHelper.numberOfChildren(); }

Layout StudentCDFRangeNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(StudentCDFRange(this), floatDisplayMode, numberOfSignificantDigits, StudentCDFRange::s_functionHelper.name());
}

int StudentCDFRangeNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, StudentCDFRange::s_functionHelper.name());
}

template<typename T>
Evaluation<T> StudentCDFRangeNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> aEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> bEvaluation = childAtIndex(1)->approximate(T(), approximationContext);
  Evaluation<T> kEvaluation = childAtIndex(2)->approximate(T(), approximationContext);

  T a = aEvaluation.toScalar();
  T b = bEvaluation.toScalar();
  T k = kEvaluation.toScalar();

  if (std::isnan(a) || std::isnan(b) || !StudentDistribution::KIsOK(k)) {
    return Complex<T>::Undefined();
  }
  if (b <= a) {
    return Complex<T>::Builder(static_cast<T>(0.0));
  }
  return Complex<T>::Builder(StudentDistribution::CumulativeDistributiveFunctionAtAbscissa(b, k) - StudentDistribution::CumulativeDistributiveFunctionAtAbscissa(a, k));
}

}
