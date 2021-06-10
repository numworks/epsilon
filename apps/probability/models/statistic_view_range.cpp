#include "statistic_view_range.h"

#include <ion/display.h>
#include <math.h>

#include "probability/app.h"
#include "probability/helpers.h"

namespace Probability {

float StatisticViewRange::yMax() const {
  float zAlpha = m_statistic->zAlpha();
  float z = m_statistic->testCriticalValue();
  float max =
      fmaxf(m_statistic->normedDensityFunction(z), m_statistic->normedDensityFunction(zAlpha));
  float pixelHeight = max / k_areaHeight;
  return 100 * pixelHeight;  // TODO access or compute view height
}

float StatisticViewRange::xMin() const {
  return computeRange().min;
}

float StatisticViewRange::xMax() const {
  return computeRange().max;
}

StatisticViewRange::Range StatisticViewRange::computeRange() const {
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

}  // namespace Probability
