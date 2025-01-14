#include "two_means_statistic.h"

namespace Inference {

void TwoMeansStatistic::syncParametersWithStore(const Statistic* stat) {
  if (!hasSeries(static_cast<int>(m_activePageIndex))) {
    return;
  }
  syncParametersWithStore(stat, static_cast<uint8_t>(m_activePageIndex));
}

void TwoMeansStatistic::syncParametersWithStore(const Statistic* stat,
                                                uint8_t index) {
  int series = seriesAt(index);
  assert(series >= 0);

  /* For T tests, the S parameters are the sample standard deviations, which can
   * be computed from the datasets. For Z tests however, the S parameters are
   * the population standard deviations, which are given by the user. */
  m_params[TwoMeans::X(index)] = mean(series);
  TwoMeans::Type type = twoMeansType(stat);
  if (type != TwoMeans::Type::Z) {
    m_params[TwoMeans::S(index)] = sampleStandardDeviation(series);
  }
  m_params[TwoMeans::N(index)] = sumOfOccurrences(series);
}

}  // namespace Inference
