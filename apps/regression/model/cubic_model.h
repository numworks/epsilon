#ifndef REGRESSION_CUBIC_MODEL_H
#define REGRESSION_CUBIC_MODEL_H

#include "model.h"
#include <poincare/expression_layout.h>

namespace Regression {

class CubicModel : public Model {
public:
  using Model::Model;
  Poincare::ExpressionLayout * layout() override;
  double evaluate(double * modelCoefficients, double x) const override;
  double levelSet(double * modelCoefficients, double y) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  double numberOfCoefficients() const override { return 4; }
};

}


#endif
