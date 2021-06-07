#ifndef APPS_PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H
#define APPS_PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H

#include <escher/solid_color_view.h>
#include <escher/view.h>

#include "test_conclusion_view.h"

namespace Probability {

typedef Escher::SolidColorView FakeDistributionCurveView;
typedef Escher::SolidColorView Separator;
typedef Escher::SolidColorView Legend;

/* This is the common view for Significance Tests and Confidence Intervals.
 * It displays either 1 or 2 CurveViews separated by a Separator, a legend, and a ConclusionView.
 */
class GraphView : public Escher::View {
public:
  GraphView();
  void computeMode();
  void setType(TestConclusionView::Type t) { m_conclusionView.setType(t); }
  KDSize minimalSizeForOptimalDisplay() const override;

protected:
  int numberOfSubviews() const override { return 5; }  // TODO could change according to mode
  void layoutSubviews(bool force = false) override;
  Escher::View * subviewAtIndex(int i) override;

private:
  enum class Mode { OneCurveView, TwoCurveViews };
  constexpr static int k_conclusionViewHeight = 40;
  constexpr static int k_separatorWidth = 14;
  constexpr static int k_legendMarginRight = 10;
  constexpr static int k_legendWidth = 70;
  constexpr static int k_legendHeight = 40;
  FakeDistributionCurveView m_curveViewLeft;
  FakeDistributionCurveView m_curveViewRight;
  Escher::SolidColorView m_separatorView;
  Legend m_legend;
  TestConclusionView m_conclusionView;
  Mode m_mode;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H */
