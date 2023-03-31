#include "logarithmic_model.h"

#include <assert.h>
#include <poincare/multiplication.h>
#include <poincare/naperian_logarithm.h>
#include <poincare/print.h>

#include <cmath>

#include "../store.h"

using namespace Poincare;

namespace Regression {

Poincare::Expression LogarithmicModel::privateExpression(
    double* modelCoefficients) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  // a+b*ln(x)
  return AdditionOrSubtractionBuilder(
      Number::DecimalNumber(a),
      Multiplication::Builder(
          Number::DecimalNumber(std::fabs(b)),
          NaperianLogarithm::Builder(Symbol::Builder(k_xSymbol))),
      b >= 0.0);
}

double LogarithmicModel::evaluate(double* modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  return a + b * std::log(x);
}

double LogarithmicModel::levelSet(double* modelCoefficients, double xMin,
                                  double xMax, double y,
                                  Poincare::Context* context) {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  if (b == 0) {
    return NAN;
  }
  return std::exp((y - a) / b);
}

void LogarithmicModel::privateFit(Store* store, int series,
                                  double* modelCoefficients,
                                  Poincare::Context* context) {
  // Y1 = a+b*ln(X1) => Y1 = a+b*Z1 with Z1=ln(X1)
  Shared::DoublePairStore::Parameters parameters(true, false, false);
  modelCoefficients[0] = store->yIntercept(series, parameters);
  modelCoefficients[1] = store->slope(series, parameters);
}

bool LogarithmicModel::dataSuitableForFit(Store* store, int series) const {
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

}  // namespace Regression
