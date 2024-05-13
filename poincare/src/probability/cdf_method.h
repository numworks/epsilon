#ifndef POINCARE_PROBABILITY_CDF_METHOD_H
#define POINCARE_PROBABILITY_CDF_METHOD_H

#include <poincare/probability/distribution.h>

#include "distribution_method.h"

namespace Poincare::Internal {

class CDFMethod final : public DistributionMethod {
  float EvaluateAtAbscissa(float* x, const Distribution* distribution,
                           const float* parameters) const override {
    return distribution->cumulativeDistributiveFunctionAtAbscissa(x[0],
                                                                  parameters);
  }

  double EvaluateAtAbscissa(double* x, const Distribution* distribution,
                            const double* parameters) const override {
    return distribution->cumulativeDistributiveFunctionAtAbscissa(x[0],
                                                                  parameters);
  }

  bool shallowReduce(const Tree** abscissae, const Distribution* distribution,
                     const Tree** parameters, Tree* expression) const override;
};

}  // namespace Poincare::Internal

#endif
