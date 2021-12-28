#include <quiz.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <cmath>
#include <poincare/test/helper.h>
#include "probability/models/distribution/student_distribution.h"
#include "probability/models/distribution/hypergeometric_function.h"

void assert_hypergeometric_is(double a, double b, double c, double z, double result) {
  double r = 0.0;
  const double precision = FLT_EPSILON;
  quiz_assert(hypergeometricFunction(a, b, c, z, precision, 1000, &r));
  assert_roughly_equal(r, result, 100 * precision); // Multiply by 100 because precision is too strict
}

QUIZ_CASE(probability_hypergeometric_function) {
  assert_hypergeometric_is(1.0, 2.0, 3.0, 0.5, 1.545177444479562475337856971665412544604001074882042032965);
  assert_hypergeometric_is(0.5, 0.6, 0.9, 0.2, 1.076590925287316818209663064430201685162648800967441005164);
  assert_hypergeometric_is(1.4, 0.72, 3.56, 0.9, 1.496490448634238403792101320605116555747748144140465947806);
  assert_hypergeometric_is(10.0, 0.2, 13.3, 0.12, 1.019119266590223428068941990750518046484840980662175905693);
  assert_hypergeometric_is(0.1, 0.2, 56.0, 0.21, 1.000075183394368041565539149267931545331418014779591403758);
  assert_hypergeometric_is(33.0, 0.3, 0.5678, 0.765, 85766460438444348287.386477193902261907533508044855870);
}

