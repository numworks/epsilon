#ifndef POINCARE_DISCRETE_DISTRIBUTION_H
#define POINCARE_DISCRETE_DISTRIBUTION_H

#include <poincare/distribution.h>

namespace Poincare {

class DiscreteDistribution : public Distribution {
public:
  // The range is inclusive on both ends
  float CumulativeDistributiveFunctionForRange(float x, float y, const float * parameters) override {
    return CumulativeDistributiveFunctionAtAbscissa(y, parameters) - CumulativeDistributiveFunctionAtAbscissa(x - 1.0f, parameters);
      }

  double CumulativeDistributiveFunctionForRange(double x, double y, const double * parameters) override {
    return CumulativeDistributiveFunctionAtAbscissa(y, parameters) - CumulativeDistributiveFunctionAtAbscissa(x - 1.0, parameters);
  }
};

}

#endif
