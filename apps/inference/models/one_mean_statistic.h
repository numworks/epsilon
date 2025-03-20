#ifndef INFERENCE_MODELS_ONE_MEAN_STATISTIC_H
#define INFERENCE_MODELS_ONE_MEAN_STATISTIC_H

#include <poincare/statistics/inference.h>

#include "input_table_from_store.h"

namespace Inference {

class OneMeanStatistic : public InputTableFromStatisticStore {
 public:
  using InputTableFromStatisticStore::InputTableFromStatisticStore;
  int numberOfSeries() const override { return 1; }

 protected:
  void computeParametersFromSeries(const InferenceModel* inference,
                                   int pageIndex) override;

  double preProcessOneMeanParameter(double p, int index) const {
    if (index == Params::OneMean::N) {
      return std::round(p);
    }
    return p;
  }

  double m_params[Poincare::Inference::NumberOfParameters(TestType::OneMean)];
};

}  // namespace Inference

#endif
