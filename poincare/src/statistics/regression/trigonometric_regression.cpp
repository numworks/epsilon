#include "trigonometric_regression.h"

#include <assert.h>
#include <omg/float.h>
#include <poincare/k_tree.h>
#include <poincare/preferences.h>
#include <poincare/statistics/dataset_adapter.h>
#include <poincare/trigonometry.h>

#include <cmath>

namespace Poincare::Internal {

double TrigonometricRegression::toRadiansCoeff() const {
  return M_PI / Poincare::Trigonometry::PiInAngleUnit(m_angleUnit);
}

UserExpression TrigonometricRegression::privateExpression(
    const double* modelCoefficients) const {
  // a*sin(bx+c)+d
  return UserExpression::Create(
      KAdd(KMult(KA, KSin(KAdd(KMult(KB, "x"_e), KC))), KD),
      {.KA = UserExpression::Builder(modelCoefficients[0]),
       .KB = UserExpression::Builder(modelCoefficients[1]),
       .KC = UserExpression::Builder(modelCoefficients[2]),
       .KD = UserExpression::Builder(modelCoefficients[3])});
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
  StatisticsDatasetFromTable dataset = series->createDatasetFromColumn(0);
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

TrigonometricRegression::DataMetrics
TrigonometricRegression::ComputeDataMetrics(const Series* series,
                                            AngleUnit angleUnit) {
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
  double amplitude = (yMax - yMin) / 2.0;
  double piInAngleUnit = Poincare::Trigonometry::PiInAngleUnit(angleUnit);
  double period = 2.0 * std::fabs(xMax - xMin);
  double frequency =
      (period > 0)
          ?
          /* b/(2*piInAngleUnit) is the frequency of the sine.
           * With two successive extrema, we have the period, so we initialize b
           * so that b*period = 2*piInAngleUnit . This helps preventing an
           * overfitting regression with an excessive frequency. */
          (2.0 * piInAngleUnit) / period
          :
          /* Without enough data points to compute a period, fall back on a
             default value of pi. */
          piInAngleUnit;
  /* Choose [phase] so that sin([frequency] * xMax + [phase]) is maximal. It
   * must depend on the angle unit */
  double phase = piInAngleUnit / 2 - frequency * xMax;
  double yDelta = (yMax + yMin) / 2.0;
  return DataMetrics{.amplitude = amplitude,
                     .frequency = frequency,
                     .phase = phase,
                     .yDelta = yDelta};
}

Regression::Coefficients
TrigonometricRegression::specializedInitCoefficientsForFit(
    double defaultValue, size_t attemptNumber, const Series* series) const {
  /* For each initialization of coefficients (indicated by attemptNumber), we
   * modify the frequency parameter that was computed from the data. Out of the
   * total number of attempts (m_initialParametersIterations), half of the
   * modified b values are higher than the initial b value, and half of the
   * modified b values are lower than the initial b value. The multiplication
   * coefficient increases or decreases geometrically with a certain geometrical
   * factor.  */
  double initialFrequency =
      m_dataMetrics.frequency *
      std::pow(k_frequencyMultiplicationFactor,
               (int)attemptNumber + 1 - (int)m_initialParametersIterations / 2);
  return {m_dataMetrics.amplitude, initialFrequency, m_dataMetrics.phase,
          m_dataMetrics.yDelta};
}

void TrigonometricRegression::uniformizeCoefficientsFromFit(
    Coefficients& modelCoefficients) const {
  // Coefficients must be unique.
  double piInAngleUnit = Poincare::Trigonometry::PiInAngleUnit(m_angleUnit);
  if (std::abs(modelCoefficients[0]) <= OMG::Float::Epsilon<double>()) {
    // If A is null, B and C must be too
    modelCoefficients[0] = 0.0;
    modelCoefficients[1] = 0.0;
    modelCoefficients[2] = 0.0;
  } else {
    /* Not expected when modelCoefficients[0] isn't null. If this happens, A
     * and C should be set to 0, and D should be set to D + A*sin(C), taking
     * piInAngleUnit into account. */
    assert(std::abs(modelCoefficients[1]) > OMG::Float::Epsilon<double>());

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
}

double TrigonometricRegression::coefficientMagnitude(
    const Regression::Coefficients& modelCoefficients1,
    const Regression::Coefficients& modelCoefficients2) const {
  /* Only coefficients A and D are taken into account. B and C are ignored so
   * that an angle unit change does not impact the best model choice */
  assert(numberOfCoefficients() == 4);
  return std::max({modelCoefficients1[0], modelCoefficients1[3],
                   modelCoefficients2[0], modelCoefficients2[3]});
}

double TrigonometricRegression::modelScore(
    double meanSquaredError,
    const Regression::Coefficients& modelCoefficients) const {
  assert(meanSquaredError >= 0.);
  assert(modelCoefficients[1] >= 0.);
  assert(m_dataMetrics.amplitude >= 0.);
  assert(m_dataMetrics.frequency > 0.);
  // To prevent division by zero, fallback to 1 if denominator is null.
  double amplitude = m_dataMetrics.amplitude > OMG::Float::Epsilon<double>()
                         ? m_dataMetrics.amplitude
                         : 1.0;
  /* The model score is a weighted combination of the mean squared error and of
   * the frequency, that should be the lowest possible for a trigonometric
   * regression. This is to avoid overfitting the data with a high frequency.
   * The residual square sum is scaled by the amplitude of the data and the
   * model frequency is scaled by the "frequency" computed from the data.
   * The weight before the frequency term is chosen arbitrarily. Because the
   * frequency estimation from the data is not so reliable, we choose a low
   * coefficient in front of the frequency term. */
  constexpr static double lambda_f = 0.1;
  return (std::sqrt(meanSquaredError) / amplitude) +
         lambda_f * (modelCoefficients[1] / m_dataMetrics.frequency);
}

}  // namespace Poincare::Internal
