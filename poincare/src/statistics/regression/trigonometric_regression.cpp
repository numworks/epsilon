#include "trigonometric_regression.h"

#include <assert.h>
#include <poincare/k_tree.h>
#include <poincare/preferences.h>
#include <poincare/src/statistics/dataset_adapter.h>
#include <poincare/statistics/statistics.h>
#include <poincare/trigonometry.h>

#include <cmath>

namespace Poincare::Internal {

double TrigonometricRegression::toRadiansCoeff() const {
  return M_PI / Poincare::Trigonometry::PiInAngleUnit(m_angleUnit);
}

API::UserExpression TrigonometricRegression::privateExpression(
    const double* modelCoefficients) const {
  // a*sin(bx+c)+d
  return API::UserExpression::Create(
      KAdd(KMult(KA, KSin(KAdd(KMult(KB, "x"_e), KC))), KD),
      {.KA = API::UserExpression::FromDouble(modelCoefficients[0]),
       .KB = API::UserExpression::FromDouble(modelCoefficients[1]),
       .KC = API::UserExpression::FromDouble(modelCoefficients[2]),
       .KD = API::UserExpression::FromDouble(modelCoefficients[3])});
}

double TrigonometricRegression::privateEvaluate(
    const Coefficients& modelCoefficients, double x) const {
  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double d = modelCoefficients[3];
  double radian = toRadiansCoeff();
  // sin() is here defined for radians, so b*x+c are converted in radians.
  return a * std::sin(radian * (b * x + c)) + d;
}

double TrigonometricRegression::partialDerivate(
    const Coefficients& modelCoefficients, int derivateCoefficientIndex,
    double x) const {
  if (derivateCoefficientIndex == 3) {
    // Derivate with respect to d: 1
    return 1.0;
  }

  double a = modelCoefficients[0];
  double b = modelCoefficients[1];
  double c = modelCoefficients[2];
  double radian = toRadiansCoeff();
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
static bool checkExtremum(double x2, double y2, int lastExtremum,
                          double* xExtremum, double* yExtremum) {
  assert(lastExtremum >= 2);
  if (lastExtremum != 2) {
    return false;
  }
  *xExtremum = x2;
  *yExtremum = y2;
  return true;
}

// If lower < higher, update xMinMax and yMinMax with x and y
static void updateMinMax(double lower, double higher, double x, double y,
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
static void findExtrema(double* xMinExtremum, double* xMaxExtremum,
                        double* yMinExtremum, double* yMaxExtremum,
                        const Series* series) {
  int numberOfPairs = series->numberOfPairs();
  assert(numberOfPairs >= 3);
  // Use a StatisticsDataset to memoize the sorted index
  StatisticsDatasetFromColumn dataset = series->createDatasetFromColumn(0);
  // Compute values at index 0 and 1
  int firstIndex = dataset.indexAtSortedIndex(0);
  int secondIndex = dataset.indexAtSortedIndex(1);
  double x2 = series->getX(firstIndex);
  double y2 = series->getY(firstIndex);
  double x1 = series->getX(secondIndex);
  double y1 = series->getY(secondIndex);
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
    int sortedIndex = dataset.indexAtSortedIndex(i);
    x0 = series->getX(sortedIndex);
    y0 = series->getY(sortedIndex);
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
      break;  // Two extremum have been found
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

Regression::Coefficients
TrigonometricRegression::specializedInitCoefficientsForFit(
    double defaultValue, size_t attemptNumber, const Series* series) const {
  /* With trigonometric model, a good fit heavily depends on good starting
   * parameters. We try to find two successive extrema, and from them deduce the
   * amplitude, frequency, y-delta and phase.
   * Since we look for "good" extrema (having the 2 previous/next values
   * smaller/bigger), this should be pretty resilient to outliers as long as
   * there are enough data points.
   * As a downside, data that should not be fitted as trigonometric could be
   * very off and suboptimal. */
  double xMin, xMax, yMin, yMax;
  findExtrema(&xMin, &xMax, &yMin, &yMax, series);
  // Init the "amplitude" coefficient a
  double a = (yMax - yMin) / 2.0;
  // Init the "frequency" coefficient b
  double piInAngleUnit = Poincare::Trigonometry::PiInAngleUnit(m_angleUnit);
  double period = 2.0 * std::fabs(xMax - xMin);
  double b =
      (period > 0)
          ?
          /* b/(2*piInAngleUnit) is the frequency of the sine.
           * With two successive extrema, we have the period, so we initialize b
           * so that b*period = 2*piInAngleUnit . This helps preventing an
           * overfitting regression with an excessive frequency. */
          (2.0 * piInAngleUnit) / period
          :
          /* Without period, fall back on default value, taking into account the
           * angleUnit to ensure consistent result across different angle units.
           */
          defaultValue * piInAngleUnit;

  // Init the "Phase" coefficient c
  /* Choose c so that sin(b * xMax + c) is maximal. It must depend on the angle
   * unit */
  double c = piInAngleUnit / 2 - b * xMax;
  // Init the "y-delta" coefficient d
  double d = (yMax + yMin) / 2.0;

  /* For each initialization of coefficients (indicated by attemptNumber), we
   * modify the initial guess for the frequency parameter (b) which was computed
   * earlier. Out of the total number of attempts
   * (m_initialParametersIterations), half of the modified b values are higher
   * than the initial b value, and half of the modified b values are lower than
   * the initial b value. The multiplication coefficient increases or decreases
   * geometrically with a certain geometrical factor.  */
  b = b *
      std::pow(k_frequencyMultiplicationFactor,
               (int)attemptNumber + 1 - (int)m_initialParametersIterations / 2);

  return {a, b, c, d};
}

void TrigonometricRegression::uniformizeCoefficientsFromFit(
    Coefficients& modelCoefficients) const {
  // Coefficients must be unique.
  double piInAngleUnit = Poincare::Trigonometry::PiInAngleUnit(m_angleUnit);
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

bool TrigonometricRegression::isRegressionBetter(
    double residualsSquareSum1, double residualsSquareSum2,
    const Regression::Coefficients& modelCoefficients1,
    const Regression::Coefficients& modelCoefficients2) const {
  /* If model 1 has a significantly lower residual standard deviation than
   * model 2, it is the best of the two models. */
  constexpr double significantlyLowerFactor = 0.9;
  if (residualsSquareSum1 < significantlyLowerFactor * residualsSquareSum2) {
    return true;
  }
  constexpr double sameRangeFactor = 1.1;
  /* If model 1 has its residual standard deviation in the same range as the
   * one of model 2, but has a lower frequency parameter (the "b" parameter),
   * we prefer it to model 2. This is because we want to avoid trigonometric
   * regressions which fits the data, but with a frequency that is for
   * instance twice the "base" frequency that would also fit the data. */
  if ((residualsSquareSum1 < sameRangeFactor * residualsSquareSum2) &&
      modelCoefficients1[1] < modelCoefficients2[1]) {
    return true;
  }
  return false;
}

}  // namespace Poincare::Internal
