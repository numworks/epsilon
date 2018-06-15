#ifndef REGRESSION_CUBIC_MODEL_H
#define REGRESSION_CUBIC_MODEL_H

#include "model.h"
#include <poincare/expression_layout.h>

namespace Regression {

class CubicModel : public Model {
public:
  CubicModel() : m_expression(nullptr) {}
  Poincare::ExpressionLayout * layout() override;
  Poincare::Expression * simplifiedExpression(double * modelCoefficients, Poincare::Context * context) override;
  I18n::Message formulaMessage() const override { return I18n::Message::CubicRegressionFormula; }
  double evaluate(double * modelCoefficients, double x) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 4; }
private:
  Poincare::Expression * m_expression;
};

}


#endif
