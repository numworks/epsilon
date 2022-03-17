#include "plot_controller.h"
#include "plot_view.h"
#include <assert.h>

namespace Statistics {

Escher::View * PlotView::subviewAtIndex(int index) {
  if (index == 0) {
    return m_plotCurveView;
  }
  assert(index == 1);
  return m_bannerView;
}

void PlotView::reload() {
  float yMin, yMax, xMin, xMax;
  m_plotControllerDelegate->computeYBounds(&yMin, &yMax);
  m_plotControllerDelegate->computeXBounds(&xMin, &xMax);
  m_graphRange->calibrate(m_plotCurveView->bounds().width(), m_plotCurveView->bounds().height(), xMin, xMax, yMin, yMax);
  layoutSubviews();
  m_plotCurveView->reload();
}

void PlotView::layoutDataSubviews(bool force) {
  KDCoordinate bannerHeight = bannerView()->minimalSizeForOptimalDisplay().height();
  KDCoordinate subviewHeight = (bounds().height() - bannerHeight) + 1; // +1 to make sure that all pixel rows are drawn
  KDRect frame = KDRect(0, 0, bounds().width(), subviewHeight);
  m_plotCurveView->setFrame(frame, force);
}

}
