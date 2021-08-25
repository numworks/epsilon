#ifndef APPS_PROBABILITY_TEST_TEST_HELPER_H
#define APPS_PROBABILITY_TEST_TEST_HELPER_H

#include <quiz.h>
#include <stdio.h>

#include <algorithm>
#include <cmath>

template <typename T>
bool inline roughlyEqual(T a, T b, T threshold = 1e-9, bool absolute = false) {
  if (absolute) {
    return std::fabs(a - b) < threshold;
  }
  T max = std::max(std::fabs(a), std::fabs(b));
  if (max == INFINITY) {
    return a == b;
  }
  T relerr = std::fabs(a - b) / max;
  bool res = max == 0 || relerr < threshold;
  if (!res) {
    printf("Roughly equal : %.10f vs %.10f ", a, b);
  }
  return res;
}

template <typename T>
void inline assertRoughlyEqual(T a, T b, T threshold = FLT_EPSILON, bool absolute = false) {
  quiz_assert(roughlyEqual<T>(a, b, threshold, absolute));
}

struct LawTestCase {
  LawTestCase(double K, double X, double d, double p, float precision = 1e-7) :
      k(K), x(X), density(d), probability(p), precision(precision) {}
  double k;
  double x;
  double density;
  double probability;
  double precision;  // Lower precision for extreme case (x or k "big")
};

#endif /* APPS_PROBABILITY_TEST_TEST_HELPER_H */
