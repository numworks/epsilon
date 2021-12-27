#ifndef APPS_PROBABILITY_TEST_TEST_HELPER_H
#define APPS_PROBABILITY_TEST_TEST_HELPER_H

#include <algorithm>
#include <cmath>
#include <poincare/float.h>
#include <quiz.h>
#include <stdio.h>

template <typename T>
bool inline roughlyEqual(T a, T b, T threshold, bool absolute = false) {
  if (absolute) {
    return std::fabs(a - b) < threshold;
  }
  T max = std::max(std::fabs(a), std::fabs(b));
  if (max == INFINITY) {
    return a == b;
  }
  T relerr = std::fabs(a - b) / max;
  bool res = max == 0 || relerr < threshold;
  return res;
}

template <typename T>
void inline assertRoughlyEqual(T a, T b, T threshold = Poincare::Float<T>::epsilon(), bool absolute = false) {
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
