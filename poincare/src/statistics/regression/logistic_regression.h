#ifndef POINCARE_REGRESSION_LOGISTIC_REGRESSION_H
#define POINCARE_REGRESSION_LOGISTIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Internal {

class LogisticRegression : public Regression {
 public:
  constexpr LogisticRegression(bool internal) : m_isInternal(internal){};
  Type type() const override {
    return m_isInternal ? Type::LogisticInternal : Type::Logistic;
  }

  double levelSet(const double* modelCoefficients, double xMin, double xMax,
                  double y, Poincare::Context* context) const override;

 private:
  Coefficients privateFit(const Series* series,
                          Poincare::Context* context) const override;
  double privateEvaluate(const Coefficients& modelCoefficients,
                         double x) const override;
  Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  double partialDerivate(const Coefficients& modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
  Coefficients specializedInitCoefficientsForFit(
      double defaultValue, size_t /* attemptNumber */,
      const Series* s = nullptr) const override;

  const bool m_isInternal;
};

}  // namespace Poincare::Internal

#endif
