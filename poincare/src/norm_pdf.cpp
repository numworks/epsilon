#include <poincare/norm_pdf.h>
#include <poincare/layout_helper.h>
#include <poincare/normal_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

int NormPDFNode::numberOfChildren() const { return NormPDF::s_functionHelper.numberOfChildren(); }

Layout NormPDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(NormPDF(this), floatDisplayMode, numberOfSignificantDigits, NormPDF::s_functionHelper.name());
}

int NormPDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NormPDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> NormPDFNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  return ApproximationHelper::Map<T>(
      this,
      approximationContext,
      [] (const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * ctx) {
        assert(numberOfComplexes == 3);
        T x = ComplexNode<T>::ToScalar(c[0]);
        T mu = ComplexNode<T>::ToScalar(c[1]);
        T sigma = ComplexNode<T>::ToScalar(c[2]);
        // CumulativeDistributiveInverseForProbability handles bad mu and var values
        return Complex<T>::Builder(NormalDistribution::EvaluateAtAbscissa(x, mu, sigma));
      });
}

}
