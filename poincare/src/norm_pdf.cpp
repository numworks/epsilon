#include <poincare/norm_pdf.h>
#include <poincare/layout_helper.h>
#include <poincare/normal_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper NormPDF::s_functionHelper;

int NormPDFNode::numberOfChildren() const { return NormPDF::s_functionHelper.numberOfChildren(); }

Expression NormPDFNode::setSign(Sign s, ReductionContext reductionContext) {
  assert(s == Sign::Positive);
  return NormPDF(this);
}

Layout NormPDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(NormPDF(this), floatDisplayMode, numberOfSignificantDigits, NormPDF::s_functionHelper.name());
}

int NormPDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NormPDF::s_functionHelper.name());
}

Expression NormPDFNode::shallowReduce(ReductionContext reductionContext) {
  return NormPDF(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> NormPDFNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> xEvaluation = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> muEvaluation = childAtIndex(1)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> varEvaluation = childAtIndex(2)->approximate(T(), context, complexFormat, angleUnit);

  T x = xEvaluation.toScalar();
  T mu = muEvaluation.toScalar();
  T var = varEvaluation.toScalar();

  if (std::isnan(x) || std::isnan(mu) || std::isnan(var)) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(NormalDistribution::EvaluateAtAbscissa(x, mu, var));
}

Expression NormPDF::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  //TODO LEA
  return *this;
}

}
