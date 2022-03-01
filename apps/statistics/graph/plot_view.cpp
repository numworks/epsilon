#include "plot_view.h"
#include <assert.h>

namespace Statistics {

Escher::View * PlotView::subviewAtIndex(int index) {
  if (index == 0) {
    return plotCurveView();
  }
  assert(index == 1);
  return &m_bannerView;
}

void PlotView::reload() {
  float yMin, yMax;
  computeYBounds(&yMin, &yMax);
  m_graphRange.calibrate(plotCurveView()->bounds().width(), plotCurveView()->bounds().height(), m_store->minValueForAllSeries(), m_store->maxValueForAllSeries(), yMin, yMax);
  layoutSubviews();
  plotCurveView()->reload();
}

void PlotView::layoutDataSubviews(bool force) {
  KDCoordinate bannerHeight = bannerView()->minimalSizeForOptimalDisplay().height();
  KDCoordinate subviewHeight = (bounds().height() - bannerHeight) + 1; // +1 to make sure that all pixel rows are drawn
  KDRect frame = KDRect(0, 0, bounds().width(), subviewHeight);
  plotCurveView()->setFrame(frame, force);
}

}
