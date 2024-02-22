#include "slope_t_test.h"

namespace Inference {

void SlopeTTest::compute() {
  double n = doubleCastedNumberOfPairsOfSeries(m_series);
  double b = slope(m_series);
  m_degreesOfFreedom = n - 2.0;
  m_testCriticalValue = b / computeStandardError();
  m_pValue = SignificanceTest::ComputePValue(this);
}

}  // namespace Inference
