#include <poincare/binom_pdf.h>
#include <poincare/layout_helper.h>
#include <poincare/binomial_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper BinomPDF::s_functionHelper;

int BinomPDFNode::numberOfChildren() const { return BinomPDF::s_functionHelper.numberOfChildren(); }

Expression BinomPDFNode::setSign(Sign s, ReductionContext reductionContext) {
  assert(s == Sign::Positive);
  return BinomPDF(this);
}

Layout BinomPDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(BinomPDF(this), floatDisplayMode, numberOfSignificantDigits, BinomPDF::s_functionHelper.name());
}

int BinomPDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, BinomPDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> BinomPDFNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> xEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> nEvaluation = childAtIndex(1)->approximate(T(), approximationContext);
  Evaluation<T> pEvaluation = childAtIndex(2)->approximate(T(), approximationContext);

  T x = xEvaluation.toScalar();
  T n = nEvaluation.toScalar();
  T p = pEvaluation.toScalar();

  // EvaluateAtAbscissa handles bad n and p values
  return Complex<T>::Builder(BinomialDistribution::EvaluateAtAbscissa(x, n, p));
}

}
