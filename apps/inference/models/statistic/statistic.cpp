#include "statistic.h"

#include "one_mean_t_interval.h"
#include "one_mean_t_test.h"

namespace Inference {

double Statistic::parameterAtIndex(int i) const {
  assert(i <= indexOfThreshold() &&
         indexOfThreshold() == numberOfStatisticParameters());
  return i == indexOfThreshold()
             ? m_threshold
             : const_cast<Statistic *>(this)->parametersArray()[i];
}

void Statistic::setParameterAtIndex(double f, int i) {
  assert(i <= indexOfThreshold() &&
         indexOfThreshold() == numberOfStatisticParameters());
  if (i == indexOfThreshold()) {
    m_threshold = f;
  } else {
    assert(i < indexOfThreshold());
    parametersArray()[i] = f;
  }
}

bool Statistic::Initialize(Statistic *statistic, SubApp subApp) {
  if (statistic->subApp() == subApp) {
    return false;
  }
  statistic->~Statistic();
  Statistic *s = nullptr;
  switch (subApp) {
    case SubApp::Test:
      s = new (statistic) OneMeanTTest();
      break;
    case SubApp::Interval:
      s = new (statistic) OneMeanTInterval();
      break;
    default:
      assert(false);
  }
  if (s) {
    s->initParameters();
  }
  return true;
}

}  // namespace Inference
