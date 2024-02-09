#ifndef POINCARE_JUNIOR_CONTINUOUS_DISTRIBUTION_H
#define POINCARE_JUNIOR_CONTINUOUS_DISTRIBUTION_H

#include <poincare/distribution.h>

namespace PoincareJ {

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

}  // namespace PoincareJ

#endif
