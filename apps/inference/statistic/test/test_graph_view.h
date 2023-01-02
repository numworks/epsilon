#ifndef INFERENCE_STATISTIC_TEST_GRAPH_VIEW_H
#define INFERENCE_STATISTIC_TEST_GRAPH_VIEW_H

#include <escher/view.h>
#include "inference/statistic/test/legend_view.h"
#include "inference/statistic/test/test_curve_view.h"
#include "inference/statistic/test/zoom_hint_view.h"

namespace Inference {

class TestGraphView : public Escher::View {
public:
  TestGraphView(Test * test) : m_curveView(test) {}
  void reload();

  void setDisplayHint(bool displayHint) { m_displayHint = displayHint; }
  bool displayHint() { return m_displayHint; }

private:
  constexpr static int k_legendMarginRight = 10;
  constexpr static int k_legendMarginTop = 10;
  constexpr static KDCoordinate k_zoomHintHeight = ZoomHintView::k_legendHeight;

  int numberOfSubviews() const override { return 3; }
  void layoutSubviews(bool force = false) override;
  Escher::View * subviewAtIndex(int i) override;

  bool m_displayHint;
  TestCurveView m_curveView;
  LegendView m_legend;
  ZoomHintView m_zoom_hint;
};

}

#endif
