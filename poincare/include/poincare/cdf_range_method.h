#ifndef POINCARE_CDF_RANGE_METHOD_H
#define POINCARE_CDF_RANGE_METHOD_H

#include "distribution.h"
#include "distribution_method.h"

namespace Poincare {

class CDFRangeMethod final : public DistributionMethod {
  float EvaluateAtAbscissa(float* x, const Distribution* distribution,
                           const float* parameters) const override {
    return distribution->cumulativeDistributiveFunctionForRange(x[0], x[1],
                                                                parameters);
  }

  double EvaluateAtAbscissa(double* x, const Distribution* distribution,
                            const double* parameters) const override {
    return distribution->cumulativeDistributiveFunctionForRange(x[0], x[1],
                                                                parameters);
  }

  OExpression shallowReduce(OExpression* abscissae,
                            const Distribution* distribution,
                            OExpression* parameters,
                            ReductionContext reductionContext,
                            OExpression* expression) const override;
};

}  // namespace Poincare

#endif
