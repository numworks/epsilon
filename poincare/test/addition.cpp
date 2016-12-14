#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_addition_approximate) {
  GlobalContext globalContext;
  Expression * a = Expression::parse("1+2");
  assert(a->approximate(globalContext) == 3.0f);
  delete a;
}

QUIZ_CASE(poincare_addition_evaluate) {
  GlobalContext globalContext;
  Expression * a = Expression::parse("1+2");
  Expression * e = a->evaluate(globalContext);
  assert(e->approximate(globalContext) == 3.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]+3");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 4.0f);
  assert(e->operand(1)->approximate(globalContext) == 5.0f);
  assert(e->operand(2)->approximate(globalContext) == 6.0f);
  assert(e->operand(3)->approximate(globalContext) == 7.0f);
  assert(e->operand(4)->approximate(globalContext) == 8.0f);
  assert(e->operand(5)->approximate(globalContext) == 9.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]+[[1,2][3,4][5,6]]");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 2.0f);
  assert(e->operand(1)->approximate(globalContext) == 4.0f);
  assert(e->operand(2)->approximate(globalContext) == 6.0f);
  assert(e->operand(3)->approximate(globalContext) == 8.0f);
  assert(e->operand(4)->approximate(globalContext) == 10.0f);
  assert(e->operand(5)->approximate(globalContext) == 12.0f);
  delete a;
  delete e;
}
