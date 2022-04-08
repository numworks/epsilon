#include "normal_probability_controller.h"
#include <assert.h>

namespace Statistics {

int NormalProbabilityController::totalValues(int series) const {
  double totalNormalProbabilityValues = m_store->totalNormalProbabilityValues(series);
  assert(totalNormalProbabilityValues <= INT_MAX);
  /* TODO : Set lower than INT_MAX maximum, handle it as well as non-integers
   * frequencies. */
  return static_cast<int>(totalNormalProbabilityValues);
}

void NormalProbabilityController::computeYBounds(float * yMin, float *yMax) const {
  int biggestSeries = 0;
  double maxTotal = 0.0;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    double total = m_store->totalNormalProbabilityValues(i);
    if (total > maxTotal) {
      biggestSeries = i;
      maxTotal = total;
    }
  }
  // Normal probability curve is bounded by the biggest series
  *yMin = m_store->normalProbabilityResultAtIndex(biggestSeries, 0);
  assert(*yMin <= 0.0);
  // The other bound is the opposite with Normal probability curve.
  *yMax = -*yMin;
}

void NormalProbabilityController::computeXBounds(float * xMin, float *xMax) const {
  *xMin = m_store->minValueForAllSeries(false);
  *xMax = m_store->maxValueForAllSeries(false);
}

bool NormalProbabilityController::drawSeriesZScoreLine(int series, float * x, float * y, float * u, float * v) const {
  // Plot the y=(x-mean(X)/sigma(X)) line
  float mean = m_store->mean(series);
  float sigma = m_store->standardDeviation(series);
  float xMin = m_store->minValue(series, false);
  float xMax = m_store->maxValue(series, false);
  *x = xMin;
  *y = (xMin-mean)/sigma;
  *u = xMax;
  *v = (xMax-mean)/sigma;
  return true;
}

}  // namespace Statistics
