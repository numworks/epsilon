#ifndef POINCARE_REGRESSION_LINEAR_REGRESSION_H
#define POINCARE_REGRESSION_LINEAR_REGRESSION_H

#include "affine_regression.h"

namespace Poincare::Regression {

class LinearRegression : public AffineRegression {
 public:
  constexpr LinearRegression(bool isApbxForm = false)
      : m_isApbxForm(isApbxForm) {}

  Type type() const override {
    return m_isApbxForm ? Type::LinearApbx : Type::LinearAxpb;
  }

 private:
  Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  Coefficients privateFit(const Series* series,
                          Poincare::Context* context) const override;
  /* In a+bx form, Coefficients are swapped */
  int slopeCoefficientIndex() const override { return m_isApbxForm; }
  int yInterceptCoefficientIndex() const override { return !m_isApbxForm; }

  bool m_isApbxForm;
};

}  // namespace Poincare::Regression

#endif
