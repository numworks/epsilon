#include "statistic_view_range.h"

#include <math.h>

namespace Probability {

float StatisticViewRange::xMin() const {
  float p = m_statistic->pValue();
  float a = m_inputParams->threshold();
  return fmin(p, a) - 10 * (!m_isLeftRange);
}

float StatisticViewRange::xMax() const {
  float p = m_statistic->pValue();
  float a = m_inputParams->threshold();
  return fmax(p, a) + 10 * m_isLeftRange;
}

}  // namespace Probability
