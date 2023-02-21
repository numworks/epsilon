#include "plot_view.h"

#include <assert.h>

#include "plot_controller.h"

namespace Statistics {

Escher::View* PlotView::subviewAtIndex(int index) {
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

void PlotView::layoutDataSubviews(bool force) {
  m_plotCurveView->setFrame(bounds(), force);
}

}  // namespace Statistics
