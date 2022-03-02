#include "normal_probability_view.h"
#include <poincare/normal_distribution.h>
#include <algorithm>
#include <assert.h>

namespace Statistics {

NormalProbabilityView::NormalProbabilityView(Store * store) :
    PlotView(store),
    // No bannerView given to the curve view because the display is handled here
    m_curveView(&m_graphRange, &m_cursor, &m_cursorView, store) {
}

void NormalProbabilityView::computeYBounds(float * yMin, float *yMax) {
  int maxNumberOfPairs = 0;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    maxNumberOfPairs = std::max(maxNumberOfPairs, m_store->numberOfPairsOfSeries(i));
  }
  // Normal probability curve is bounded by the biggest series
  /* TODO : Factorize with zScoreAtSortedIndex or
   * NormalProbabilityCurveView::resultAtIndex */
  float yBound = Poincare::NormalDistribution::CumulativeDistributiveInverseForProbability<float>(0.5f/maxNumberOfPairs, 0.0f, 1.0f);
  assert(yBound <= 0.0f);
  *yMin = yBound;
  *yMax = -yBound;
}

void NormalProbabilityView::computeXBounds(float * xMin, float *xMax) {
  *xMin = m_store->minValueForAllSeries(false);
  *xMax = m_store->maxValueForAllSeries(false);
}

}
