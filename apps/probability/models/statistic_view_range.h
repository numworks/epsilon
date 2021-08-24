#ifndef APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H
#define APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H

#include <apps/shared/curve_view_range.h>

#include "probability/models/statistic/statistic.h"

namespace Probability {

/* This class is in charge of computing the range to display for a given statistic.
 * In case two ranges are needed (the graph is split), the StatisticViewRange has a m_isLeftRange.
 */
class StatisticViewRange : public Shared::CurveViewRange {
public:
  void setStatistic(Statistic * statistic) { m_statistic = statistic; }
  float yMin() const override;
  float yMax() const override;
  float xMin() const override;
  float xMax() const override;

private:
  bool isInTestSubapp() const;
  float intervalYMin() const;
  float intervalYMax() const;
  float intervalXMin() const;
  float intervalXMax() const;
  float normalizedYMin() const;
  float normalizedYMax() const;
  float normalizedXMin() const;
  float normalizedXMax() const;

  Statistic * m_statistic;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H */
