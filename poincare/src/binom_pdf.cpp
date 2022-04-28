#include <poincare/binom_pdf.h>
#include <poincare/layout_helper.h>
#include <poincare/binomial_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper BinomPDF::s_functionHelper;

int BinomPDFNode::numberOfChildren() const { return BinomPDF::s_functionHelper.numberOfChildren(); }

Layout BinomPDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(BinomPDF(this), floatDisplayMode, numberOfSignificantDigits, BinomPDF::s_functionHelper.name());
}

int BinomPDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, BinomPDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> BinomPDFNode::templatedApproximate(ApproximationContext approximationContext) const {
  return ApproximationHelper::Map<T>(this,
      approximationContext,
      [] (const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * ctx) {
        assert(numberOfComplexes == 3);
        const T x = c[0].real();
        const T n = c[1].real();
        const T p = c[2].real();
        // EvaluateAtAbscissa handles bad n and p values
        return Complex<T>::Builder(BinomialDistribution::EvaluateAtAbscissa(x, n, p));
      });
}

}
