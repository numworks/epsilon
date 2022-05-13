#ifndef REGRESSION_NONE_MODEL_H
#define REGRESSION_NONE_MODEL_H

#include "model.h"

namespace Regression {

// This model is selected by default and represents a simple scatter plot
class NoneModel : public Model {
public:
  using Model::Model;
  Poincare::Layout layout() override { assert(false); return Poincare::Layout(); }
  I18n::Message formulaMessage() const override { assert(false); return I18n::Message::Default; }
  // Used in regression model menus when no regression is selected
  I18n::Message name() const override { return I18n::Message::Default; }
  int buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const override { assert(false); return 0; }
  // Return NAN so that no regression curve is drawn
  double evaluate(double * modelCoefficients, double x) const override { return NAN; }
  double levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) override { assert(false); return NAN; }
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override { assert(false); return NAN; }
  void fit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) override {}
  int numberOfCoefficients() const override { return 0; }
};

}

#endif
