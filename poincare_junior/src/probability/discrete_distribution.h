#ifndef POINCARE_DISCRETE_DISTRIBUTION_H
#define POINCARE_DISCRETE_DISTRIBUTION_H

#include <poincare/distribution.h>

namespace Poincare {

// More precisely distributions deriving from this should be defined on N
class DiscreteDistribution : public Distribution {
 public:
  bool isContinuous() const override { return false; }

  template <typename T>
  T CumulativeDistributiveFunctionAtAbscissa(T x, const T* parameters) const;
  float cumulativeDistributiveFunctionAtAbscissa(
      float x, const float* parameters) const override {
    return CumulativeDistributiveFunctionAtAbscissa<float>(x, parameters);
  }
  double cumulativeDistributiveFunctionAtAbscissa(
      double x, const double* parameters) const override {
    return CumulativeDistributiveFunctionAtAbscissa<double>(x, parameters);
  }

  // The range is inclusive on both ends
  float cumulativeDistributiveFunctionForRange(
      float x, float y, const float* parameters) const override {
    if (y < x) {
      return 0.0f;
    }
    return CumulativeDistributiveFunctionAtAbscissa(y, parameters) -
           CumulativeDistributiveFunctionAtAbscissa(x - 1.0f, parameters);
  }

  double cumulativeDistributiveFunctionForRange(
      double x, double y, const double* parameters) const override {
    if (y < x) {
      return 0.0;
    }
    return CumulativeDistributiveFunctionAtAbscissa(y, parameters) -
           CumulativeDistributiveFunctionAtAbscissa(x - 1.0, parameters);
  }
};

}  // namespace Poincare

#endif
