#include "statistic_view_range.h"

#include "probability/app.h"

namespace Probability {

float StatisticViewRange::yMin() const {
  float ym = isInTestSubapp() ? canonicalYMin() : intervalYMin();
  assert(ym < yMax());
  return ym;
}

float StatisticViewRange::yMax() const {
  return isInTestSubapp() ? canonicalYMax() : intervalYMax();
}

float StatisticViewRange::xMin() const {
  float xm = isInTestSubapp() ? canonicalXMin() : intervalXMin();
  assert(xm < xMax());
  return xm;
}

float StatisticViewRange::xMax() const {
  return isInTestSubapp() ? canonicalXMax() : intervalXMax();
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
