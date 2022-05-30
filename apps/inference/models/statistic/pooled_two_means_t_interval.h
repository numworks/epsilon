#ifndef INFERENCE_MODELS_STATISTIC_POOLED_TWO_MEANS_T_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_POOLED_TWO_MEANS_T_INTERVAL_H

#include "two_means_t_interval.h"

namespace Inference {

class PooledTwoMeansTInterval : public TwoMeansTInterval {
public:
  DistributionType distributionType() const override { return DistributionType::TPooled; }
  I18n::Message title() const override { return PooledTwoMeans::Title(); }
  void compute() override { PooledTwoMeans::ComputeInterval(this); }
};

}  // namespace Inference

#endif /* INFERENCE_MODELS_STATISTIC_POOLED_TWO_MEANS_INTERVAL_H */
