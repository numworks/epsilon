#ifndef PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_STATISTIC_H

#include "two_means_t_statistic.h"

namespace Probability {

class PooledTwoMeansStatistic : public TwoMeansTStatistic {
public:
  double computeDegreesOfFreedom(double s1, double n1, double s2, double n2) override;
  double computeStandardError(double s1, double n1, double s2, double n2) override;
  /* Pooled estimate of sigma */
  double computeSP(double s1, double n1, double s2, double n2);
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_POOLED_TWO_MEANS_STATISTIC_H */
