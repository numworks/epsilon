#include "proportional_model.h"
#include "../store.h"
#include <poincare/print.h>
#include <assert.h>

using namespace Poincare;

namespace Regression {

int ProportionalModel::buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const {
  return Poincare::Print::SafeCustomPrintf(buffer, bufferSize, "%*.*ed·x",
      modelCoefficients[0], displayMode, significantDigits);
}

double ProportionalModel::evaluate(double * modelCoefficients, double x) const {
  return modelCoefficients[0] * x;
}

double ProportionalModel::levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) {
  const double a = modelCoefficients[0];
  if (a == 0.0) {
    return NAN;
  }
  return y/a;
}

double ProportionalModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  assert(derivateCoefficientIndex == 0);
  // Derivate: x
  return x;
}

}
