#ifndef REGRESSION_POWER_MODEL_H
#define REGRESSION_POWER_MODEL_H

#include "transformed_model.h"

namespace Regression {

class PowerModel : public TransformedModel {
 public:
  using TransformedModel::TransformedModel;
  I18n::Message formulaMessage() const override {
    return I18n::Message::PowerRegressionFormula;
  }
  I18n::Message name() const override { return I18n::Message::Power; }
  Poincare::Layout templateLayout() const override;

 private:
  Poincare::Expression privateExpression(
      double* modelCoefficients) const override;
  bool applyLnOnX() const override { return true; }
  bool applyLnOnY() const override { return true; }
};

}  // namespace Regression

#endif
