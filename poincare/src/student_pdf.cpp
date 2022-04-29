#include <poincare/student_pdf.h>
#include <poincare/layout_helper.h>
#include <poincare/student_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper StudentPDF::s_functionHelper;

int StudentPDFNode::numberOfChildren() const { return StudentPDF::s_functionHelper.numberOfChildren(); }

Layout StudentPDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(StudentPDF(this), floatDisplayMode, numberOfSignificantDigits, StudentPDF::s_functionHelper.name());
}

int StudentPDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, StudentPDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> StudentPDFNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> xEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> kEvaluation = childAtIndex(1)->approximate(T(), approximationContext);

  T x = xEvaluation.toScalar();
  T k = kEvaluation.toScalar();

  // EvaluateAtAbscissa handles bad k values
  return Complex<T>::Builder(StudentDistribution::EvaluateAtAbscissa(x, k));
}

}
