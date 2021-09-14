#include "pooled_two_means_statistic.h"

namespace Probability {

float PooledTwoMeansStatistic::computeDegreesOfFreedom(float s1, float n1, float s2, float n2) {
  return n1 + n2 - 2;
}

float PooledTwoMeansStatistic::computeStandardError(float s1, float n1, float s2, float n2) {
  return computeSP(s1, n1, s2, n2) * std::sqrt(1. / n1 + 1. / n2);
}

float PooledTwoMeansStatistic::computeSP(float s1, float n1, float s2, float n2) {
  return std::sqrt(((n1 - 1) * s1 * s1 + (n2 - 1) * s2 * s2) / (n1 + n2 - 2));
}

}  // namespace Probability
