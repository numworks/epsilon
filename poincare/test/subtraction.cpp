#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_subtraction_approximate) {
  GlobalContext globalContext;
  Expression * s = Expression::parse("1-2");
  assert(s->approximate(globalContext) == -1.0f);
  delete s;
}

QUIZ_CASE(poincare_substraction_evaluate) {
  GlobalContext globalContext;
  Expression * a = Expression::parse("1-2");
  Expression * e = a->evaluate(globalContext);
  assert(e->approximate(globalContext) == -1.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]-3");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == -2.0f);
  assert(e->operand(1)->approximate(globalContext) == -1.0f);
  assert(e->operand(2)->approximate(globalContext) == 0.0f);
  assert(e->operand(3)->approximate(globalContext) == 1.0f);
  assert(e->operand(4)->approximate(globalContext) == 2.0f);
  assert(e->operand(5)->approximate(globalContext) == 3.0f);
  delete a;
  delete e;

  a = Expression::parse("3-[[1,2][3,4][5,6]]");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 2.0f);
  assert(e->operand(1)->approximate(globalContext) == 1.0f);
  assert(e->operand(2)->approximate(globalContext) == 0.0f);
  assert(e->operand(3)->approximate(globalContext) == -1.0f);
  assert(e->operand(4)->approximate(globalContext) == -2.0f);
  assert(e->operand(5)->approximate(globalContext) == -3.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]-[[6,5][4,3][2,1]]");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == -5.0f);
  assert(e->operand(1)->approximate(globalContext) == -3.0f);
  assert(e->operand(2)->approximate(globalContext) == -1.0f);
  assert(e->operand(3)->approximate(globalContext) == 1.0f);
  assert(e->operand(4)->approximate(globalContext) == 3.0f);
  assert(e->operand(5)->approximate(globalContext) == 5.0f);
  delete a;
  delete e;
}
