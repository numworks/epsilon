#include "slope_t_interval.h"

namespace Inference {

void SlopeTInterval::privateCompute() {
  double n = doubleCastedNumberOfPairsOfSeries(0);
  m_degreesOfFreedom = n - 2.0;
  m_SE = computeStandardError();
  m_estimate = slope(0);
}

}  // namespace Inference
