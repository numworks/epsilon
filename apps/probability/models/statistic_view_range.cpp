#include "statistic_view_range.h"

#include <ion/display.h>
#include <math.h>

#include "probability/helpers.h"

namespace Probability {

float StatisticViewRange::xMin() const {
  return computeRange().min;
}

float StatisticViewRange::xMax() const {
  return computeRange().max;
}

StatisticViewRange::Range StatisticViewRange::computeRange() const {
  float p = m_statistic->pValue();
  float z = m_statistic->testCriticalValue();
  float min = fminf(p, z);
  float max = fmaxf(p, z);
  float pixelWidth = (max - min) / k_areaSize;
  if (m_mode == GraphDisplayMode::OneCurveView) {
    return Range{min - k_marginLeftOfMin * pixelWidth,
                 min + (Ion::Display::Width - k_marginLeftOfMin) * pixelWidth};
  }
  return m_isLeftRange ? Range{-10, -0.5} : Range{0.5, 10};
}

}  // namespace Probability
