#ifndef APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H
#define APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H

#include <apps/shared/curve_view_range.h>

#include "probability/models/input_parameters.h"
#include "statistic.h"

namespace Probability {


enum class GraphDisplayMode { OneCurveView, TwoCurveViews };


/* This class is in charge of computing the range to display for a given statistic.
 * In case two ranges are needed (the graph is split), the StatisticViewRange has a m_isLeftRange.
 */
class StatisticViewRange : public Shared::CurveViewRange {
public:
  StatisticViewRange(bool isLeftRange) : m_isLeftRange(isLeftRange) {}
  void setStatistic(Statistic * statistic) { m_statistic = statistic; }
  void setInputParams(InputParameters * inputParams) { m_inputParams = inputParams; }
  float yMin() const override { return k_yMin; }
  float yMax() const override { return k_yMax; }
  float xMin() const override;
  float xMax() const override;
  void setMode(GraphDisplayMode m) { m_mode = m; }

private:
  struct Range {
    float min;
    float max;
  };
  Range computeRange() const;
  constexpr static float k_yMin = -0.1;
  constexpr static float k_yMax = 0.4;
  constexpr static int k_marginLeftOfMin = 30;
  constexpr static int k_areaSize = 50;
  Statistic * m_statistic;
  InputParameters * m_inputParams;
  bool m_isLeftRange;
  GraphDisplayMode m_mode;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H */
