#include <poincare/binom_pdf.h>
#include <poincare/layout_helper.h>
#include <poincare/binomial_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

int BinomPDFNode::numberOfChildren() const { return BinomPDF::s_functionHelper.numberOfChildren(); }

Layout BinomPDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(BinomPDF(this), floatDisplayMode, numberOfSignificantDigits, BinomPDF::s_functionHelper.name());
}

int BinomPDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, BinomPDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> BinomPDFNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  return ApproximationHelper::Map<T>(this,
      approximationContext,
      [] (const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * ctx) {
        assert(numberOfComplexes == 3);
        const T x = ComplexNode<T>::ToScalar(c[0]);
        const T n = ComplexNode<T>::ToScalar(c[1]);
        const T p = ComplexNode<T>::ToScalar(c[2]);
        // EvaluateAtAbscissa handles bad n and p values
        return Complex<T>::Builder(BinomialDistribution::EvaluateAtAbscissa(x, n, p));
      });
}

}
