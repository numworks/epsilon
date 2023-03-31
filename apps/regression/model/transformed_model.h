#ifndef REGRESSION_TRANSFORMED_MODEL_H
#define REGRESSION_TRANSFORMED_MODEL_H

#include "model.h"

namespace Regression {

/* Such model are transformed before being fitted with a Linear Regression.
 * Transformation is the application of Ln on either None (Linear), X
 * (Logarithm), Y (Exponential) or both (Power). */
class TransformedModel : public Model {
 public:
  int numberOfCoefficients() const override { return 2; }
  double evaluate(double* modelCoefficients, double x) const override;
  double levelSet(double* modelCoefficients, double xMin, double xMax, double y,
                  Poincare::Context* context) override;

 protected:
  void privateFit(Store* store, int series, double* modelCoefficients,
                  Poincare::Context* context) override;
  bool dataSuitableForFit(Store* store, int series) const override;

  virtual bool applyLnOnX() const = 0;
  virtual bool applyLnOnY() const = 0;
  bool applyLnOnA() const { return applyLnOnY(); }
  virtual bool applyLnOnB() const { return false; }
};

}  // namespace Regression

#endif
