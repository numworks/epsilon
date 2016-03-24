#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_subtraction_approximate) {
  Context context;
  Expression * s = Expression::parse("1-2");
  assert(s->approximate(context) == -1.0f);
  delete s;
}
