#ifndef REGRESSION_LOGARITHMIC_MODEL_H
#define REGRESSION_LOGARITHMIC_MODEL_H

#include "transformed_model.h"

namespace Regression {

class LogarithmicModel : public TransformedModel {
 public:
  LogarithmicModel();
  I18n::Message formulaMessage() const override {
    return I18n::Message::LogarithmicRegressionFormula;
  }
  I18n::Message name() const override { return I18n::Message::Logarithmic; }

 private:
  Poincare::Expression privateExpression(
      double* modelCoefficients) const override;
  bool applyLnOnX() const override { return true; }
  bool applyLnOnY() const override { return false; }
};

}  // namespace Regression

#endif
