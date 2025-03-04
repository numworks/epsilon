#include "one_mean_statistic.h"

#include <poincare/statistics/inference.h>

namespace Inference {

void OneMeanStatistic::computeParametersFromSeries(const Statistic* stat,
                                                   int pageIndex) {
  assert(hasSeries(pageIndex));

  int series = seriesAt(pageIndex);
  m_params[PcrInference::Params::OneMean::X] = mean(series);
  m_params[PcrInference::Params::OneMean::N] = sumOfOccurrences(series);

  /* For T tests, the S parameter is the sample standard deviation, which
   * can be computed from the dataset. For Z tests however, the S parameter
   * is the population standard deviation, which is given by the user. */
  if (stat->statisticType() == PcrInference::StatisticType::T) {
    const double s = sampleStandardDeviation(series);
    m_params[PcrInference::Params::OneMean::S] = s;
  }
}

}  // namespace Inference
