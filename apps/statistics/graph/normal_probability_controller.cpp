#include "normal_probability_controller.h"
#include <assert.h>
#include <algorithm>

namespace Statistics {

NormalProbabilityController::NormalProbabilityController(Escher::Responder * parentResponder, Escher::ButtonRowController * header, Escher::Responder * tabController, Escher::StackViewController * stackViewController, Escher::ViewController * typeViewController, Store * store) :
    PlotController(parentResponder, header, tabController, stackViewController, typeViewController, store) {
  m_curveView.setCursorView(&m_cursorView);
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

bool NormalProbabilityController::moveSelectionHorizontally(int deltaIndex) {
  assert(m_selectedSeries >= 0);
  int nextIndex = SanitizeIndex(m_selectedIndex + deltaIndex, totalValues(m_selectedSeries));
  if (nextIndex != m_selectedIndex) {
    m_selectedIndex = nextIndex;
    moveCursorToSelectedIndex();
    return true;
  }
  return false;
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

}  // namespace Statistics
