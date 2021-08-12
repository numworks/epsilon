#ifndef APPS_PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_STATISTIC_H

#include "two_means_t_statistic.h"

namespace Probability {

class PooledTwoMeansStatistic : public TwoMeansTStatistic {
public:
  float computeDegreesOfFreedom(float s1, int n1, float s2, int n2) override;
  float computeStandardError(float s1, int n1, float s2, int n2) override;
  /* Pooled estimate of sigma */
  float _sp(float s1, int n1, float s2, int n2);
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_STATISTIC_H */
