#ifndef INFERENCE_STATISTIC_TEST_GRAPH_VIEW_H
#define INFERENCE_STATISTIC_TEST_GRAPH_VIEW_H

#include <escher/view.h>
#include "inference/statistic/test/legend_view.h"
#include "inference/statistic/test/test_curve_view.h"

namespace Inference {

class TestGraphView : public Escher::View {
public:
  TestGraphView(Test * test) : m_curveView(test) {}
  void reload();

private:
  constexpr static int k_legendMarginRight = 10;
  constexpr static int k_legendMarginTop = 10;

  int numberOfSubviews() const override { return 2; }
  void layoutSubviews(bool force = false) override;
  Escher::View * subviewAtIndex(int i) override;

  TestCurveView m_curveView;
  LegendView m_legend;
};

}  // namespace Inference

#endif /* INFERENCE_STATISTIC_TEST_GRAPH_VIEW_H */

