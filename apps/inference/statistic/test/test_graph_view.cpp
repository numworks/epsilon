#include "test_graph_view.h"

namespace Inference {

void TestGraphView::reload() {
  layoutSubviews();
  m_curveView.reload();
}

void TestGraphView::layoutSubviews(bool force) {
  int availableHeight = m_frame.height();
  int availableWidth = m_frame.width();
  m_curveView.setFrame(KDRect(KDPointZero, KDSize(availableWidth, availableHeight - (m_displayHint ? k_zoomHintHeight : 0))), force);
  KDSize legendSize = m_legend.minimalSizeForOptimalDisplay();
  KDPoint legendOrigin = KDPoint(availableWidth - legendSize.width() - k_legendMarginRight, k_legendMarginTop);
  m_legend.setFrame(KDRect(legendOrigin, legendSize), force);
  m_zoom_hint.setFrame(m_displayHint ? KDRect(0, availableHeight-k_zoomHintHeight, availableWidth, k_zoomHintHeight) : KDRectZero, force);
}

Escher::View * TestGraphView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  Escher::View * subviews[] = {&m_curveView, &m_legend, &m_zoom_hint};
  return subviews[i];
}

}  // namespace Inference

