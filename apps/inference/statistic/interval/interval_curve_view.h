#ifndef INFERENCE_STATISTIC_INTERVAL_INTERVAL_CURVE_VIEW_H
#define INFERENCE_STATISTIC_INTERVAL_INTERVAL_CURVE_VIEW_H

#include "inference/models/statistic/interval.h"
#include "inference/statistic/statistic_curve_view.h"

namespace Inference {

class IntervalCurveView : public StatisticCurveView {
public:
  IntervalCurveView(Interval * interval) : StatisticCurveView(interval), m_interval(interval) {}
  void drawRect(KDContext * ctx, KDRect rect) const override;

private:
  constexpr static KDCoordinate k_intervalThickness = k_axisWidth;
  constexpr static KDCoordinate k_mainIntervalThickness = 2;
  constexpr static KDCoordinate k_intervalBoundHalfHeight = 4;

  void drawInterval(KDContext * ctx, KDRect rect) const;
  void drawLabelAndGraduationAtPosition(KDContext * ctx, float position, const char * text, RelativePosition horizontal = RelativePosition::None) const;
  void drawIntervalLabelAndGraduation(KDContext * ctx) const;
  void convertFloatToText(float value, char * buffer, int bufferSize) const;

  Interval * m_interval;
};

}  // namespace Inference

#endif /* INFERENCE_STATISTIC_INTERVAL_INTERVAL_CURVE_VIEW_H */
