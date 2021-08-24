#include "statistic_view_range.h"

#include <algorithm>
#include <cmath>

#include "probability/app.h"
#include "probability/gui/statistic_curve_view.h"

namespace Probability {

float StatisticViewRange::yMin() const {
  return isInTestSubapp() ? normalizedYMin() : intervalYMin();
}

float StatisticViewRange::yMax() const {
  return isInTestSubapp() ? normalizedYMax() : intervalYMax();
}

float StatisticViewRange::xMin() const {
  return isInTestSubapp() ? normalizedXMin() : intervalXMin();
}

float StatisticViewRange::xMax() const {
  return isInTestSubapp() ? normalizedXMax() : intervalXMax();
}

bool StatisticViewRange::isInTestSubapp() const {
  return App::app()->subapp() == Data::SubApp::Tests;
}

float StatisticViewRange::intervalYMin() const {
  return normalizedYMin();
}

float StatisticViewRange::intervalYMax() const {
  return normalizedYMax();
}

float StatisticViewRange::intervalXMin() const {
  return m_statistic->estimate() + normalizedXMin() * m_statistic->standardError();
}

float StatisticViewRange::intervalXMax() const {
  return m_statistic->estimate() + normalizedXMax() * m_statistic->standardError();
}

float StatisticViewRange::normalizedYMin() const {
  return m_statistic->yMin();
}

float StatisticViewRange::normalizedYMax() const {
  return m_statistic->yMax();
}

float StatisticViewRange::normalizedXMin() const {
  return m_statistic->xMin();
}

float StatisticViewRange::normalizedXMax() const {
  return m_statistic->xMax();
}

}  // namespace Probability
