#ifndef POINCARE_CDF_METHOD_H
#define POINCARE_CDF_METHOD_H

#include <poincare/distribution.h>
#include <poincare/distribution_method.h>

namespace Poincare {

class CDFMethod final : public DistributionMethod {
  float EvaluateAtAbscissa(float * x, const Distribution * distribution, const float * parameters) const override {
    return distribution->cumulativeDistributiveFunctionAtAbscissa(x[0], parameters);
  }

  double EvaluateAtAbscissa(double * x, const Distribution * distribution, const double * parameters) const override {
    return distribution->cumulativeDistributiveFunctionAtAbscissa(x[0], parameters);
  }

  Expression shallowReduce(Expression * abscissae, const Distribution * distribution, Expression * parameters, ReductionContext reductionContext, Expression * expression) const override;

};

}

#endif
