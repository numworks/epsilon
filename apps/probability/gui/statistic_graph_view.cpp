#include "statistic_graph_view.h"

#include "probability/app.h"

namespace Probability {

StatisticGraphView::StatisticGraphView(StatisticCurveView * curveView, View * conclusionView, LegendView * legendView) :
    m_curveView(curveView), m_conclusionView(conclusionView), m_legend(legendView) {
}

void StatisticGraphView::drawRect(KDContext * ctx, KDRect rect) const {
  /* We draw a background wall screen to avoid noisy screen when switching on
   * and off while the results are computed. */
  ctx->fillRect(KDRect(0, m_frame.height() - k_conclusionViewHeight, m_frame.width(), k_conclusionViewHeight), Escher::Palette::WallScreen);
}

void StatisticGraphView::reload() {
  layoutSubviews();
  m_curveView->reload();
}

int StatisticGraphView::numberOfSubviews() const {
  return 2 + (m_legend != nullptr);
}

void StatisticGraphView::layoutSubviews(bool force) {
  int availableHeight = m_frame.height();
  int availableWidth = m_frame.width();
  int curveViewHeight = availableHeight - k_conclusionViewHeight;
  // Layout curve views and sep according to mode
  m_curveView->setFrame(KDRect(KDPointZero, KDSize(availableWidth, curveViewHeight)), force);
  m_conclusionView->setFrame(
      KDRect(KDPoint(0, curveViewHeight), KDSize(availableWidth, k_conclusionViewHeight)),
      force);
  if (m_legend) {
    KDSize legendSize = m_legend->minimalSizeForOptimalDisplay();
    KDPoint legendOrigin = KDPoint(availableWidth - legendSize.width() - k_legendMarginRight, k_legendMarginTop);
    m_legend->setFrame(KDRect(legendOrigin, legendSize), force);
  }
}

Escher::View * StatisticGraphView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  Escher::View * subviews[] = {m_curveView, m_conclusionView, m_legend};
  return subviews[i];
}

}  // namespace Probability
