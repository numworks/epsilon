#ifndef POINCARE_JUNIOR_INV_METHOD_H
#define POINCARE_JUNIOR_INV_METHOD_H

#include "distribution.h"
#include "distribution_method.h"

namespace PoincareJ {

class InverseMethod final : public DistributionMethod {
  float EvaluateAtAbscissa(float* x, const Distribution* distribution,
                           const float* parameters) const override {
    return distribution->cumulativeDistributiveInverseForProbability(
        x[0], parameters);
  }

  double EvaluateAtAbscissa(double* x, const Distribution* distribution,
                            const double* parameters) const override {
    return distribution->cumulativeDistributiveInverseForProbability(
        x[0], parameters);
  }

  bool shallowReduce(const Tree** abscissae, const Distribution* distribution,
                     const Tree** parameters, Tree* expression) const override;
};

}  // namespace PoincareJ

#endif
