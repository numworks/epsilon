#include "logarithmic_model.h"
#include "../store.h"
#include <poincare/print.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Regression {

int LogarithmicModel::buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const {
  return Poincare::Print::SafeCustomPrintf(buffer, bufferSize, "%*.*ed%+*.*ed·ln(x)",
      modelCoefficients[0], displayMode, significantDigits,
      modelCoefficients[1], displayMode, significantDigits);
}

double LogarithmicModel::evaluate(double * modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  return a+b*std::log(x);
}

double LogarithmicModel::levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (b == 0) {
    return NAN;
  }
  return std::exp((y-a)/b);
}

double LogarithmicModel::partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const {
  if (derivateCoefficientIndex == 1) {
    // Derivate with respect to b: ln(x)
    assert(x > 0);
    return std::log(x);
  }
  assert(derivateCoefficientIndex == 0);
  // Derivate with respect to a: 1
  return 1.0;
}

bool LogarithmicModel::dataSuitableForFit(Store * store, int series) const {
  if (!Model::dataSuitableForFit(store, series)) {
    return false;
  }
  int numberOfPairs = store->numberOfPairsOfSeries(series);
  for (int j = 0; j < numberOfPairs; j++) {
    if (store->get(series, 0, j) <= 0) {
      return false;
    }
  }
  return true;
}

}
