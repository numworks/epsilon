#ifndef REGRESSION_QUADRATIC_MODEL_H
#define REGRESSION_QUADRATIC_MODEL_H

#include "model.h"
#include <poincare/expression_layout.h>

namespace Regression {

class QuadraticModel : public Model {
public:
  QuadraticModel() : m_expression(nullptr) {}
  Poincare::ExpressionLayout * layout() override;
  Poincare::Expression * expression(double * modelCoefficients) override;
  I18n::Message formulaMessage() const override { return I18n::Message::QuadraticRegressionFormula; }
  double evaluate(double * modelCoefficients, double x) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 3; }
private:
  Poincare::Expression * m_expression;};

}


#endif
