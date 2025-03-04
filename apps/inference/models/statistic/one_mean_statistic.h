#ifndef INFERENCE_MODELS_STATISTIC_ONE_MEAN_STATISTIC_H
#define INFERENCE_MODELS_STATISTIC_ONE_MEAN_STATISTIC_H

#include <poincare/statistics/inference.h>

#include "table_from_store.h"

namespace Inference {

class OneMeanStatistic : public TableFromStatisticStore {
 public:
  using TableFromStatisticStore::TableFromStatisticStore;
  int numberOfSeries() const override { return 1; }

 protected:
  void computeParametersFromSeries(const Statistic* stat,
                                   int pageIndex) override;

  double preProcessOneMeanParameter(double p, int index) const {
    if (index == PcrInference::Params::OneMean::N) {
      return std::round(p);
    }
    return p;
  }

  double m_params[PcrInference::NumberOfParameters(
      PcrInference::TestType::OneMean)];
};

}  // namespace Inference

#endif
