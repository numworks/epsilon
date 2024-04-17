#include "two_means_statistic.h"

namespace Inference {

void TwoMeansStatistic::syncParametersWithStore(Statistic* stat) {
  if (!hasSeries()) {
    return;
  }
  int series1 = seriesAt(0);
  int series2 = seriesAt(1);

  /* For T tests, the S parameters are the sample standard deviations, which can
   * be computed from the datasets. For Z tests however, the S parameters are
   * the population standard deviations, which are given by the user. */
  m_params[TwoMeans::ParamsOrder::x1] = mean(series1);
  m_params[TwoMeans::ParamsOrder::x2] = mean(series2);
  TwoMeans::Type type = twoMeansType(stat);
  if (type != TwoMeans::Type::Z) {
    m_params[TwoMeans::ParamsOrder::s1] = sampleStandardDeviation(series1);
    m_params[TwoMeans::ParamsOrder::s2] = sampleStandardDeviation(series2);
  }
  m_params[TwoMeans::ParamsOrder::n1] = sumOfOccurrences(series1);
  m_params[TwoMeans::ParamsOrder::n2] = sumOfOccurrences(series2);
}

}  // namespace Inference
