#include "one_mean_statistic.h"

namespace Inference {

void OneMeanStatistic::syncParametersWithStore(Statistic* stat) {
  if (!hasSeries()) {
    return;
  }
  m_params[OneMean::ParamsOrder::x] = mean(series());
  // For Z tests, the S parameter is the population standard deviation, which is
  // not computed from the sample.
  if (stat->distributionType() != DistributionType::Z) {
    m_params[OneMean::ParamsOrder::s] = sampleStandardDeviation(series());
  }
  m_params[OneMean::ParamsOrder::n] = sumOfOccurrences(series());
}

}  // namespace Inference
