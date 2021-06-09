#include "statistic.h"

#include "probability/app.h"

namespace Probability {

float Statistic::paramAtIndex(int i) {
  return i == numberOfParameters() ? m_threshold : paramArray()[i];
}

bool Statistic::testPassed() {
   return pValue() > threshold();
}

} // namespace Probability
