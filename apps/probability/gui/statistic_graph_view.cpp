#include "statistic_graph_view.h"

#include "probability/app.h"

namespace Probability {

StatisticGraphView::StatisticGraphView(Statistic * statistic,
                                       StatisticViewRange * range,
                                       LegendPositionDataSource * legendPositionDataSource) :
    m_separatorView(Palette::WallScreen),
    m_curveView(range),
    m_legendPositionDataSource(legendPositionDataSource),
    m_testConclusionView(statistic) {
}

KDSize StatisticGraphView::minimalSizeForOptimalDisplay() const {
  return KDSize(10000, 10000);  // TODO compute pixel perfect
}

void StatisticGraphView::setStatistic(Statistic * statistic) {
  m_curveView.setStatistic(statistic);
}

void StatisticGraphView::reload() {
  m_testConclusionView.reload();
  layoutSubviews();
  m_curveView.reload();
}

int StatisticGraphView::numberOfSubviews() const {
  return 4 - (App::app()->subapp() == Data::SubApp::Intervals);
}

void StatisticGraphView::layoutSubviews(bool force) {
  int availableHeight = m_frame.height();
  int availableWidth = m_frame.width();
  int curveViewHeight = availableHeight - k_conclusionViewHeight;
  // Layout curve views and sep according to mode
  m_curveView.setFrame(KDRect(KDPointZero, KDSize(availableWidth, curveViewHeight)), force);
  m_separatorView.setFrame(KDRectZero, force);

  conclusionView()->setFrame(
      KDRect(KDPoint(0, curveViewHeight), KDSize(availableWidth, k_conclusionViewHeight)),
      true);
  KDSize k_legendSize = m_legend.minimalSizeForOptimalDisplay();
  KDPoint legendOrigin = m_legendPositionDataSource->shouldPositionLegendLeft()
                             ? KDPoint(k_legendMarginRight, k_legendMarginTop)
                             : KDPoint(availableWidth - k_legendSize.width() - k_legendMarginRight,
                                       k_legendMarginTop);
  m_legend.setFrame(KDRect(legendOrigin, k_legendSize), force);
}

Escher::View * StatisticGraphView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  Escher::View * subviews[] = {&m_curveView, &m_separatorView, conclusionView(), &m_legend};
  return subviews[i];
}

Escher::View * StatisticGraphView::conclusionView() {
  if (App::app()->subapp() == Data::SubApp::Tests) {
    return &m_testConclusionView;
  }
  return &m_intervalConclusionView;
}

}  // namespace Probability
