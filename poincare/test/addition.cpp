#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_addition_approximate) {
  Context context;
  // Note: these have to be heap allocated.
  Integer *a = new Integer(1);
  Integer *b = new Integer(2);
  assert(Addition(a, b).approximate(context) == 3.0f);
}

