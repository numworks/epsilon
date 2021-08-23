#include "statistic_view_range.h"

#include <algorithm>
#include <cmath>

#include "probability/app.h"
#include "probability/gui/statistic_curve_view.h"

namespace Probability {

float StatisticViewRange::yMin() const {
  return isInTestSubapp() ? testYMin() : intervalYMin();
}

float StatisticViewRange::yMax() const {
  return isInTestSubapp() ? testYMax() : intervalYMax();
}

float StatisticViewRange::xMin() const {
  return isInTestSubapp() ? testXMin() : intervalXMin();
}

float StatisticViewRange::xMax() const {
  return isInTestSubapp() ? testXMax() : intervalXMax();
}

bool StatisticViewRange::isInTestSubapp() const {
  return App::app()->subapp() == Data::SubApp::Tests;
}

float StatisticViewRange::intervalYMin() const {
  return testYMin();
}

float StatisticViewRange::intervalYMax() const {
  return testYMax();
}

float StatisticViewRange::intervalXMin() const {
  return m_statistic->estimate() + .5 * testXMin() * m_statistic->marginOfError();
}

float StatisticViewRange::intervalXMax() const {
  return m_statistic->estimate() + .5 * testXMax() * m_statistic->marginOfError();
}

float StatisticViewRange::testYMin() const {
  return -0.2 * testYMax();
}

float StatisticViewRange::testYMax() const {
  return 1.05 * m_statistic->normalizedDensityFunction(0);
}

float StatisticViewRange::testXMin() const {
  return -5.0;
}

float StatisticViewRange::testXMax() const {
  return 5.0;
}

}  // namespace Probability
