#ifndef APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H
#define APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H

#include <apps/shared/curve_view_range.h>

#include "probability/models/statistic/statistic.h"

namespace Probability {

/* This class is in charge of computing the range to display for a given statistic.
 * It wraps the range required by the statistic and scales it if needed, depending on the type of
 * the graph (Test or Interval)
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
  float canonicalYMin() const;
  float canonicalYMax() const;
  float canonicalXMin() const;
  float canonicalXMax() const;

  Statistic * m_statistic;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_VIEW_RANGE_H */
