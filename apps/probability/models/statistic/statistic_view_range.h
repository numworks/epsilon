#ifndef APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H
#define APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H

#include <apps/shared/curve_view_range.h>

#include "statistic.h"
#include "probability/models/input_parameters.h"

namespace Probability {

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

private:
  constexpr static float k_yMin = -0.2;
  constexpr static float k_yMax = 1.2;
  Statistic * m_statistic;
  InputParameters * m_inputParams;
  bool m_isLeftRange;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H */
