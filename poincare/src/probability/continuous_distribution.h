#ifndef POINCARE_PROBABILITY_CONTINUOUS_DISTRIBUTION_H
#define POINCARE_PROBABILITY_CONTINUOUS_DISTRIBUTION_H

#include "distribution.h"

namespace Poincare::Internal {

class ContinuousDistribution : public Distribution {
 public:
  bool isContinuous() const override { return true; }

  // The range is inclusive on both ends
  float cumulativeDistributiveFunctionForRange(
      float x, float y, const float* parameters) const override {
    if (y <= x) {
      return 0.0f;
    }
    return cumulativeDistributiveFunctionAtAbscissa(y, parameters) -
           cumulativeDistributiveFunctionAtAbscissa(x, parameters);
  }

  double cumulativeDistributiveFunctionForRange(
      double x, double y, const double* parameters) const override {
    if (y <= x) {
      return 0.0;
    }
    return cumulativeDistributiveFunctionAtAbscissa(y, parameters) -
           cumulativeDistributiveFunctionAtAbscissa(x, parameters);
  }
};

}  // namespace Poincare::Internal

#endif
