#ifndef REGRESSION_REGRESSION_MODEL_H
#define REGRESSION_REGRESSION_MODEL_H

namespace Regression {

class RegressionModel {
public:
  static constexpr int k_maxNumberOfCoefficients = 5;
  virtual double evaluate(double * modelCoefficients, double x) const = 0;
  virtual double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const = 0;
  virtual double numberOfCoefficients() const = 0;
};

}

#endif
