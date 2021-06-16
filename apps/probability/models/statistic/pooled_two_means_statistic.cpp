#include "pooled_two_means_statistic.h"

#include "math.h"

namespace Probability {

float PooledTwoMeansStatistic::_degreeOfFreedom(float s1, int n1, float s2, int n2) {
  return n1 + n2 - 2;
}

float PooledTwoMeansStatistic::_SE(float s1, int n1, float s2, int n2) {
  return _sp(s1, n1, s2, n2) * sqrtf(1. / n1  + 1. / n2);
}

float PooledTwoMeansStatistic::_sp(float s1, int n1, float s2, int n2) {
  return sqrtf(((n1 - 1) * s1 * s1 + (n2 - 1) * s2 * s2) / (n1 + n2 - 2));
}

} // namespace Probability
