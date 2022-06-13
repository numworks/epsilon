#include <poincare/norm_cdf.h>
#include <poincare/layout_helper.h>
#include <poincare/normal_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

int NormCDFNode::numberOfChildren() const { return NormCDF::s_functionHelper.numberOfChildren(); }

Layout NormCDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(NormCDF(this), floatDisplayMode, numberOfSignificantDigits, NormCDF::s_functionHelper.name());
}

int NormCDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, NormCDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> NormCDFNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  return ApproximationHelper::Map<T>(
      this,
      approximationContext,
      [] (const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * ctx) {
        assert(numberOfComplexes == 3);
        T a = ComplexNode<T>::ToScalar(c[0]);
        T mu = ComplexNode<T>::ToScalar(c[1]);
        T sigma = ComplexNode<T>::ToScalar(c[2]);
        // CumulativeDistributiveInverseForProbability handles bad mu and var values
        return Complex<T>::Builder(NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(a, mu, sigma));
  });
}

}
