#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_power_approximate) {
  Context context;
  Expression * p = Expression::parse("2^3");
  assert(p->approximate(context) == 8.0f);
  delete p;
}

QUIZ_CASE(poincare_power_evaluate) {
  Context context;
  Expression * a = Expression::parse("2^3");
  Expression * e = a->evaluate(context);
  assert(e->approximate(context) == 8.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4]]^3");
  e = a->evaluate(context);
  assert(e->operand(0)->approximate(context) == 37.0f);
  assert(e->operand(1)->approximate(context) == 54.0f);
  assert(e->operand(2)->approximate(context) == 81.0f);
  assert(e->operand(3)->approximate(context) == 118.0f);
  delete a;
  delete e;
}
