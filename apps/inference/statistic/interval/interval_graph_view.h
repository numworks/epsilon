#ifndef INFERENCE_STATISTIC_INTERVAL_GRAPH_VIEW_H
#define INFERENCE_STATISTIC_INTERVAL_GRAPH_VIEW_H

#include <escher/view.h>
#include "inference/statistic/interval/interval_curve_view.h"
#include "inference/statistic/interval/interval_conclusion_view.h"

namespace Inference {

class IntervalGraphView : public Escher::View {
public:
  IntervalGraphView(Interval * interval) : m_curveView(interval) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void reload(bool resetSelectedInterval, bool force = false);
  void selectAdjacentInterval(bool goUp);
private:
  constexpr static int k_conclusionViewHeight = 50;

  int numberOfSubviews() const override { return 2; }
  void layoutSubviews(bool force = false) override;
  Escher::View * subviewAtIndex(int i) override;

  IntervalCurveView m_curveView;
  IntervalConclusionView m_conclusionView;
};

}  // namespace Inference

#endif /* INFERENCE_STATISTIC_INTERVAL_GRAPH_VIEW_H */
