#ifndef STATISTICS_NORMAL_PROBABILITY_VIEW_H
#define STATISTICS_NORMAL_PROBABILITY_VIEW_H

#include "plot_view.h"
#include "normal_probability_curve_view.h"
#include "../store.h"

namespace Statistics {

class NormalProbabilityView : public PlotView {
public:
  NormalProbabilityView(Store * store);
  PlotCurveView * plotCurveView() override { return &m_curveView; }

private:
  void computeYBounds(float * yMin, float *yMax) override;

  NormalProbabilityCurveView m_curveView;
};

}  // namespace Statistics

#endif /* STATISTICS_NORMAL_PROBABILITY_VIEW_H */
