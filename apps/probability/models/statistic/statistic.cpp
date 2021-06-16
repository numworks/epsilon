#include "statistic.h"

#include <math.h>

#include "probability/app.h"

namespace Probability {

float Statistic::paramAtIndex(int i) {
  return i == indexOfThreshold() ? m_threshold : paramArray()[i];
}

void Statistic::setParamAtIndex(int i, float p) {
  if (i == indexOfThreshold()) {
    m_threshold = p;
  } else {
    assert(i < indexOfThreshold());
    paramArray()[i] = p;
  }
}

bool Statistic::testPassed() {
  return pValue() > std::fabs(m_threshold);
}

void Statistic::initThreshold() {
  if (App::app()->snapshot()->navigation()->subapp() == Data::SubApp::Tests) {
    m_threshold = 0.05;
  } else {
    m_threshold = 0.95;
  }
}

float Statistic::absIfNeeded(float f) {
  return m_hypothesisParams.op() == HypothesisParams::ComparisonOperator::Different ? fabs(f)
                                                                                    : f;
}

}  // namespace Probability
