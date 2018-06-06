#ifndef REGRESSION_MODEL_H
#define REGRESSION_MODEL_H

#include <stdint.h>

namespace Regression {

class Model {
public:
  enum class Type : uint8_t {
    Linear      = 0,
    Quadratic   = 1,
    Cubic       = 2,
    Quartic     = 3,
    Logarithmic = 4,
    Exponential = 5,
    Power       = 6,
    Trigonometric = 7,
    Logistic    = 8
  };
  static constexpr int k_maxNumberOfCoefficients = 5;
  virtual double evaluate(double * modelCoefficients, double x) const = 0;
  virtual double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const = 0;
  virtual double numberOfCoefficients() const = 0;
};

}

#endif
