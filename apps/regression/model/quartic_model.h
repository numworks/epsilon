#ifndef REGRESSION_QUARTIC_MODEL_H
#define REGRESSION_QUARTIC_MODEL_H

#include "model.h"
#include <poincare/expression_layout.h>

namespace Regression {

class QuarticModel : public Model {
public:
  using Model::Model;
  Poincare::ExpressionLayout * layout() override;
  Poincare::Expression * simplifiedExpression(double * modelCoefficients, Poincare::Context * context) override;
  I18n::Message formulaMessage() const override { return I18n::Message::QuarticRegressionFormula; }
  double evaluate(double * modelCoefficients, double x) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 5; }
  int bannerLinesCount() const override { return 4; }
};

}


#endif
