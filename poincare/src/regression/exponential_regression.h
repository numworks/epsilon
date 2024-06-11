#ifndef POINCARE_REGRESSION_EXPONENTIAL_REGRESSION_H
#define POINCARE_REGRESSION_EXPONENTIAL_REGRESSION_H

#include "transformed_regression.h"

namespace Poincare::Regression {

class ExponentialRegression : public TransformedRegression {
 public:
  constexpr ExponentialRegression(bool isAbxForm = false)
      : m_isAbxForm(isAbxForm) {}
  const char* formula() const override {
    return m_isAbxForm ? "y=a·b^x" : "y=a·exp(b·x)";
  }
  Poincare::Layout templateLayout() const override;

 private:
  Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  bool applyLnOnX() const override { return false; }
  bool applyLnOnY() const override { return true; }
  bool applyLnOnB() const override { return m_isAbxForm; }

  /* In a*b^x form, modelCoefficients[1] contains the b, and is transformed via
   * log to the b' of the normal a*exp(b'*x) form */
  bool m_isAbxForm;
};

}  // namespace Poincare::Regression

#endif
