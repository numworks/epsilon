#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_addition_approximate) {
  Context context;
  Expression * a = Expression::parse("1+2");
  assert(a->approximate(context) == 3.0f);
  delete a;
}

QUIZ_CASE(poincare_addition_evaluate) {
  Context context;
  Expression * a = Expression::parse("1+2");
  Expression * e = a->evaluate(context);
  assert(e->approximate(context) == 3.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]+3");
  e = a->evaluate(context);
  assert(e->operand(0)->approximate(context) == 4.0f);
  assert(e->operand(1)->approximate(context) == 5.0f);
  assert(e->operand(2)->approximate(context) == 6.0f);
  assert(e->operand(3)->approximate(context) == 7.0f);
  assert(e->operand(4)->approximate(context) == 8.0f);
  assert(e->operand(5)->approximate(context) == 9.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]+[[1,2][3,4][5,6]]");
  e = a->evaluate(context);
  assert(e->operand(0)->approximate(context) == 2.0f);
  assert(e->operand(1)->approximate(context) == 4.0f);
  assert(e->operand(2)->approximate(context) == 6.0f);
  assert(e->operand(3)->approximate(context) == 8.0f);
  assert(e->operand(4)->approximate(context) == 10.0f);
  assert(e->operand(5)->approximate(context) == 12.0f);
  delete a;
  delete e;
}
