#ifndef INFERENCE_MODELS_STATISTIC_POOLED_TWO_MEANS_TEST_H
#define INFERENCE_MODELS_STATISTIC_POOLED_TWO_MEANS_TEST_H

#include "interfaces/distributions.h"
#include "interfaces/significance_tests.h"
#include "two_means_t_test.h"

namespace Inference {

class PooledTwoMeansTTest : public TwoMeansTTest {
public:
  DistributionType distributionType() const override { return DistributionType::TPooled; }
  I18n::Message title() const override { return PooledTwoMeans::Title(); }
  void compute() override { PooledTwoMeans::ComputeTest(this); }
};

}  // namespace Inference

#endif /* INFERENCE_MODELS_STATISTIC_POOLED_TWO_MEANS_TEST_H */
