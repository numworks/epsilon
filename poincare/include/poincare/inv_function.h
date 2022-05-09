#ifndef POINCARE_INV_FUNCTION_H
#define POINCARE_INV_FUNCTION_H

#include <poincare/distribution.h>
#include <poincare/distribution_method.h>

namespace Poincare {

class InverseFunction final : public DistributionMethod {
  float EvaluateAtAbscissa(float * x, Distribution * distribution, const float * parameters) const override {
    return distribution->CumulativeDistributiveInverseForProbability(x[0], parameters);
  }

  double EvaluateAtAbscissa(double * x, Distribution * distribution, const double * parameters) const override {
    return distribution->CumulativeDistributiveInverseForProbability(x[0], parameters);
  }

  Expression shallowReduce(Expression * x, Context * context, Expression * expression) const override;
};

}

#endif
