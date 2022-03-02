#ifndef STATISTICS_NORMAL_PROBABILITY_CURVE_VIEW_H
#define STATISTICS_NORMAL_PROBABILITY_CURVE_VIEW_H

#include "plot_curve_view.h"
namespace Statistics {

class NormalProbabilityCurveView : public PlotCurveView {
public:
  using PlotCurveView::PlotCurveView;
  int totalValues(int series, int * sortedIndex) const override { return m_store->totalNormalProbabilityValues(series); }
  double valueAtIndex(int series, int * sortedIndex, int i) const override { return m_store->normalProbabilityValueAtIndex(series, sortedIndex, i); }
  double resultAtIndex(int series, int * sortedIndex, int i) const override { return m_store->normalProbabilityResultAtIndex(series, i); }
  void drawSeriesCurve(KDContext * ctx, KDRect rect, int series) const override;
};

}  // namespace Statistics

#endif /* STATISTICS_NORMAL_PROBABILITY_CURVE_VIEW_H */
