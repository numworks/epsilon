#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_procuct_approximate) {
  Context context;
  Expression * p = Expression::parse("1*2");
  assert(p->approximate(context) == 2.0f);
  delete p;
}

QUIZ_CASE(poincare_product_evaluate) {
  Context context;
  Expression * a = Expression::parse("1*2");
  Expression * e = a->evaluate(context);
  assert(e->approximate(context) == 2.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]*2");
  e = a->evaluate(context);
  assert(e->operand(0)->approximate(context) == 2.0f);
  assert(e->operand(1)->approximate(context) == 4.0f);
  assert(e->operand(2)->approximate(context) == 6.0f);
  assert(e->operand(3)->approximate(context) == 8.0f);
  assert(e->operand(4)->approximate(context) == 10.0f);
  assert(e->operand(5)->approximate(context) == 12.0f);
  delete a;
  delete e;

  a = Expression::parse("3*[[1,2][3,4][5,6]]");
  e = a->evaluate(context);
  assert(e->operand(0)->approximate(context) == 3.0f);
  assert(e->operand(1)->approximate(context) == 6.0f);
  assert(e->operand(2)->approximate(context) == 9.0f);
  assert(e->operand(3)->approximate(context) == 12.0f);
  assert(e->operand(4)->approximate(context) == 15.0f);
  assert(e->operand(5)->approximate(context) == 18.0f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]*[[1,2,3,4][5,6,7,8]]");
  e = a->evaluate(context);
  assert(e->operand(0)->approximate(context) == 11.0f);
  assert(e->operand(1)->approximate(context) == 14.0f);
  assert(e->operand(2)->approximate(context) == 17.0f);
  assert(e->operand(3)->approximate(context) == 20.0f);
  assert(e->operand(4)->approximate(context) == 23.0f);
  assert(e->operand(5)->approximate(context) == 30.0f);
  assert(e->operand(6)->approximate(context) == 37.0f);
  assert(e->operand(7)->approximate(context) == 44.0f);
  assert(e->operand(8)->approximate(context) == 35.0f);
  assert(e->operand(9)->approximate(context) == 46.0f);
  assert(e->operand(10)->approximate(context) == 57.0f);
  assert(e->operand(11)->approximate(context) == 68.0f);
  delete a;
  delete e;
}
