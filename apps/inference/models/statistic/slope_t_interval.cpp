#include "slope_t_interval.h"

namespace Inference {

void SlopeTInterval::compute() {
  double n = doubleCastedNumberOfPairsOfSeries(0);
  m_degreesOfFreedom = n - 2.0;
  m_zCritical = computeIntervalCriticalValue();
  m_SE = computeStandardError();
  m_marginOfError = m_zCritical * m_SE;
  m_estimate = slope(0);
}

}  // namespace Inference
