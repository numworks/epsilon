#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_procuct_approximate) {
  GlobalContext globalContext;
  Expression * p = Expression::parse("1*2");
  assert(p->approximate(globalContext) == 2.0f);
  delete p;
}

QUIZ_CASE(poincare_product_evaluate) {
  GlobalContext globalContext;
  Expression * a = Expression::parse("1*2");
  Expression * e = a->evaluate(globalContext);
  assert(e->approximate(globalContext) == 2.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]*2");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 2.0f);
  assert(e->operand(1)->approximate(globalContext) == 4.0f);
  assert(e->operand(2)->approximate(globalContext) == 6.0f);
  assert(e->operand(3)->approximate(globalContext) == 8.0f);
  assert(e->operand(4)->approximate(globalContext) == 10.0f);
  assert(e->operand(5)->approximate(globalContext) == 12.0f);
  delete a;
  delete e;

  a = Expression::parse("3*[[1,2][3,4][5,6]]");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 3.0f);
  assert(e->operand(1)->approximate(globalContext) == 6.0f);
  assert(e->operand(2)->approximate(globalContext) == 9.0f);
  assert(e->operand(3)->approximate(globalContext) == 12.0f);
  assert(e->operand(4)->approximate(globalContext) == 15.0f);
  assert(e->operand(5)->approximate(globalContext) == 18.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]*[[1,2,3,4][5,6,7,8]]");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 11.0f);
  assert(e->operand(1)->approximate(globalContext) == 14.0f);
  assert(e->operand(2)->approximate(globalContext) == 17.0f);
  assert(e->operand(3)->approximate(globalContext) == 20.0f);
  assert(e->operand(4)->approximate(globalContext) == 23.0f);
  assert(e->operand(5)->approximate(globalContext) == 30.0f);
  assert(e->operand(6)->approximate(globalContext) == 37.0f);
  assert(e->operand(7)->approximate(globalContext) == 44.0f);
  assert(e->operand(8)->approximate(globalContext) == 35.0f);
  assert(e->operand(9)->approximate(globalContext) == 46.0f);
  assert(e->operand(10)->approximate(globalContext) == 57.0f);
  assert(e->operand(11)->approximate(globalContext) == 68.0f);
  delete a;
  delete e;
}
