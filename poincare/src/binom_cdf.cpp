#include <poincare/binom_cdf.h>
#include <poincare/approximation_helper.h>
#include <poincare/layout_helper.h>
#include <poincare/binomial_distribution.h>
#include <poincare/serialization_helper.h>
#include <assert.h>

namespace Poincare {

int BinomCDFNode::numberOfChildren() const { return BinomCDF::s_functionHelper.numberOfChildren(); }

Layout BinomCDFNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(BinomCDF(this), floatDisplayMode, numberOfSignificantDigits, BinomCDF::s_functionHelper.name());
}

int BinomCDFNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, BinomCDF::s_functionHelper.name());
}

template<typename T>
Evaluation<T> BinomCDFNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  return ApproximationHelper::Map<T>(this,
      approximationContext,
      [] (const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * ctx) {
        assert(numberOfComplexes == 3);
        const T x = c[0].real();
        const T n = c[1].real();
        const T p = c[2].real();
        // CumulativeDistributiveFunctionAtAbscissa handles bad mu and var values
        return Complex<T>::Builder(BinomialDistribution::CumulativeDistributiveFunctionAtAbscissa(x, n, p));
      });
}

}
