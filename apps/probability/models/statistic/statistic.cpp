#include "statistic.h"

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
  return pValue() > threshold();
}

void Statistic::initThreshold() {
  if (App::app()->snapshot()->navigation()->subapp() == Data::SubApp::Tests) {
    m_threshold = 0.05;
  } else {
    m_threshold = 0.95;
  }
}

}  // namespace Probability
