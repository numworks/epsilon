#ifndef STATISTICS_NORMAL_PROBABILITY_CURVE_VIEW_H
#define STATISTICS_NORMAL_PROBABILITY_CURVE_VIEW_H

#include "plot_curve_view.h"
namespace Statistics {

class NormalProbabilityCurveView : public PlotCurveView {
public:
  using PlotCurveView::PlotCurveView;
  double valueAtIndex(int series, int * sortedIndex, int i) const override { return m_store->zScoreAtSortedIndex(series, sortedIndex, i); }
  void drawSeriesCurve(KDContext * ctx, KDRect rect, int series) const override;
};

}  // namespace Statistics

#endif /* STATISTICS_NORMAL_PROBABILITY_CURVE_VIEW_H */
