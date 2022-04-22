#include "test_graph_view.h"

namespace Inference {

void TestGraphView::reload() {
  layoutSubviews();
  m_curveView.reload();
}

void TestGraphView::layoutSubviews(bool force) {
  int availableHeight = m_frame.height();
  int availableWidth = m_frame.width();
  m_curveView.setFrame(KDRect(KDPointZero, KDSize(availableWidth, availableHeight)), force);
  KDSize legendSize = m_legend.minimalSizeForOptimalDisplay();
  KDPoint legendOrigin = KDPoint(availableWidth - legendSize.width() - k_legendMarginRight, k_legendMarginTop);
  m_legend.setFrame(KDRect(legendOrigin, legendSize), force);
}

Escher::View * TestGraphView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  Escher::View * subviews[] = {&m_curveView, &m_legend};
  return subviews[i];
}

}  // namespace Inference

