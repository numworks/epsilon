#ifndef REGRESSION_NONE_MODEL_H
#define REGRESSION_NONE_MODEL_H

#include "model.h"

namespace Regression {

// This model is selected by default and represents a simple scatter plot
class NoneModel : public Model {
 public:
  using Model::Model;
  I18n::Message formulaMessage() const override {
    assert(false);
    return I18n::Message::Default;
  }
  // Used in regression model menus when no regression is selected
  I18n::Message name() const override { return I18n::Message::Default; }

  Poincare::Layout templateLayout() const override {
    assert(false);
    return Poincare::Layout();
  }

  double evaluate(double* modelCoefficients, double x) const override {
    return NAN;
  }

  double levelSet(double* modelCoefficients, double xMin, double xMax, double y,
                  Poincare::Context* context) override {
    assert(false);
    return NAN;
  }
  int numberOfCoefficients() const override { return 0; }

 private:
  Poincare::Expression privateExpression(
      double* modelCoefficients) const override {
    return Poincare::Expression();
  }
  void privateFit(Store* store, int series, double* modelCoefficients,
                  Poincare::Context* context) override {}
};

}  // namespace Regression

#endif
