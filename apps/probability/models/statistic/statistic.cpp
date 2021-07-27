#include "statistic.h"

#include <assert.h>
#include <math.h>

#include <new>

#include "goodness_statistic.h"
#include "homogeneity_statistic.h"
#include "one_mean_t_statistic.h"
#include "one_mean_z_statistic.h"
#include "one_proportion_statistic.h"
#include "pooled_two_means_statistic.h"
#include "two_means_t_statistic.h"
#include "two_means_z_statistic.h"
#include "two_proportions_statistic.h"

namespace Probability {

float Statistic::paramAtIndex(int i) {
  return i == indexOfThreshold() ? m_threshold : paramArray()[i];
}

bool Statistic::isValidParamAtIndex(int i, float p) {
  if (i == indexOfThreshold()) {
    return p >= 0 && p <= 1;
  }
  return true;
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
  return pValue() < std::fabs(m_threshold);
}

void Statistic::initThreshold(Data::SubApp subapp) {
  if (subapp == Data::SubApp::Tests) {
    m_threshold = 0.05;
  } else {
    m_threshold = 0.95;
  }
}

void Statistic::initializeStatistic(Statistic * statistic,
                                    Data::Test t,
                                    Data::TestType type,
                                    Data::CategoricalType categoricalType) {
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
    case Data::Test::Categorical:
      if (categoricalType == Data::CategoricalType::Goodness) {
        new (statistic) GoodnessStatistic();
      } else {
        new (statistic) HomogeneityStatistic();
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
