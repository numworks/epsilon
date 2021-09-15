#ifndef PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_STATISTIC_H

#include "two_means_t_statistic.h"

namespace Probability {

class PooledTwoMeansStatistic : public TwoMeansTStatistic {
public:
  float computeDegreesOfFreedom(float s1, float n1, float s2, float n2) override;
  float computeStandardError(float s1, float n1, float s2, float n2) override;
  /* Pooled estimate of sigma */
  float computeSP(float s1, float n1, float s2, float n2);
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_STATISTIC_H */
