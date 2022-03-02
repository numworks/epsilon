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
  int biggestSeries = 0;
  int maxTotal = 0;
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    int total = m_store->totalNormalProbabilityValues(i);
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

void NormalProbabilityView::computeXBounds(float * xMin, float *xMax) {
  *xMin = m_store->minValueForAllSeries(false);
  *xMax = m_store->maxValueForAllSeries(false);
}

}
