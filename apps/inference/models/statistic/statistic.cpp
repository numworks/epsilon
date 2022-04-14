#include "statistic.h"

namespace Inference {

double Statistic::parameterAtIndex(int i) const {
  assert(i <= indexOfThreshold() && indexOfThreshold() == numberOfStatisticParameters());
  return i == indexOfThreshold() ? m_threshold : const_cast<Statistic *>(this)->parametersArray()[i];
}

void Statistic::setParameterAtIndex(double f, int i) {
  assert(i <= indexOfThreshold() && indexOfThreshold() == numberOfStatisticParameters());
  if (i == indexOfThreshold()) {
    m_threshold = f;
  } else {
    assert(i < indexOfThreshold());
    parametersArray()[i] = f;
  }
}

}  // namespace Inference
