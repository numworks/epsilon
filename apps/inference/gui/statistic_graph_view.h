#ifndef PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H
#define PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H

#include <escher/view.h>

#include "legend_view.h"
#include "statistic_curve_view.h"

namespace Inference {

/* This is the common view for Significance Tests and Confidence Intervals.
 * It displays a CurveView, a legend, and a ConclusionView.
 */

class StatisticGraphView : public Escher::View {
public:
  StatisticGraphView(StatisticCurveView * curveView, View * conclusionView, LegendView * legendView = nullptr);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void reload();

protected:
  int numberOfSubviews() const override;
  void layoutSubviews(bool force = false) override;
  Escher::View * subviewAtIndex(int i) override;

private:
  constexpr static int k_conclusionViewHeight = 50;
  constexpr static int k_separatorWidth = 14;
  constexpr static int k_legendMarginRight = 10;
  constexpr static int k_legendMarginTop = 10;
  StatisticCurveView * m_curveView;
  View * m_conclusionView;
  LegendView * m_legend;
};

}  // namespace Inference

#endif /* PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H */
