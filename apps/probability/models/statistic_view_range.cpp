#include "statistic_view_range.h"

#include "probability/app.h"

namespace Probability {

void StatisticViewRange::setStatistic(Statistic * statistic) {
  m_statistic = statistic;

  protectedSetXMin(isInTestSubapp() ? canonicalXMin() : intervalXMin(), Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, false);
  protectedSetXMax(isInTestSubapp() ? canonicalXMax() : intervalXMax(), Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, true);
  protectedSetYMin(isInTestSubapp() ? canonicalYMin() : intervalYMin(), Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, false);
  protectedSetYMax(isInTestSubapp() ? canonicalYMax() : intervalYMax(), Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, true);
}

bool StatisticViewRange::isInTestSubapp() const {
  return App::app()->subapp() == Data::SubApp::Tests;
}

float StatisticViewRange::intervalYMin() const {
  return canonicalYMin();
}

float StatisticViewRange::intervalYMax() const {
  return canonicalYMax();
}

float StatisticViewRange::intervalXMin() const {
  return m_statistic->estimate() + canonicalXMin() * m_statistic->standardError();
}

float StatisticViewRange::intervalXMax() const {
  return m_statistic->estimate() + canonicalXMax() * m_statistic->standardError();
}

float StatisticViewRange::canonicalYMin() const {
  return m_statistic->yMin();
}

float StatisticViewRange::canonicalYMax() const {
  return m_statistic->yMax();
}

float StatisticViewRange::canonicalXMin() const {
  return m_statistic->xMin();
}

float StatisticViewRange::canonicalXMax() const {
  return m_statistic->xMax();
}

}  // namespace Probability
