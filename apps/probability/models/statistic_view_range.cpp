#include "statistic_view_range.h"

#include <ion/display.h>
#include <math.h>

#include "probability/app.h"
#include "probability/helpers.h"

namespace Probability {

float StatisticViewRange::xMin() const {
  return computeXRange().min;
}

float StatisticViewRange::xMax() const {
  return computeXRange().max;
}

float StatisticViewRange::yMin() const {
  return computeYRange().min;
}

float StatisticViewRange::yMax() const {
  return computeYRange().max;
}

StatisticViewRange::Range StatisticViewRange::computeXRange() const {
  // TODO this is called +100 times, optimize ?
  if (App::app()->snapshot()->navigation()->subapp() == Data::SubApp::Tests) {
    float zAlpha = m_statistic->zAlpha();
    float z = m_statistic->testCriticalValue();
    float min = fminf(zAlpha, z);
    float max = fmaxf(zAlpha, z);
    float pixelWidth = (max - min) / k_areaWidth;
    if (m_mode == GraphDisplayMode::OneCurveView) {
      return Range{min - k_marginLeftOfMin * pixelWidth,
                   min + (Ion::Display::Width - k_marginLeftOfMin) * pixelWidth};
    }
    return m_isLeftRange ? Range{-10, -0.5} : Range{0.5, 10};
  }
  float center = m_statistic->estimate();
  float delta = m_statistic->standardError();
  constexpr static float factor = 2.5;
  return Range{center - factor * delta, center + factor * delta};
}

StatisticViewRange::Range StatisticViewRange::computeYRange() const {
  if (App::app()->snapshot()->navigation()->subapp() == Data::SubApp::Intervals) {
    return Range{-0.01, 0.5};
  }
  float zAlpha = m_statistic->zAlpha();
  float z = m_statistic->testCriticalValue();
  float min =
      fminf(m_statistic->normedDensityFunction(z), m_statistic->normedDensityFunction(zAlpha));
  float pixelHeight = min / k_areaHeight;
  return Range{-25 * pixelHeight, 100 * pixelHeight};  // TODO access or compute view height
}

}  // namespace Probability
