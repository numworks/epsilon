#ifndef INFERENCE_MODELS_STATISTIC_TWO_MEANS_STATISTIC_H
#define INFERENCE_MODELS_STATISTIC_TWO_MEANS_STATISTIC_H

#include <poincare/statistics/inference.h>

#include "table_from_store.h"

namespace Inference {

class TwoMeansStatistic : public TableFromStatisticStore {
 public:
  using TableFromStatisticStore::TableFromStatisticStore;

  int numberOfSeries() const override { return 2; }

 protected:
  void computeParametersFromSeries(const Statistic* stat,
                                   int pageIndex) override;

  double preProcessTwoMeansParameter(double p, int index) const {
    if (index == PcrInference::Params::TwoMeans::N1 ||
        index == PcrInference::Params::TwoMeans::N2) {
      return std::round(p);
    }
    return p;
  }

  double m_params[PcrInference::NumberOfParameters(
      PcrInference::TestType::TwoMeans)];
};

}  // namespace Inference

#endif
