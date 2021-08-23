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

class LegendPositionDataSource {
public:
  virtual bool shouldPositionLegendLeft() = 0;
};

/* This is the common view for Significance Tests and Confidence Intervals.
 * It displays either 1 or 2 CurveViews separated by a Separator, a legend, and a ConclusionView.
 */
class StatisticGraphView : public Escher::View {  // TODO VerticalLayout ?
public:
  StatisticGraphView(Statistic * statistic, StatisticViewRange * range,
                     LegendPositionDataSource * legendPositionDataSource);
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
  Escher::SolidColorView m_separatorView;
  StatisticCurveView m_curveView;
  LegendView m_legend;
  LegendPositionDataSource * m_legendPositionDataSource;
  // TODO union of the two?
  TestConclusionView m_testConclusionView;
  IntervalConclusionView m_intervalConclusionView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H */
