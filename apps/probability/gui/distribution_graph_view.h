#ifndef APPS_PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H
#define APPS_PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H

#include <escher/solid_color_view.h>
#include <escher/view.h>

typedef Escher::SolidColorView DistributionCurveView;
typedef Escher::SolidColorView Separator;
typedef Escher::SolidColorView ConclusionView;

/* This is the common view for Significance Tests and Confidence Intervals.
 * It displays either 1 or 2 CurveViews separated by a Separator, a legend, and a ConclusionView.
 */
class GraphView : public Escher::View {
public:
  GraphView();
  void computeMode();
  KDSize minimalSizeForOptimalDisplay() const override;

protected:
  int numberOfSubviews() const override { return 4; } // TODO could change according to mode
  void layoutSubviews(bool force = false) override;
  Escher::View * subviewAtIndex(int i) override;

private:
  enum class Mode { OneCurveView, TwoCurveViews };
  constexpr static int k_conclusionViewHeight = 40;
  constexpr static int k_separatorWidth = 14;
  DistributionCurveView m_curveViewLeft;
  DistributionCurveView m_curveViewRight;
  Escher::SolidColorView m_separatorView;
  ConclusionView m_conclusionView;
  Mode m_mode;
};

#endif /* APPS_PROBABILITY_GUI_DISTRIBUTION_GRAPH_VIEW_H */
