#pragma once

#include <poincare/statistics/regression.h>

namespace Poincare::Internal {

// This model is selected by default and represents a simple scatter plot
class NoneRegression : public Regression {
 public:
  Type type() const override { return Type::None; }

  double levelSet(const double* modelCoefficients, double xMin, double xMax,
                  double y,
                  const Poincare::SymbolContext& symbolContext) const override {
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
  Coefficients privateFit(const Series* series) const override { return {}; }
};

}  // namespace Poincare::Internal
