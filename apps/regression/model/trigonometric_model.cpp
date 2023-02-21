#include "trigonometric_model.h"

#include <apps/regression/store.h>
#include <assert.h>
#include <poincare/multiplication.h>
#include <poincare/number.h>
#include <poincare/power.h>
#include <poincare/preferences.h>
#include <poincare/print.h>
#include <poincare/sine.h>
#include <poincare/symbol.h>

#include <cmath>

#include "../../shared/poincare_helpers.h"

using namespace Poincare;
using namespace Shared;

namespace Regression {

static double toRadians() {
  return M_PI / Trigonometry::PiInAngleUnit(
                    Poincare::Preferences::sharedPreferences->angleUnit());
}

Expression TrigonometricModel::privateExpression(
    double* modelCoefficients) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  // a*sin(bx+c)+d
  return AdditionOrSubtractionBuilder(
      Multiplication::Builder(
          Number::DecimalNumber(a),
          Sine::Builder(AdditionOrSubtractionBuilder(
              Multiplication::Builder(Number::DecimalNumber(b),
                                      Symbol::Builder(k_xSymbol)),
              Number::DecimalNumber(std::fabs(c)), c >= 0.0))),
      Number::DecimalNumber(std::fabs(d)), d >= 0.0);
}

double TrigonometricModel::evaluate(double* modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  double radian = toRadians();
  // sin() is here defined for radians, so b*x+c are converted in radians.
  return a * std::sin(radian * (b * x + c)) + d;
}

double TrigonometricModel::partialDerivate(double* modelCoefficients,
                                           int derivateCoefficientIndex,
                                           double x) const {
  if (derivateCoefficientIndex == 3) {
    // Derivate with respect to d: 1
    return 1.0;
  }

  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double radian = toRadians();
  /* sin() and cos() are here defined for radians, so b*x+c are converted in
   * radians. The added coefficient also appear in derivatives. */
  if (derivateCoefficientIndex == 0) {
    // Derivate with respect to a: sin(b*x+c)
    return std::sin(radian * (b * x + c));
  }
  if (derivateCoefficientIndex == 1) {
    // Derivate with respect to b: x*a*cos(b*x+c);
    return radian * x * a * std::cos(radian * (b * x + c));
  }
  assert(derivateCoefficientIndex == 2);
  // Derivate with respect to c: a*cos(b*x+c)
  return radian * a * std::cos(radian * (b * x + c));
}

// If (x2, y2) was an extremum, update xExtremum and yExtremum
bool checkExtremum(double x2, double y2, int lastExtremum, double* xExtremum,
                   double* yExtremum) {
  assert(lastExtremum >= 2);
  if (lastExtremum != 2) {
    return false;
  }
  *xExtremum = x2;
  *yExtremum = y2;
  return true;
}

// If lower < higher, update xMinMax and yMinMax with x and y
void updateMinMax(double lower, double higher, double x, double y,
                  double* xMinMax, double* yMinMax) {
  if (lower < higher) {
    *xMinMax = x;
    *yMinMax = y;
  }
}

/* This methods finds two consecutive extrema. The conditions are:
 * - minimum extrema: x(i) having y(i-2) > y(i-1) > y(i) < y(i+1) < y(i+2)
 * - maximum extrema: x(i) having y(i-2) < y(i-1) < y(i) > y(i+1) > y(i+2)
 * If one isn't found, return series min and max */
void findExtrema(double* xMinExtremum, double* xMaxExtremum,
                 double* yMinExtremum, double* yMaxExtremum, Store* store,
                 int series) {
  int numberOfPairs = store->numberOfPairsOfSeries(series);
  assert(numberOfPairs >= 3);
  // Compute values at index 0 and 1
  double x2 = store->get(series, 0, 0);
  double y2 = store->get(series, 1, 0);
  double x1 = store->get(series, 0, 1);
  double y1 = store->get(series, 1, 1);
  // Compute max and min in case an extremum isn't identified
  double xMin, yMin, xMax, yMax, x0, y0;
  yMin = yMax = y2;
  xMin = xMax = x2;
  updateMinMax(y1, yMin, x1, y1, &xMin, &yMin);
  updateMinMax(yMax, y1, x1, y1, &xMax, &yMax);
  // Initialize last extrema so that the first two pairs cannot be one
  int lastMinExtremum, lastMaxExtremum;
  lastMinExtremum = lastMaxExtremum = 3;
  bool foundMin, foundMax;
  foundMin = foundMax = false;
  for (int i = 2; i < numberOfPairs; i++) {
    x0 = store->get(series, 0, i);
    y0 = store->get(series, 1, i);
    if (y2 < y1 && y1 < y0) {
      // Check if (x2, y2) was a minimum extrema (y4 > y3 > y2 < y1 < y0)
      foundMin = foundMin || checkExtremum(x2, y2, lastMinExtremum,
                                           xMinExtremum, yMinExtremum);
      // (x0, y0) could be a maximum extrema, override the last candidate
      lastMaxExtremum = 0;
      // Unless we found a minimum extrema, reset foundMax to false
      foundMax = foundMax && foundMin;
    } else if (y2 > y1 && y1 > y0) {
      // Check if (x2, y2) was a maximum extrema (y4 < y3 < y2 > y1 > y0)
      foundMax = foundMax || checkExtremum(x2, y2, lastMaxExtremum,
                                           xMaxExtremum, yMaxExtremum);
      // (x0, y0) could be a minimum extrema, override the last candidate
      lastMinExtremum = 0;
      // Unless we found a maximum extrema, reset foundMin to false
      foundMin = foundMin && foundMax;
    }
    if (foundMin && foundMax) {
      return;  // Two extremum have been found
    }
    lastMinExtremum++;
    lastMaxExtremum++;
    // Shift down values
    x2 = x1;
    y2 = y1;
    x1 = x0;
    y1 = y0;
    // Update max and min
    updateMinMax(y0, yMin, x0, y0, &xMin, &yMin);
    updateMinMax(yMax, y0, x0, y0, &xMax, &yMax);
  }
  // At least one extremum is missing, fall back on series min and max
  *xMinExtremum = xMin;
  *yMinExtremum = yMin;
  *xMaxExtremum = xMax;
  *yMaxExtremum = yMax;
}

void TrigonometricModel::specializedInitCoefficientsForFit(
    double* modelCoefficients, double defaultValue, Store* store,
    int series) const {
  assert(store != nullptr && series >= 0 && series < Store::k_numberOfSeries &&
         store->seriesIsActive(series));
  /* With trigonometric model, a good fit heavily depends on good starting
   * parameters. We try to find two successive extrema, and from them deduce the
   * amplitude, period, y-delta and phase.
   * Since we look for "good" extrema (having the 2 previous/next values
   * smaller/bigger), this should be pretty resilient to outliers as long as
   * there are enough data points.
   * As a downside, data that should not be fitted as trigonometric could be
   * very off and suboptimal. */
  double xMin, xMax, yMin, yMax;
  findExtrema(&xMin, &xMax, &yMin, &yMax, store, series);
  // Init the "amplitude" coefficient a
  modelCoefficients[0] = (yMax - yMin) / 2.0;
  // Init the "period" coefficient b
  double piInAngleUnit = Trigonometry::PiInAngleUnit(
      Poincare::Preferences::sharedPreferences->angleUnit());
  double period = 2.0 * std::fabs(xMax - xMin);
  if (period > 0) {
    /* b/(2*piInAngleUnit) is the frequency of the sine.
     * With two successive extrema, we have the period, so we initialize b
     * so that b*period = 2*piInAngleUnit . This helps preventing an overfitting
     * regression with an excessive frequency. */
    modelCoefficients[1] = (2.0 * piInAngleUnit) / period;
  } else {
    /* Without period, fall back on default value, taking into account the
     * angleUnit to ensure consistent result across different angle units. */
    modelCoefficients[1] = defaultValue * piInAngleUnit;
  }
  // Init the "Phase" coefficient c
  /* Choose c so that sin(b * xMax + c) is maximal. It must depend on the angle
   * unit */
  modelCoefficients[2] = piInAngleUnit / 2 - modelCoefficients[1] * xMax;
  // Init the "y-delta" coefficient d
  modelCoefficients[k_numberOfCoefficients - 1] = (yMax + yMin) / 2.0;
}

void TrigonometricModel::uniformizeCoefficientsFromFit(
    double* modelCoefficients) const {
  // Coefficients must be unique.
  double piInAngleUnit = Trigonometry::PiInAngleUnit(
      Poincare::Preferences::sharedPreferences->angleUnit());
  // A must be positive.
  if (modelCoefficients[0] < 0.0) {
    // A * sin(B * x + C) + D = -A * sin(B * x + C + π) + D
    modelCoefficients[0] *= -1.0;
    modelCoefficients[2] += piInAngleUnit;
  }
  // B must be positive.
  if (modelCoefficients[1] < 0.0) {
    /* A * sin(B * x + C) + D = -A * sin(-B * x - C) + D
     * -A * sin(-B * x - C) + D = A * sin(-B * x - C + π) + D */
    modelCoefficients[1] *= -1.0;
    modelCoefficients[2] *= -1.0;
    modelCoefficients[2] += piInAngleUnit;
  }
  // C must be between -π (excluded) and π (included).
  if (modelCoefficients[2] <= -piInAngleUnit ||
      modelCoefficients[2] > piInAngleUnit) {
    /* A*sin(B*x + C) + D = A*sin(B*x + C - 2π) = A*sin(B*x + C + 2π)
     * Using remainder(C,2π) = C - 2π * round(C / 2π) */
    modelCoefficients[2] -=
        2.0 * piInAngleUnit *
        std::round(modelCoefficients[2] / (2.0 * piInAngleUnit));
    if (modelCoefficients[2] == -piInAngleUnit) {
      // Keep π instead of -π
      modelCoefficients[2] = piInAngleUnit;
    }
  }
}

}  // namespace Regression
