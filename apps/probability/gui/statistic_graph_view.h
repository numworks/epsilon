#ifndef APPS_PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H
#define APPS_PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H

#include <escher/view.h>

#include "interval_conclusion_view.h"
#include "legend_view.h"
#include "probability/models/statistic_view_range.h"
#include "statistic_curve_view.h"
#include "test_conclusion_view.h"

namespace Probability {

/* This is the common view for Significance Tests and Confidence Intervals.
 * It displays a CurveView, a legend, and a ConclusionView.
 */
class StatisticGraphView : public Escher::View {
public:
  StatisticGraphView(Statistic * statistic, StatisticViewRange * range);
  KDSize minimalSizeForOptimalDisplay() const override;
  void setStatistic(Statistic * statistic);
  void reload();
  IntervalConclusionView * intervalConclusionView() { return &m_intervalConclusionView; }

protected:
  int numberOfSubviews() const override;
  void layoutSubviews(bool force = false) override;
  Escher::View * subviewAtIndex(int i) override;

private:
  Escher::View * conclusionView();

  constexpr static int k_conclusionViewHeight = 50;
  constexpr static int k_separatorWidth = 14;
  constexpr static int k_legendMarginRight = 10;
  constexpr static int k_legendMarginTop = 10;
  StatisticCurveView m_curveView;
  LegendView m_legend;
  // TODO union of the two?
  TestConclusionView m_testConclusionView;
  IntervalConclusionView m_intervalConclusionView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H */
