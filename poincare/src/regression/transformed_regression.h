#ifndef POINCARE_REGRESSION_TRANSFORMED_REGRESSION_H
#define POINCARE_REGRESSION_TRANSFORMED_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

/* Such model are transformed before being fitted with a Linear Regression.
 * Transformation is the application of Ln on either None (Linear), X
 * (Logarithm), Y (Exponential) or both (Power). */
class TransformedRegression : public Regression {
 public:
  int numberOfCoefficients() const override { return 2; }
  double evaluate(double* modelCoefficients, double x) const override;
  double levelSet(double* modelCoefficients, double xMin, double xMax, double y,
                  Poincare::Context* context) const override;

 protected:
  void privateFit(const Series* series, double* modelCoefficients,
                  Poincare::Context* context) const override;
  bool dataSuitableForFit(const Series* series) const override;

  virtual bool applyLnOnX() const = 0;
  virtual bool applyLnOnY() const = 0;
  bool applyLnOnA() const { return applyLnOnY(); }
  virtual bool applyLnOnB() const { return false; }
};

}  // namespace Poincare::Regression

#endif
