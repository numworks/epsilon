#include <poincare/poisson_pdf.h>
#include <poincare/layout_helper.h>
#include <poincare/poisson_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper PoissonPDF::s_functionHelper;

int PoissonPDFNode::numberOfChildren() const { return PoissonPDF::s_functionHelper.numberOfChildren(); }

Layout PoissonPDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(PoissonPDF(this), floatDisplayMode, numberOfSignificantDigits, PoissonPDF::s_functionHelper.name());
}

int PoissonPDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, PoissonPDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> PoissonPDFNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> xEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> lambdaEvaluation = childAtIndex(1)->approximate(T(), approximationContext);

  T x = xEvaluation.toScalar();
  T lambda = lambdaEvaluation.toScalar();

  // EvaluateAtAbscissa handles bad n and p values
  return Complex<T>::Builder(PoissonDistribution::EvaluateAtAbscissa(x, lambda));
}

}
