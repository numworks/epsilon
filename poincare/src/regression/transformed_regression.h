#ifndef POINCARE_REGRESSION_TRANSFORMED_REGRESSION_H
#define POINCARE_REGRESSION_TRANSFORMED_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

/* Such model are transformed before being fitted with a Linear Regression.
 * Transformation is the application of Ln on either None (Linear), X
 * (Logarithm), Y (Exponential) or both (Power). */
class TransformedRegression : public Regression {
 public:
  double evaluate(const double* modelCoefficients, double x) const override;
  double levelSet(const double* modelCoefficients, double xMin, double xMax,
                  double y, Poincare::Context* context) const override;

 protected:
  void privateFit(const Series* series, double* modelCoefficients,
                  Poincare::Context* context) const override;
  bool dataSuitableForFit(const Series* series) const override;

  bool applyLnOnX() const { return FitsLnX(type()); };
  bool applyLnOnY() const { return FitsLnY(type()); }
  bool applyLnOnA() const { return applyLnOnY(); }
  bool applyLnOnB() const { return FitsLnB(type()); }
};

}  // namespace Poincare::Regression

#endif
