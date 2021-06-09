#include "statistic_view_range.h"

#include <ion/display.h>
#include <math.h>

namespace Probability {

float StatisticViewRange::xMin() const {
  return computeRange().min;
}

float StatisticViewRange::xMax() const {
  return computeRange().max;
}

StatisticViewRange::Range StatisticViewRange::computeRange() const {
  float p = m_statistic->pValue();
  float a = m_inputParams->threshold();
  float min = fmin(p, a);
  float max = fmax(p, a);
  float pixelWidth = (max - min) / k_areaSize;
  return m_mode == GraphDisplayMode::OneCurveView ? Range{-10, 10}
         : m_isLeftRange                         ? Range{-10, -0.5}
                                                 : Range{0.5, 10};
}

}  // namespace Probability
