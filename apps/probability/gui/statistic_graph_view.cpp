#include "statistic_graph_view.h"

#include "probability/app.h"

namespace Probability {

StatisticGraphView::StatisticGraphView(Statistic * statistic, StatisticViewRange * range) :
    m_curveView(range), m_testConclusionView(statistic) {
}

KDSize StatisticGraphView::minimalSizeForOptimalDisplay() const {
  return KDSizeZero;  // Never queried
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
  return 3 - (App::app()->subapp() == Data::SubApp::Intervals);
}

void StatisticGraphView::layoutSubviews(bool force) {
  int availableHeight = m_frame.height();
  int availableWidth = m_frame.width();
  int curveViewHeight = availableHeight - k_conclusionViewHeight;
  // Layout curve views and sep according to mode
  m_curveView.setFrame(KDRect(KDPointZero, KDSize(availableWidth, curveViewHeight)), force);
  conclusionView()->setFrame(
      KDRect(KDPoint(0, curveViewHeight), KDSize(availableWidth, k_conclusionViewHeight)),
      true);
  KDSize k_legendSize = m_legend.minimalSizeForOptimalDisplay();
  KDPoint legendOrigin = KDPoint(availableWidth - k_legendSize.width() - k_legendMarginRight,
                                 k_legendMarginTop);
  m_legend.setFrame(KDRect(legendOrigin, k_legendSize), force);
}

Escher::View * StatisticGraphView::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  Escher::View * subviews[] = {&m_curveView, conclusionView(), &m_legend};
  return subviews[i];
}

Escher::View * StatisticGraphView::conclusionView() {
  if (App::app()->subapp() == Data::SubApp::Tests) {
    return &m_testConclusionView;
  }
  return &m_intervalConclusionView;
}

}  // namespace Probability
