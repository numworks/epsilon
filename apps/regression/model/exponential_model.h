#ifndef REGRESSION_EXPONENTIAL_MODEL_H
#define REGRESSION_EXPONENTIAL_MODEL_H

#include "model.h"

namespace Regression {

class ExponentialModel : public Model {
public:
  ExponentialModel(bool isAbxForm = false) : m_isAbxForm(isAbxForm) {}
  Poincare::Layout layout() override;
  I18n::Message formulaMessage() const override { return m_isAbxForm ? I18n::Message::ExponentialAbxRegressionFormula : I18n::Message::ExponentialAebxRegressionFormula; }
  int buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const override;
  I18n::Message name() const override { return I18n::Message::ExponentialRegression; }
  double evaluate(double * modelCoefficients, double x) const override;
  double levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 2; }
protected:
  bool dataSuitableForFit(Store * store, int series) const override;
  void specializedInitCoefficientsForFit(double * modelCoefficients, double defaultValue, Store * store, int series) const override;
private:
  double aebxFormatBValue(double * modelCoefficients) const;
  /* In a*b^x form, modelCoefficients[1] contains the b, and is transformed via
   * log to the b of the normal a*exp(b*x) form */
  bool m_isAbxForm;
};

}


#endif
