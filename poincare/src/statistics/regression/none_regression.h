#ifndef POINCARE_REGRESSION_NONE_REGRESSION_H
#define POINCARE_REGRESSION_NONE_REGRESSION_H

#include "regression.h"

namespace Poincare::Internal {

// This model is selected by default and represents a simple scatter plot
class NoneRegression : public Regression {
 public:
  Type type() const override { return Type::None; }

  double levelSet(const double* modelCoefficients, double xMin, double xMax,
                  double y, Poincare::Context* context) const override {
    assert(false);
    return NAN;
  }

 private:
  double privateEvaluate(const Coefficients& modelCoefficients,
                         double x) const override {
    return NAN;
  }
  Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const override {
    return UserExpression();
  }
  Coefficients privateFit(const Series* series,
                          Poincare::Context* context) const override {
    return {};
  }
};

}  // namespace Poincare::Internal

#endif
