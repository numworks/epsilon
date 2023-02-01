#ifndef REGRESSION_EXPONENTIAL_MODEL_H
#define REGRESSION_EXPONENTIAL_MODEL_H

#include "model.h"

namespace Regression {

class ExponentialModel : public Model {
public:
  ExponentialModel(bool isAbxForm = false) : m_isAbxForm(isAbxForm) {}

  I18n::Message formulaMessage() const override { return m_isAbxForm ? I18n::Message::ExponentialAbxRegressionFormula : I18n::Message::ExponentialAebxRegressionFormula; }
  I18n::Message name() const override { return I18n::Message::ExponentialRegression; }
  int numberOfCoefficients() const override { return 2; }

  Poincare::Layout templateLayout() const override;
  Poincare::Expression expression(double * modelCoefficients) const override;

  double evaluate(double * modelCoefficients, double x) const override;
  double levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) override;

private:
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  void privateFit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) override;
  bool dataSuitableForFit(Store * store, int series) const override;
  double aebxFormatBValue(double * modelCoefficients) const;
  /* In a*b^x form, modelCoefficients[1] contains the b, and is transformed via
   * log to the b of the normal a*exp(b*x) form */
  bool m_isAbxForm;
};

}


#endif
