#include <poincare/geom_pdf.h>
#include <poincare/layout_helper.h>
#include <poincare/geometric_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper GeomPDF::s_functionHelper;

int GeomPDFNode::numberOfChildren() const { return GeomPDF::s_functionHelper.numberOfChildren(); }

Layout GeomPDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(GeomPDF(this), floatDisplayMode, numberOfSignificantDigits, GeomPDF::s_functionHelper.name());
}

int GeomPDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, GeomPDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> GeomPDFNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> xEvaluation = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> pEvaluation = childAtIndex(1)->approximate(T(), approximationContext);

  T x = xEvaluation.toScalar();
  T p = pEvaluation.toScalar();

  // EvaluateAtAbscissa handles bad n and p values
  return Complex<T>::Builder(GeometricDistribution::EvaluateAtAbscissa(x, p));
}

}
