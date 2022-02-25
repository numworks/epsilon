#include "statistic.h"

#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <new>
#include <float.h>

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

Statistic::~Statistic() {
  tidy();
}

double Statistic::paramAtIndex(int i) {
  assert(i <= indexOfThreshold() && indexOfThreshold() == numberOfStatisticParameters());
  return i == indexOfThreshold() ? m_threshold : paramArray()[i];
}

bool Statistic::isValidParamAtIndex(int i, double p) {
  assert(i <= indexOfThreshold() && indexOfThreshold() == numberOfStatisticParameters());
  if (i == indexOfThreshold()) {
    return p >= 0.0 && p <= 1.0;
  }
  return true;
}

void Statistic::setParamAtIndex(int i, double p) {
  assert(i <= indexOfThreshold() && indexOfThreshold() == numberOfStatisticParameters());
  if (i == indexOfThreshold()) {
    m_threshold = p;
  } else {
    assert(i < indexOfThreshold());
    paramArray()[i] = p;
  }
}

bool Statistic::canRejectNull() {
  assert(m_threshold >= 0 && m_threshold <= 1);
  return pValue() <= m_threshold;
}

Poincare::Layout Statistic::intervalCriticalValueSymbol() {
  return Poincare::HorizontalLayout::Builder(
      testCriticalValueSymbol(),
      Poincare::VerticalOffsetLayout::Builder(
          Poincare::CodePointLayout::Builder('*'),
          Poincare::VerticalOffsetLayoutNode::Position::Superscript));
}

void Statistic::initThreshold(Data::SubApp subapp) {
  if (subapp == Data::SubApp::Tests) {
    m_threshold = 0.05;
  } else {
    m_threshold = 0.95;
  }
}

void Statistic::initializeStatistic(Statistic * statistic,
                                    Data::SubApp subapp,
                                    Data::Test t,
                                    Data::TestType type,
                                    Data::CategoricalType categoricalType) {
  statistic->~Statistic();
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
  statistic->init(subapp);
}

bool Statistic::isGraphable() const {
  double SE = standardError();
  assert(std::isnan(SE) || SE >= 0);
  return std::isnan(SE) || SE >= FLT_MIN;
}

float Statistic::yMin() const {
  // TODO factor with Distribution
  return -k_displayBottomMarginRatio * yMax();
}

double Statistic::computePValue(double z, HypothesisParams::ComparisonOperator op) const {
  // Compute probability of obtaining a more extreme result
  switch (op) {
    case HypothesisParams::ComparisonOperator::Higher:
      return 1.0 - cumulativeNormalizedDistributionFunction(z);
    case HypothesisParams::ComparisonOperator::Lower:
      return cumulativeNormalizedDistributionFunction(z);
    case HypothesisParams::ComparisonOperator::Different:
      return 2.0 * cumulativeNormalizedDistributionFunction(-std::fabs(z));
    default:
      assert(false);
      return -1.0;
  }
}

double Statistic::computeIntervalCriticalValue(double confidenceLevel) const {
  return cumulativeNormalizedInverseDistributionFunction(0.5 + confidenceLevel / 2);
}

}  // namespace Probability
