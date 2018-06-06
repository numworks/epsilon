#ifndef REGRESSION_LOGARITHMIC_MODEL_H
#define REGRESSION_LOGARITHMIC_MODEL_H

#include "model.h"

namespace Regression {

class LogarithmicModel : public Model {
public:
  using Model::Model;
  double evaluate(double * modelCoefficients, double x) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  double numberOfCoefficients() const override { return 2; }
};

}


#endif
