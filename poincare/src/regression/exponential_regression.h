#ifndef POINCARE_REGRESSION_EXPONENTIAL_REGRESSION_H
#define POINCARE_REGRESSION_EXPONENTIAL_REGRESSION_H

#include "transformed_regression.h"

namespace Poincare::Regression {

class ExponentialRegression : public TransformedRegression {
 public:
  constexpr ExponentialRegression(bool isAbxForm = false)
      : m_isAbxForm(isAbxForm) {}

  Type type() const override {
    return m_isAbxForm ? Type::ExponentialAbx : Type::ExponentialAebx;
  }

 private:
  Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const override;

  /* In a*b^x form, modelCoefficients[1] contains the b, and is transformed via
   * log to the b' of the normal a*exp(b'*x) form */
  bool m_isAbxForm;
};

}  // namespace Poincare::Regression

#endif
