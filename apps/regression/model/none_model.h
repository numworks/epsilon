#ifndef REGRESSION_NONE_MODEL_H
#define REGRESSION_NONE_MODEL_H

#include "model.h"
#include <poincare/undefined.h>

namespace Regression {

// This model is selected by default and represents a simple scatter plot
class NoneModel : public Model {
public:
  using Model::Model;
  I18n::Message formulaMessage() const override { assert(false); return I18n::Message::Default; }
  // Used in regression model menus when no regression is selected
  I18n::Message name() const override { return I18n::Message::Default; }

  Poincare::Layout layout() override { assert(false); return Poincare::Layout(); }
  Poincare::Expression expression(double * modelCoefficients) const override { return Poincare::Undefined::Builder(); }

  double evaluate(double * modelCoefficients, double x) const override { return NAN; }

  double levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) override { assert(false); return NAN; }
  int numberOfCoefficients() const override { return 0; }

private:
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override { assert(false); return NAN; }
  void privateFit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) override {}
};

}

#endif
