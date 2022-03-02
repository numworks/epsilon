#ifndef STATISTICS_FREQUENCY_VIEW_H
#define STATISTICS_FREQUENCY_VIEW_H

#include "plot_view.h"
#include "frequency_curve_view.h"
#include "../store.h"

namespace Statistics {

class FrequencyView : public PlotView {
public:
  FrequencyView(Store * store);
  PlotCurveView * plotCurveView() override { return &m_curveView; }

private:
  void computeYBounds(float * yMin, float *yMax) override;
  void computeXBounds(float * xMin, float *xMax) override;

  FrequencyCurveView m_curveView;
};

}  // namespace Statistics

#endif /* STATISTICS_FREQUENCY_VIEW_H */
