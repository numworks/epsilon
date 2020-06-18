#include "store.h"
#include "linear_model_helper.h"
#include <poincare/preferences.h>
#include <assert.h>
#include <float.h>
#include <cmath>
#include <string.h>
#include <algorithm>

using namespace Shared;

namespace Regression {

static_assert(Model::k_numberOfModels == 10, "Number of models changed, Regression::Store() needs to adapt");
static_assert(Store::k_numberOfSeries == 3, "Number of series changed, Regression::Store() needs to adapt (m_seriesChecksum)");

Store::Store() :
  InteractiveCurveViewRange(),
  DoublePairStore(),
  m_angleUnit(Poincare::Preferences::AngleUnit::Degree)
{
  resetMemoization();
}

void Store::reset() {
  deleteAllPairs();
  resetMemoization();
}

void Store::tidy() {
  for (int i = 0; i < Model::k_numberOfModels; i++) {
    regressionModel(i)->tidy();
  }
}

/* Regressions */
void Store::setSeriesRegressionType(int series, Model::Type type) {
  assert(series >= 0 && series < k_numberOfSeries);
  if (m_regressionTypes[series] != type) {
    m_regressionTypes[series] = type;
    m_regressionChanged[series] = true;
  }
}

/* Dots */

int Store::closestVerticalDot(int direction, double x, double y, int currentSeries, int currentDot, int * nextSeries, Poincare::Context * globalContext) {
  double nextX = INFINITY;
  double nextY = INFINITY;
  int selectedDot = -1;
  for (int series = 0; series < k_numberOfSeries; series++) {
    if (seriesIsEmpty(series) || (currentDot >= 0 && currentSeries == series)) {
      /* If the currentDot is valid, the next series should not be the current
       * series */
      continue;
    }
    int numberOfPoints = numberOfPairsOfSeries(series);
    for (int i = 0; i <= numberOfPoints; i++) {
      double currentX = i < numberOfPoints ? m_data[series][0][i] : meanOfColumn(series, 0);
      double currentY = i < numberOfPoints ? m_data[series][1][i] : meanOfColumn(series, 1);
      if (xMin() <= currentX && currentX <= xMax() // The next dot is within the window abscissa bounds
          && (std::fabs(currentX - x) <= std::fabs(nextX - x)) // The next dot is the closest to x in abscissa
          && ((currentY > y && direction > 0) // The next dot is above/under y
            || (currentY < y && direction < 0)
            || (currentY == y
              && ((currentDot < 0 && direction > 0)|| ((direction < 0) == (series > currentSeries)))))
          && (nextX != currentX // Edge case: if 2 dots have the same abscissa but different ordinates
            || ((currentY <= nextY) == (direction > 0))))
      {
        nextX = currentX;
        nextY = currentY;
        selectedDot = i;
        *nextSeries = series;
      }
    }
  }
  return selectedDot;
}

int Store::nextDot(int series, int direction, int dot) {
  float nextX = INFINITY;
  int selectedDot = -1;
  double meanX = meanOfColumn(series, 0);
  float x = meanX;
  if (dot >= 0 && dot < numberOfPairsOfSeries(series)) {
    x = get(series, 0, dot);
  }
  /* We have to scan the Store in opposite ways for the 2 directions to ensure to
   * select all dots (even with equal abscissa) */
  if (direction > 0) {
    for (int index = 0; index < numberOfPairsOfSeries(series); index++) {
      /* The conditions to test are in this order:
       * - the next dot is the closest one in abscissa to x
       * - the next dot is not the same as the selected one
       * - the next dot is at the right of the selected one */
      if (std::fabs(m_data[series][0][index] - x) < std::fabs(nextX - x) &&
          (index != dot) &&
          (m_data[series][0][index] >= x)) {
        // Handle edge case: 2 dots have same abscissa
        if (m_data[series][0][index] != x || (index > dot)) {
          nextX = m_data[series][0][index];
          selectedDot = index;
        }
      }
    }
    // Compare with the mean dot
    if (std::fabs(meanX - x) < std::fabs(nextX - x) &&
        (numberOfPairsOfSeries(series) != dot) &&
        (meanX >= x)) {
      if (meanX != x || (numberOfPairsOfSeries(series) > dot)) {
        selectedDot = numberOfPairsOfSeries(series);
      }
    }
  } else {
    // Compare with the mean dot
    if (std::fabs(meanX - x) < std::fabs(nextX - x) &&
        (numberOfPairsOfSeries(series) != dot) &&
        (meanX <= x)) {
      if ((meanX != x) || (numberOfPairsOfSeries(series) < dot)) {
        nextX = meanX;
        selectedDot = numberOfPairsOfSeries(series);
      }
    }
    for (int index = numberOfPairsOfSeries(series)-1; index >= 0; index--) {
      if (std::fabs(m_data[series][0][index] - x) < std::fabs(nextX - x) &&
          (index != dot) &&
          (m_data[series][0][index] <= x)) {
        // Handle edge case: 2 dots have same abscissa
        if (m_data[series][0][index] != x || (index < dot)) {
          nextX = m_data[series][0][index];
          selectedDot = index;
        }
      }
    }
  }
  return selectedDot;
}

/* Window */

void Store::setDefault() {
  m_yAuto = true;
  float minX = FLT_MAX;
  float maxX = -FLT_MAX;
  for (int series = 0; series < k_numberOfSeries; series++) {
    if (!seriesIsEmpty(series)) {
      minX = std::min(minX, minValueOfColumn(series, 0));
      maxX = std::max(maxX, maxValueOfColumn(series, 0));
    }
  }
  float range = maxX - minX;
  setXMin(minX - k_displayHorizontalMarginRatio*range);
  setXMax(maxX + k_displayHorizontalMarginRatio*range);
}

/* Series */

bool Store::seriesIsEmpty(int series) const {
  return numberOfPairsOfSeries(series) < 2;
}

/* Calculations */

double * Store::coefficientsForSeries(int series, Poincare::Context * globalContext) {
  assert(series >= 0 && series <= k_numberOfSeries);
  assert(!seriesIsEmpty(series));
  uint32_t storeChecksumSeries = storeChecksumForSeries(series);
  Poincare::Preferences::AngleUnit currentAngleUnit = Poincare::Preferences::sharedPreferences()->angleUnit();
  if (m_angleUnit != currentAngleUnit) {
    m_angleUnit = currentAngleUnit;
    for (int i = 0; i < k_numberOfSeries; i++) {
      if (m_regressionTypes[i] == Model::Type::Trigonometric) {
        m_regressionChanged[i] = true;
      }
    }
  }
  if (m_regressionChanged[series] || (m_seriesChecksum[series] != storeChecksumSeries)) {
    Model * seriesModel = modelForSeries(series);
    seriesModel->fit(this, series, m_regressionCoefficients[series], globalContext);
    m_regressionChanged[series] = false;
    m_seriesChecksum[series] = storeChecksumSeries;
  }
  return m_regressionCoefficients[series];
}

double Store::doubleCastedNumberOfPairsOfSeries(int series) const {
  return DoublePairStore::numberOfPairsOfSeries(series);
}

void Store::resetMemoization() {
  assert(((int)Model::Type::Linear) == 0);
  memset(m_seriesChecksum, 0, sizeof(m_seriesChecksum));
  memset(m_regressionTypes, 0, sizeof(m_regressionTypes));
  memset(m_regressionChanged, 0, sizeof(m_regressionChanged));
}

float Store::maxValueOfColumn(int series, int i) const {
  float maxColumn = -FLT_MAX;
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    maxColumn = std::max<float>(maxColumn, m_data[series][i][k]);
  }
  return maxColumn;
}

float Store::minValueOfColumn(int series, int i) const {
  float minColumn = FLT_MAX;
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    minColumn = std::min<float>(minColumn, m_data[series][i][k]);
  }
  return minColumn;
}

double Store::squaredValueSumOfColumn(int series, int i, bool lnOfSeries) const {
  double result = 0;
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    if (lnOfSeries) {
      result += log(m_data[series][i][k]) * log(m_data[series][i][k]);
    } else {
      result += m_data[series][i][k]*m_data[series][i][k];
    }
  }
  return result;
}

double Store::columnProductSum(int series, bool lnOfSeries) const {
  double result = 0;
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    if (lnOfSeries) {
      result += log(m_data[series][0][k]) * log(m_data[series][1][k]);
    } else {
      result += m_data[series][0][k] * m_data[series][1][k];
    }
  }
  return result;
}

double Store::meanOfColumn(int series, int i, bool lnOfSeries) const {
  return numberOfPairsOfSeries(series) == 0 ? 0 : sumOfColumn(series, i, lnOfSeries)/numberOfPairsOfSeries(series);
}

double Store::varianceOfColumn(int series, int i, bool lnOfSeries) const {
  double mean = meanOfColumn(series, i, lnOfSeries);
  return squaredValueSumOfColumn(series, i, lnOfSeries)/numberOfPairsOfSeries(series) - mean*mean;
}

double Store::standardDeviationOfColumn(int series, int i, bool lnOfSeries) const {
  return std::sqrt(varianceOfColumn(series, i, lnOfSeries));
}

double Store::covariance(int series, bool lnOfSeries) const {
  return columnProductSum(series, lnOfSeries)/numberOfPairsOfSeries(series) - meanOfColumn(series, 0, lnOfSeries)*meanOfColumn(series, 1, lnOfSeries);
}

double Store::slope(int series, bool lnOfSeries) const {
  return LinearModelHelper::Slope(covariance(series, lnOfSeries), varianceOfColumn(series, 0, lnOfSeries));
}

double Store::yIntercept(int series, bool lnOfSeries) const {
  return LinearModelHelper::YIntercept(meanOfColumn(series, 1, lnOfSeries), meanOfColumn(series, 0, lnOfSeries), slope(series, lnOfSeries));
}

double Store::yValueForXValue(int series, double x, Poincare::Context * globalContext) {
  Model * model = regressionModel((int)m_regressionTypes[series]);
  double * coefficients = coefficientsForSeries(series, globalContext);
  return model->evaluate(coefficients, x);
}

double Store::xValueForYValue(int series, double y, Poincare::Context * globalContext) {
  Model * model = regressionModel((int)m_regressionTypes[series]);
  double * coefficients = coefficientsForSeries(series, globalContext);
  return model->levelSet(coefficients, xMin(), xGridUnit()/10.0, xMax(), y, globalContext);
}

double Store::correlationCoefficient(int series) const {
  double sd0 = standardDeviationOfColumn(series, 0);
  double sd1 = standardDeviationOfColumn(series, 1);
  return (sd0 == 0.0 || sd1 == 0.0) ? 1.0 : covariance(series)/(sd0*sd1);
}

double Store::squaredCorrelationCoefficient(int series) const {
  double cov = covariance(series);
  double v0 = varianceOfColumn(series, 0);
  double v1 = varianceOfColumn(series, 1);
  return (v0 == 0.0 || v1 == 0.0) ? 1.0 : cov*cov/(v0*v1);
}

Model * Store::regressionModel(int index) {
  Model * models[Model::k_numberOfModels] = {&m_linearModel, &m_proportionalModel, &m_quadraticModel, &m_cubicModel, &m_quarticModel, &m_logarithmicModel, &m_exponentialModel, &m_powerModel, &m_trigonometricModel, &m_logisticModel};
  return models[index];
}

}
