#ifndef POINCARE_REGRESSION_LINEAR_REGRESSION_H
#define POINCARE_REGRESSION_LINEAR_REGRESSION_H

#include "affine_regression.h"

namespace Poincare::Regression {

class LinearRegression : public AffineRegression {
 public:
  constexpr LinearRegression(bool isApbxForm = false)
      : m_isApbxForm(isApbxForm) {}
  const char* formula() const override {
    return m_isApbxForm ? "y=a+b·x" : AffineRegression::formula();
  }

 private:
  Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  void privateFit(const Series* series, double* modelCoefficients,
                  Poincare::Context* context) const override;
  /* In a+bx form, Coefficients are swapped */
  int slopeCoefficientIndex() const override { return m_isApbxForm; }
  int yInterceptCoefficientIndex() const override { return !m_isApbxForm; }

  bool m_isApbxForm;
};

}  // namespace Poincare::Regression

#endif
