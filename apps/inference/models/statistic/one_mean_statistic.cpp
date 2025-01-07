#include "one_mean_statistic.h"

namespace Inference {

void OneMeanStatistic::syncParametersWithStore(const Statistic* stat) {
  if (!hasSeries()) {
    return;
  }
  m_params[OneMean::ParamsOrder::x] = mean(series());
  /* For T tests, the S parameter is the sample standard deviation, which can be
   * computed from the dataset. For Z tests however, the S parameter is the
   * population standard deviation, which is given by the user. */
  OneMean::Type type = oneMeanType(stat);
  if (type == OneMean::Type::T) {
    m_params[OneMean::ParamsOrder::s] = sampleStandardDeviation(series());
  }
  m_params[OneMean::ParamsOrder::n] = sumOfOccurrences(series());
}

}  // namespace Inference
