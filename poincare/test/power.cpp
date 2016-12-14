#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_power_approximate) {
  GlobalContext globalContext;
  Expression * p = Expression::parse("2^3");
  assert(p->approximate(globalContext) == 8.0f);
  delete p;
}

QUIZ_CASE(poincare_power_evaluate) {
  GlobalContext globalContext;
  Expression * a = Expression::parse("2^3");
  Expression * e = a->evaluate(globalContext);
  assert(e->approximate(globalContext) == 8.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4]]^3");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 37.0f);
  assert(e->operand(1)->approximate(globalContext) == 54.0f);
  assert(e->operand(2)->approximate(globalContext) == 81.0f);
  assert(e->operand(3)->approximate(globalContext) == 118.0f);
  delete a;
  delete e;
}
