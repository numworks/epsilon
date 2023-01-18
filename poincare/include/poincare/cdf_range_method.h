#ifndef POINCARE_CDF_RANGE_METHOD_H
#define POINCARE_CDF_RANGE_METHOD_H

#include <poincare/distribution.h>
#include <poincare/distribution_method.h>

namespace Poincare {

class CDFRangeMethod final : public DistributionMethod {
  float EvaluateAtAbscissa(float * x, const Distribution * distribution, const float * parameters) const override {
    return distribution->cumulativeDistributiveFunctionForRange(x[0], x[1], parameters);
  }

  double EvaluateAtAbscissa(double * x, const Distribution * distribution, const double * parameters) const override {
    return distribution->cumulativeDistributiveFunctionForRange(x[0], x[1], parameters);
  }

  Expression shallowReduce(Expression * abscissae, const Distribution * distribution, Expression * parameters, ReductionContext reductionContext, Expression * expression) const override;

};

}

#endif
