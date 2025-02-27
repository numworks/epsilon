#include "transformed_regression.h"

#include <assert.h>
#include <poincare/src/statistics/dataset_adapter.h>

#include <cmath>

namespace Poincare::Internal {

double TransformedRegression::privateEvaluate(
    const Coefficients& modelCoefficients, double x) const {
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

double TransformedRegression::levelSet(const double* modelCoefficients,
                                       double xMin, double xMax, double y,
                                       Poincare::Context* context) const {
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
    transformedY = std::log(transformedY);
  }
  double transformedX = (transformedY - a) / b;
  return applyLnOnX() ? std::exp(transformedX) : transformedX;
}

Regression::Coefficients TransformedRegression::privateFit(
    const Series* series, Poincare::Context* context) const {
  bool opposeY = applyLnOnA() && series->getY(0) < 0.0;
  StatisticsCalculationOptions options(applyLnOnX(), applyLnOnY(), opposeY);
  double c0 = series->yIntercept(options);
  double c1 = series->slope(options);
  if (applyLnOnA()) {
    c0 = (opposeY ? -1.0 : 1.0) * std::exp(c0);
  }
  if (applyLnOnB()) {
    c1 = std::exp(c1);
  }
  return {c0, c1};
}

bool TransformedRegression::dataSuitableForFit(const Series* series) const {
  if (!Regression::dataSuitableForFit(series)) {
    return false;
  }
  int numberOfPairs = series->numberOfPairs();
  assert(numberOfPairs > 0);
  bool firstYIsNegative = series->getY(0) < 0.0;
  for (int j = 0; j < numberOfPairs; j++) {
    double x = series->getX(j);
    double y = series->getY(j);
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

}  // namespace Poincare::Internal
