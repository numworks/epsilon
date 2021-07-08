#ifndef APPS_PROBABILITY_TEST_TEST_HELPER_H
#define APPS_PROBABILITY_TEST_TEST_HELPER_H

#include <quiz.h>
#include <cmath>

template <typename T>
bool inline roughlyEqual(T a, T b, T threshold, bool absolute) {
  if (absolute) {
    return std::fabs(a - b) < threshold;
  }
  T max = fmax(std::fabs(a), std::fabs(b));
  if (max == INFINITY) {
    return a == b;
  }
  T relerr = std::fabs(a - b) / max;
  return max == 0 || relerr < threshold;
}

template <typename T>
void inline assertRoughlyEqual(T a, T b, T threshold = FLT_EPSILON, bool absolute = false) {
  quiz_assert(roughlyEqual<T>(a, b, threshold, absolute));
}

struct TestCase {
  TestCase(double K, double X, double d, double p, float precision = 1e-7) :
      k(K), x(X), density(d), probability(p), precision(precision) {}
  double k;
  double x;
  double density;
  double probability;
  double precision;  // Lower precision for extreme case (x or k "big")
};

#endif /* APPS_PROBABILITY_TEST_TEST_HELPER_H */
