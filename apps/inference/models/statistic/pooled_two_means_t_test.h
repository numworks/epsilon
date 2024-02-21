#ifndef INFERENCE_MODELS_STATISTIC_POOLED_TWO_MEANS_TEST_H
#define INFERENCE_MODELS_STATISTIC_POOLED_TWO_MEANS_TEST_H

#include "two_means_t_test.h"

namespace Inference {

class PooledTwoMeansTTest : public TwoMeansTTest {
 public:
  DistributionType distributionType() const override {
    return DistributionType::TPooled;
  }
  I18n::Message title() const override { return TwoMeans::PooledTitle(); }
  void compute() override { TwoMeans::ComputePooledTest(this); }
};

}  // namespace Inference

#endif
