#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_fraction_approximate) {
  Context context;
  // Note: these have to be heap allocated.
  Integer *a = new Integer(1);
  Integer *b = new Integer(2);
  assert(Fraction(a, b).approximate(context) == 0.5f);
}

