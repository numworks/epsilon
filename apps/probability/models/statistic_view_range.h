#ifndef APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H
#define APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H

#include <apps/shared/curve_view_range.h>

#include "probability/models/statistic/statistic.h"

namespace Probability {

class StatisticCurveView;
enum class GraphDisplayMode { OneCurve, TwoCurve };

/* This class is in charge of computing the range to display for a given statistic.
 * In case two ranges are needed (the graph is split), the StatisticViewRange has a m_isLeftRange.
 */
class StatisticViewRange : public Shared::CurveViewRange {
public:
  StatisticViewRange(StatisticCurveView * curveView, bool isLeftRange) :
      m_isLeftRange(isLeftRange), m_statisticCurveView(curveView) {}
  void setStatistic(Statistic * statistic) { m_statistic = statistic; }
  float yMin() const override;
  float yMax() const override;
  float xMin() const override;
  float xMax() const override;
  void setMode(GraphDisplayMode m) { m_mode = m; }

private:
  struct Range {
    float min;
    float max;
  };
  Range computeXRange() const;
  Range computeTestXRange(float z, float zAlpha) const;
  Range computeYRange() const;
  constexpr static int k_marginSide = 30;
  constexpr static int k_areaWidth = 50;
  constexpr static int k_areaHeight = 65;
  constexpr static float k_yMargin = 0.2;
  Statistic * m_statistic;
  bool m_isLeftRange;
  GraphDisplayMode m_mode;
  StatisticCurveView * m_statisticCurveView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H */
