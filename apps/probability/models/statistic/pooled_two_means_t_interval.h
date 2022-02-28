#ifndef PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_T_INTERVAL_H
#define PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_T_INTERVAL_H

#include "two_means_t_interval.h"

namespace Probability {

class PooledTwoMeansTInterval : public TwoMeansTInterval {
public:
  DistributionType distributionType() const override { return DistributionType::TPooled; }
  I18n::Message title() const override { return PooledTwoMeans::Title(); }
  void compute() override { PooledTwoMeans::ComputeInterval(this); }
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_INTERVAL_H */
