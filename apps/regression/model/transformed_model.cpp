#include "transformed_model.h"

#include <assert.h>

#include <cmath>

#include "../../shared/linear_regression_store.h"
#include "../store.h"

using namespace Poincare;
using namespace Shared;

namespace Regression {

double TransformedModel::evaluate(double* modelCoefficients, double x) const {
  /* TODO: Ensure this transformed evaluation remain precise. Otherwise,
   * reimplement if for each models. Same for levelSet. */
  bool opposeY = false;
  double a = modelCoefficients[0];
  if (applyLnOnA()) {
    if (a < 0.0) {
      opposeY = true;
      a = -a;
    }
    a = std::log(a);
  }
  double b = modelCoefficients[1];
  if (applyLnOnB()) {
    b = std::log(b);
  }
  if (applyLnOnX()) {
    x = std::log(x);
  }
  double transformedY = a + b * x;
  return (opposeY ? -1.0 : 1.0) *
         (applyLnOnY() ? std::exp(transformedY) : transformedY);
}

double TransformedModel::levelSet(double* modelCoefficients, double xMin,
                                  double xMax, double y,
                                  Poincare::Context* context) {
  bool opposeY = false;
  double a = modelCoefficients[0];
  if (applyLnOnA()) {
    if (a < 0.0) {
      opposeY = true;
      a = -a;
    }
    a = std::log(a);
  }
  double b = modelCoefficients[1];
  if (applyLnOnB()) {
    b = std::log(b);
  }
  double transformedY = (opposeY ? -1.0 : 1.0) * y;
  if (applyLnOnY()) {
    y = std::log(y);
  }
  double transformedX = (transformedY - a) / b;
  return applyLnOnX() ? std::exp(transformedX) : transformedX;
}

void TransformedModel::privateFit(Store* store, int series,
                                  double* modelCoefficients,
                                  Poincare::Context* context) {
  assert(store != nullptr && series >= 0 && series < Store::k_numberOfSeries &&
         store->seriesIsActive(series));
  bool opposeY = applyLnOnA() && store->get(series, 1, 0) < 0.0;
  LinearRegressionStore::CalculationOptions options(applyLnOnX(), applyLnOnY(),
                                                    opposeY);
  modelCoefficients[0] = store->yIntercept(series, options);
  modelCoefficients[1] = store->slope(series, options);
  if (applyLnOnA()) {
    modelCoefficients[0] =
        (opposeY ? -1.0 : 1.0) * std::exp(modelCoefficients[0]);
  }
  if (applyLnOnB()) {
    modelCoefficients[1] = std::exp(modelCoefficients[1]);
  }
}

bool TransformedModel::dataSuitableForFit(Store* store, int series) const {
  if (!Model::dataSuitableForFit(store, series)) {
    return false;
  }
  int numberOfPairs = store->numberOfPairsOfSeries(series);
  assert(numberOfPairs > 0);
  bool firstYIsNegative = store->get(series, 1, 0) < 0.0;
  for (int j = 0; j < numberOfPairs; j++) {
    double x = store->get(series, 0, j);
    double y = store->get(series, 1, j);
    if (applyLnOnX() && x <= 0) {
      // X data points must be strictly positive
      return false;
    }
    if (applyLnOnY() && (y == 0 || firstYIsNegative != (y < 0.0))) {
      /* Y data points of the strict same sign. By opposing both the coefficient
       * a and the Y data points, we can still apply the logarithm. */
      return false;
    }
  }
  return true;
}

}  // namespace Regression
