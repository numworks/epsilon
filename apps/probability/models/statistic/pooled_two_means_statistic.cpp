#include "pooled_two_means_statistic.h"

namespace Probability {

double PooledTwoMeansStatistic::computeDegreesOfFreedom(double s1, double n1, double s2, double n2) {
  return n1 + n2 - 2;
}

double PooledTwoMeansStatistic::computeStandardError(double s1, double n1, double s2, double n2) {
  return computeSP(s1, n1, s2, n2) * std::sqrt(1. / n1 + 1. / n2);
}

double PooledTwoMeansStatistic::computeSP(double s1, double n1, double s2, double n2) {
  return std::sqrt(((n1 - 1) * s1 * s1 + (n2 - 1) * s2 * s2) / (n1 + n2 - 2));
}

}  // namespace Probability
