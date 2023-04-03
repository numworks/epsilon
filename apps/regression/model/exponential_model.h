#ifndef REGRESSION_EXPONENTIAL_MODEL_H
#define REGRESSION_EXPONENTIAL_MODEL_H

#include "transformed_model.h"

namespace Regression {

class ExponentialModel : public TransformedModel {
 public:
  ExponentialModel(bool isAbxForm = false);
  I18n::Message formulaMessage() const override {
    return m_isAbxForm ? I18n::Message::ExponentialAbxRegressionFormula
                       : I18n::Message::ExponentialAebxRegressionFormula;
  }
  I18n::Message name() const override {
    return I18n::Message::ExponentialRegression;
  }
  Poincare::Layout templateLayout() const override;

 private:
  Poincare::Expression privateExpression(
      double* modelCoefficients) const override;
  bool applyLnOnX() const override { return false; }
  bool applyLnOnY() const override { return true; }
  bool applyLnOnB() const override { return m_isAbxForm; }

  /* In a*b^x form, modelCoefficients[1] contains the b, and is transformed via
   * log to the b' of the normal a*exp(b'*x) form */
  bool m_isAbxForm;
};

}  // namespace Regression

#endif
