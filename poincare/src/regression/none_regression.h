#ifndef POINCARE_REGRESSION_NONE_REGRESSION_H
#define POINCARE_REGRESSION_NONE_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

// This model is selected by default and represents a simple scatter plot
class NoneRegression : public Regression {
 public:
  using Regression::Regression;

  Poincare::Layout templateLayout() const override {
    assert(false);
    return Poincare::Layout();
  }

  double evaluate(double* modelCoefficients, double x) const override {
    return NAN;
  }

  double levelSet(double* modelCoefficients, double xMin, double xMax, double y,
                  Poincare::Context* context) const override {
    assert(false);
    return NAN;
  }
  int numberOfCoefficients() const override { return 0; }

 private:
  Poincare::UserExpression privateExpression(
      double* modelCoefficients) const override {
    return Poincare::UserExpression();
  }
  void privateFit(const Series* series, double* modelCoefficients,
                  Poincare::Context* context) const override {}
};

}  // namespace Poincare::Regression

#endif
