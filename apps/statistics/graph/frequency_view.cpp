#include "frequency_view.h"
#include <assert.h>

namespace Statistics {

FrequencyView::FrequencyView(Store * store) :
    PlotView(store),
    // No bannerView given to the curve view because the display is handled here
    m_curveView(&m_graphRange, &m_cursor, &m_cursorView, store) {
}

void FrequencyView::computeYBounds(float * yMin, float *yMax) {
  // Frequency curve is always bounded between 0 and 100
  *yMin = 0.0f;
  *yMax = 100.0f;
}

void FrequencyView::computeXBounds(float * xMin, float *xMax) {
  *xMin = m_store->minValueForAllSeries(true);
  *xMax = m_store->maxValueForAllSeries(true);
}

}
