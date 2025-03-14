#include "one_mean_statistic.h"

#include <apps/shared/store_to_series.h>
#include <poincare/statistics/inference.h>

namespace Inference {

void OneMeanStatistic::computeParametersFromSeries(const Statistic* stat,
                                                   int pageIndex) {
  assert(hasSeries(pageIndex));
  int seriesIndex = seriesAt(pageIndex);
  Shared::StoreToSeries seriesModel(this, seriesIndex);
  Poincare::Inference::ParametersArray array =
      Poincare::Inference::ComputeOneMeanParametersFromSeries(seriesModel);

  constexpr int nParams =
      Poincare::Inference::NumberOfParameters(TestType::OneMean);
  for (int i = 0; i < nParams; i++) {
    /* For T tests, the S parameter is the sample standard deviation, which
     * can be computed from the dataset. For Z tests however, the S parameter
     * is the population standard deviation, which is given by the user. */
    if (stat->statisticType() == StatisticType::Z && i == Params::OneMean::S) {
      continue;
    }
    m_params[i] = array[i];
  }
}

}  // namespace Inference
