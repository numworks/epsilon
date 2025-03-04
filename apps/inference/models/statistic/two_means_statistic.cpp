#include "two_means_statistic.h"

namespace Inference {

void TwoMeansStatistic::computeParametersFromSeries(const Statistic* stat,
                                                    int pageIndex) {
  int series = seriesAt(pageIndex);
  assert(series >= 0);

  int xIndex = 0;
  int sIndex = 0;
  int nIndex = 0;

  if (pageIndex == 0) {
    xIndex = PcrInference::Params::TwoMeans::X1;
    sIndex = PcrInference::Params::TwoMeans::S1;
    nIndex = PcrInference::Params::TwoMeans::N1;
  } else {
    xIndex = PcrInference::Params::TwoMeans::X2;
    sIndex = PcrInference::Params::TwoMeans::S2;
    nIndex = PcrInference::Params::TwoMeans::N2;
  }

  /* For T tests, the S parameters are the sample standard deviations, which can
   * be computed from the datasets. For Z tests however, the S parameters are
   * the population standard deviations, which are given by the user. */
  m_params[xIndex] = mean(series);
  if (stat->statisticType() != PcrInference::StatisticType::Z) {
    m_params[sIndex] = sampleStandardDeviation(series);
  }
  m_params[nIndex] = sumOfOccurrences(series);
}

}  // namespace Inference
