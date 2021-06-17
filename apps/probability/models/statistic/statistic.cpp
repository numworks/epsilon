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
  if (App::app()->subapp() == Data::SubApp::Tests) {
    m_threshold = 0.05;
  } else {
    m_threshold = 0.95;
  }
}

void Statistic::initializeStatistic(Statistic * statistic, Data::Test t, Data::TestType type) {
  switch (t) {
    case Data::Test::OneProp:
      new (statistic) OneProportionStatistic();
      break;
    case Data::Test::TwoProps:
      new (statistic) TwoProportionsStatistic();
      break;
    case Data::Test::OneMean:
      if (type == Data::TestType::TTest) {
        new (statistic) OneMeanTStatistic();
      } else if (type == Data::TestType::ZTest) {
        new (statistic) OneMeanZStatistic();
      }
      break;
    case Data::Test::TwoMeans:
      if (type == Data::TestType::TTest) {
        new (statistic) TwoMeansTStatistic();
      } else if (type == Data::TestType::ZTest) {
        new (statistic) TwoMeansZStatistic();
      } else if (type == Data::TestType::PooledTTest) {
        new (statistic) PooledTwoMeansStatistic();
      }
      break;
    default:
      assert(false);
      break;
  }
}

float Statistic::absIfNeeded(float f) {
  return m_hypothesisParams.op() == HypothesisParams::ComparisonOperator::Different ? fabs(f) : f;
}

}  // namespace Probability
