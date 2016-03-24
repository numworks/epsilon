#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_addition_approximate) {
  Context context;
  Expression * a = Expression::parse("1+2");
  assert(a->approximate(context) == 3.0f);
  delete a;
}
