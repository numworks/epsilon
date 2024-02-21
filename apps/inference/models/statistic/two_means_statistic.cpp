#include "two_means_statistic.h"

namespace Inference {

void TwoMeansStatistic::syncParametersWithStore(Statistic* stat) {
  if (!hasSeries()) {
    return;
  }

  m_params[TwoMeans::ParamsOrder::x1] = mean(seriesAt(0));
  m_params[TwoMeans::ParamsOrder::x2] = mean(seriesAt(1));
  if (stat->distributionType() != DistributionType::Z) {
    m_params[TwoMeans::ParamsOrder::s1] = sampleStandardDeviation(seriesAt(0));
    m_params[TwoMeans::ParamsOrder::s2] = sampleStandardDeviation(seriesAt(1));
  }
  m_params[TwoMeans::ParamsOrder::n1] = sumOfOccurrences(seriesAt(0));
  m_params[TwoMeans::ParamsOrder::n2] = sumOfOccurrences(seriesAt(1));
}

}  // namespace Inference
