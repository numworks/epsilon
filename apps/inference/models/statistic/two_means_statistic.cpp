#include "two_means_statistic.h"

namespace Inference {

void TwoMeansStatistic::syncParametersWithStore(Statistic* stat) {
  if (!hasSeries()) {
    return;
  }

  /* For T tests, the S parameters are the sample standard deviations, which can
   * be computed from the datasets. For Z tests however, the S parameters are
   * the population standard deviations, which are given by the user. */
  m_params[TwoMeans::ParamsOrder::x1] = mean(seriesAt(0));
  m_params[TwoMeans::ParamsOrder::x2] = mean(seriesAt(1));
  TwoMeans::Type type = twoMeansType(stat);
  if (type != TwoMeans::Type::Z) {
    m_params[TwoMeans::ParamsOrder::s1] = sampleStandardDeviation(seriesAt(0));
    m_params[TwoMeans::ParamsOrder::s2] = sampleStandardDeviation(seriesAt(1));
  }
  m_params[TwoMeans::ParamsOrder::n1] = sumOfOccurrences(seriesAt(0));
  m_params[TwoMeans::ParamsOrder::n2] = sumOfOccurrences(seriesAt(1));
}

}  // namespace Inference
