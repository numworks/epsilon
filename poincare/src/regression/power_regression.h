#ifndef POINCARE_REGRESSION_POWER_REGRESSION_H
#define POINCARE_REGRESSION_POWER_REGRESSION_H

#include "transformed_model.h"

namespace Regression {

class PowerModel : public TransformedModel {
 public:
  PowerModel();
  I18n::Message formulaMessage() const override {
    return I18n::Message::PowerRegressionFormula;
  }
  I18n::Message name() const override { return I18n::Message::Power; }
  Poincare::Layout templateLayout() const override;

 private:
  Poincare::UserExpression privateExpression(
      double* modelCoefficients) const override;
  bool applyLnOnX() const override { return true; }
  bool applyLnOnY() const override { return true; }
};

}  // namespace Regression

#endif
