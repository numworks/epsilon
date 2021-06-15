#ifndef APPS_PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H
#define APPS_PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H

#include <escher/solid_color_view.h>
#include <escher/view.h>

#include "interval_conclusion_view.h"
#include "legend_view.h"
#include "probability/models/statistic_view_range.h"
#include "statistic_curve_view.h"
#include "test_conclusion_view.h"

namespace Probability {

/* This is the common view for Significance Tests and Confidence Intervals.
 * It displays either 1 or 2 CurveViews separated by a Separator, a legend, and a ConclusionView.
 */
class GraphView : public Escher::View {
public:
  GraphView(StatisticViewRange * rangeLeft, StatisticViewRange * rangeRight);
  void setMode(GraphDisplayMode m);
  void setType(TestConclusionView::Type t) { m_testConclusionView.setType(t); }
  KDSize minimalSizeForOptimalDisplay() const override;
  void setStatistic(Statistic * statistic) {
    m_curveViewLeft.setStatistic(statistic);
    m_curveViewRight.setStatistic(statistic);
  }
  void reload() {
    m_curveViewLeft.reload();
    m_curveViewRight.reload();
  }
  IntervalConclusionView * intervalConclusionView() { return &m_intervalConclusionView; }

  StatisticCurveView * curveViewLeft() { return &m_curveViewLeft; }
  StatisticCurveView * curveViewRight() { return &m_curveViewRight; }

protected:
  int numberOfSubviews() const override { return 5; }  // TODO could change according to mode
  void layoutSubviews(bool force = false) override;
  Escher::View * subviewAtIndex(int i) override;

private:
  Escher::View * conclusionView();

  constexpr static int k_conclusionViewHeight = 40;
  constexpr static int k_separatorWidth = 14;
  constexpr static int k_legendMarginRight = 10;
  constexpr static int k_legendMarginTop = 20;
  Escher::SolidColorView m_separatorView;
  StatisticCurveView m_curveViewLeft;
  StatisticCurveView m_curveViewRight;
  LegendView m_legend;
  TestConclusionView m_testConclusionView;
  IntervalConclusionView m_intervalConclusionView;
  GraphDisplayMode m_mode;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H */
