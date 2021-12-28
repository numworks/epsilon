#ifndef APPS_PROBABILITY_TEST_TEST_HELPER_H
#define APPS_PROBABILITY_TEST_TEST_HELPER_H

#include <algorithm>
#include <cmath>
#include <poincare/float.h>
#include <quiz.h>
#include <stdio.h>

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
