#include "statistic.h"

namespace Probability {

bool Statistic::initializeSignificanceTest(SignificanceTestType testType) {
  if (testType == significanceTestType()) {
    initParameters();
    return false;
  }
  return true;
}

bool Statistic::initializeDistribution(DistributionType type) {
  if (type == distributionType()) {
    initParameters();
    return false;
  }
  return true;
}

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

}  // namespace Probability
