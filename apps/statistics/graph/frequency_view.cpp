#include "frequency_view.h"
#include <assert.h>

namespace Statistics {

FrequencyView::FrequencyView(Store * store) :
    MultipleDataView(store),
    // No bannerView given to the curve view because the display is handled here
    m_curveView(&m_graphRange, &m_cursor, &m_cursorView, store) {
}

Escher::View * FrequencyView::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_curveView;
  }
  assert(index == 1);
  return &m_bannerView;
}

void FrequencyView::reload() {
  m_graphRange.calibrate(m_curveView.bounds().width(), m_curveView.bounds().height(), m_store->minValueForAllSeries(), m_store->maxValueForAllSeries(), 0.0f, 100.0f);
  layoutSubviews();
  m_curveView.reload();
}

void FrequencyView::layoutDataSubviews(bool force) {
  KDCoordinate bannerHeight = bannerView()->minimalSizeForOptimalDisplay().height();
  KDCoordinate subviewHeight = (bounds().height() - bannerHeight) + 1; // +1 to make sure that all pixel rows are drawn
  KDRect frame = KDRect(0, 0, bounds().width(), subviewHeight);
  m_curveView.setFrame(frame, force);
}

}
