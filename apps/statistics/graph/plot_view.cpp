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
  layoutSubviews();
  m_plotCurveView->reload();
}

void PlotView::selectDataView(int index) {
  m_plotCurveView->selectMainView(true);
  m_plotCurveView->reload();
}

void PlotView::deselectDataView(int index) {
  m_plotCurveView->selectMainView(false);
  m_plotCurveView->reload();
}

void PlotView::layoutDataSubviews(bool force) {
  KDCoordinate bannerHeight = bannerView()->minimalSizeForOptimalDisplay().height();
  KDCoordinate subviewHeight = (bounds().height() - bannerHeight) + 1; // +1 to make sure that all pixel rows are drawn
  KDRect frame = KDRect(0, 0, bounds().width(), subviewHeight);
  m_plotCurveView->setFrame(frame, force);
}

}
