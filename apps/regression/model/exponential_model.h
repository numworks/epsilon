#ifndef REGRESSION_EXPONENTIAL_MODEL_H
#define REGRESSION_EXPONENTIAL_MODEL_H

#include "model.h"

namespace Regression {

class ExponentialModel : public Model {
public:
  ExponentialModel(bool abxForm = false);
  Poincare::Layout layout() override;
  I18n::Message formulaMessage() const override { return m_abxForm ? I18n::Message::ExponentialAbxRegressionFormula : I18n::Message::ExponentialRegressionFormula; }
  double evaluate(double * modelCoefficients, double x) const override;
  double levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 2; }
protected:
  void specializedInitCoefficientsForFit(double * modelCoefficients, double defaultValue, Store * store, int series) const override;
  /* In a*b^x form, modelCoefficients[1] contains the b, and is transformed via
   * log to the b of the normal a*exp(b*x) form */
  bool m_abxForm;
};

}


#endif
