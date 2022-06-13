#include <poincare/norm_cdf_range.h>
#include <poincare/layout_helper.h>
#include <poincare/normal_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

int NormCDFRangeNode::numberOfChildren() const { return NormCDFRange::s_functionHelper.numberOfChildren(); }

Layout NormCDFRangeNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(NormCDFRange(this), floatDisplayMode, numberOfSignificantDigits, NormCDFRange::s_functionHelper.name());
}

int NormCDFRangeNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NormCDFRange::s_functionHelper.name());
}

template<typename T>
Evaluation<T> NormCDFRangeNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  return ApproximationHelper::Map<T>(
      this,
      approximationContext,
      [] (const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * ctx) {
        assert(numberOfComplexes == 4);
        T a = ComplexNode<T>::ToScalar(c[0]);
        T b = ComplexNode<T>::ToScalar(c[1]);
        T mu = ComplexNode<T>::ToScalar(c[2]);
        T sigma = ComplexNode<T>::ToScalar(c[3]);
        if (std::isnan(a) || std::isnan(b) || !NormalDistribution::MuAndSigmaAreOK(mu,sigma)) {
          return Complex<T>::Undefined();
          }
        if (b <= a) {
          return Complex<T>::Builder(static_cast<T>(0.0));
        }
        return Complex<T>::Builder(NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(b, mu, sigma) - NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(a, mu, sigma));
  });
}

}
