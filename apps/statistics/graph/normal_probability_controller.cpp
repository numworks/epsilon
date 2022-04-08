#include "normal_probability_controller.h"
#include <assert.h>
#include <algorithm>

namespace Statistics {

int NormalProbabilityController::totalValues(int series) const {
  double totalNormalProbabilityValues = m_store->totalNormalProbabilityValues(series);
  if (totalNormalProbabilityValues > k_maxTotalValues) {
    return 0;
  }
  return static_cast<int>(totalNormalProbabilityValues);
}

void NormalProbabilityController::computeYBounds(float * yMin, float *yMax) const {
  int biggestSeries = 0;
  int maxTotal = 0;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    int total = totalValues(i);
    if (total > maxTotal) {
      biggestSeries = i;
      maxTotal = total;
    }
  }
  // Normal probability curve is bounded by the biggest series
  *yMin = maxTotal > 0 ? m_store->normalProbabilityResultAtIndex(biggestSeries, 0) : 0.0f;
  assert(*yMin <= 0.0f);
  // The other bound is the opposite with Normal probability curve.
  *yMax = -*yMin;
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
