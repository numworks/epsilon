#include "statistic_graph_view.h"

#include "probability/app.h"

namespace Probability {

StatisticGraphView::StatisticGraphView(StatisticViewRange * rangeLeft,
                                       StatisticViewRange * rangeRight) :
    m_separatorView(Palette::WallScreen), m_curveViewLeft(rangeLeft), m_curveViewRight(rangeRight) {
}

KDSize StatisticGraphView::minimalSizeForOptimalDisplay() const {
  return KDSize(10000, 10000);  // TODO compute pixel perfect
}

void StatisticGraphView::setStatistic(Statistic * statistic) {
  m_curveViewLeft.setStatistic(statistic);
  m_curveViewRight.setStatistic(statistic);
}

void StatisticGraphView::reload() {
  layoutSubviews();
  m_curveViewLeft.reload();
  m_curveViewRight.reload();
}

int StatisticGraphView::numberOfSubviews() const {
  return 5 - (App::app()->subapp() == Data::SubApp::Intervals);
}

void StatisticGraphView::layoutSubviews(bool force) {
  int availableHeight = m_frame.height();
  int availableWidth = m_frame.width();
  int curveViewHeight = availableHeight - k_conclusionViewHeight;
  // Layout curve views and sep according to mode
  if (m_mode == GraphDisplayMode::OneCurve) {
    m_curveViewLeft.setFrame(KDRect(KDPointZero, KDSize(availableWidth, curveViewHeight)), force);
    m_separatorView.setFrame(KDRectZero, force);
    m_curveViewRight.setFrame(KDRectZero, force);
  } else {
    int width = (availableWidth - k_separatorWidth) / 2;
    KDSize size(width, curveViewHeight);
    m_curveViewLeft.setFrame(KDRect(KDPointZero, size), force);
    m_separatorView.setFrame(KDRect(KDPoint(width, 0), KDSize(k_separatorWidth, curveViewHeight)),
                             force);
    m_curveViewRight.setFrame(KDRect(KDPoint(width + k_separatorWidth, 0), size), force);
  }
  conclusionView()->setFrame(
      KDRect(KDPoint(0, curveViewHeight), KDSize(availableWidth, k_conclusionViewHeight)),
      true);
  KDSize k_legendSize = m_legend.minimalSizeForOptimalDisplay();
  KDPoint legendOrigin = m_legendPosition == LegendPosition::Left
                             ? KDPoint(k_legendMarginRight, k_legendMarginTop)
                             : KDPoint(availableWidth - k_legendSize.width() - k_legendMarginRight,
                                       k_legendMarginTop);
  m_legend.setFrame(KDRect(legendOrigin, k_legendSize), force);
}

Escher::View * StatisticGraphView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  Escher::View * subviews[] = {&m_curveViewLeft,
                               &m_curveViewRight,
                               &m_separatorView,
                               conclusionView(),
                               &m_legend};
  return subviews[i];
}

Escher::View * StatisticGraphView::conclusionView() {
  if (App::app()->subapp() == Data::SubApp::Tests) {
    return &m_testConclusionView;
  }
  return &m_intervalConclusionView;
}

}  // namespace Probability
