#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_fraction_approximate) {
  Context context;
  Expression * f = Expression::parse("1/2");
  assert(f->approximate(context) == 0.5f);
  delete f;
}

