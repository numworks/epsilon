#ifndef POINCARE_JUNIOR_CDF_RANGE_METHOD_H
#define POINCARE_JUNIOR_CDF_RANGE_METHOD_H

#include "distribution.h"
#include "distribution_method.h"

namespace PoincareJ {

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

  bool shallowReduce(const Tree** abscissae, const Distribution* distribution,
                     const Tree** parameters, Tree* expression) const override;
};

}  // namespace PoincareJ

#endif
