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
    xIndex = Params::TwoMeans::X1;
    sIndex = Params::TwoMeans::S1;
    nIndex = Params::TwoMeans::N1;
  } else {
    xIndex = Params::TwoMeans::X2;
    sIndex = Params::TwoMeans::S2;
    nIndex = Params::TwoMeans::N2;
  }

  /* For T tests, the S parameters are the sample standard deviations, which can
   * be computed from the datasets. For Z tests however, the S parameters are
   * the population standard deviations, which are given by the user. */
  m_params[xIndex] = mean(series);
  if (stat->statisticType() != StatisticType::Z) {
    m_params[sIndex] = sampleStandardDeviation(series);
  }
  m_params[nIndex] = sumOfOccurrences(series);
}

}  // namespace Inference
