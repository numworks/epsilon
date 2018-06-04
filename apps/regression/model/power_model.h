#ifndef REGRESSION_POWER_MODEL_H
#define REGRESSION_POWER_MODEL_H

#include "regression_model.h"

namespace Regression {

class PowerModel : public RegressionModel {
public:
  using RegressionModel::RegressionModel;
  double evaluate(double * modelCoefficients, double x) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  double numberOfCoefficients() const override { return 2; }
};

}


#endif
