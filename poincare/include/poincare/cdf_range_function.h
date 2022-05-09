#ifndef POINCARE_CDF_RANGE_FUNCTION_H
#define POINCARE_CDF_RANGE_FUNCTION_H

#include <poincare/distribution.h>
#include <poincare/distribution_method.h>

namespace Poincare {

class CDFRangeFunction final : public DistributionMethod {
  float EvaluateAtAbscissa(float * x, const Distribution * distribution, const float * parameters) const override {
    return distribution->CumulativeDistributiveFunctionForRange(x[0], x[1], parameters);
  }

  double EvaluateAtAbscissa(double * x, const Distribution * distribution, const double * parameters) const override {
    return distribution->CumulativeDistributiveFunctionForRange(x[0], x[1], parameters);
  }
};

}

#endif
