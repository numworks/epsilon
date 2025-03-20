#ifndef INFERENCE_MODELS_TWO_MEANS_STATISTIC_H
#define INFERENCE_MODELS_TWO_MEANS_STATISTIC_H

#include <poincare/statistics/inference.h>

#include "input_table_from_store.h"

namespace Inference {

class TwoMeansStatistic : public InputTableFromStatisticStore {
 public:
  using InputTableFromStatisticStore::InputTableFromStatisticStore;

  int numberOfSeries() const override { return 2; }

 protected:
  void computeParametersFromSeries(const InferenceModel* inference,
                                   int pageIndex) override;

  double preProcessTwoMeansParameter(double p, int index) const {
    if (index == Params::TwoMeans::N1 || index == Params::TwoMeans::N2) {
      return std::round(p);
    }
    return p;
  }

  double m_params[Poincare::Inference::NumberOfParameters(TestType::TwoMeans)];
};

}  // namespace Inference

#endif
