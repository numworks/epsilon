#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_fraction_approximate) {
  Context context;
  Expression * f = Expression::parse("1/2");
  assert(f->approximate(context) == 0.5f);
  delete f;
}

QUIZ_CASE(poincare_fraction_evaluate) {
  Context context;
  Expression * a = Expression::parse("1/2");
  Expression * e = a->evaluate(context);
  assert(e->approximate(context) == 0.5f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]/2");
  e = a->evaluate(context);
  assert(e->operand(0)->approximate(context) == 0.5f);
  assert(e->operand(1)->approximate(context) == 1.0f);
  assert(e->operand(2)->approximate(context) == 1.5f);
  assert(e->operand(3)->approximate(context) == 2.0f);
  assert(e->operand(4)->approximate(context) == 2.5f);
  assert(e->operand(5)->approximate(context) == 3.0f);
  delete a;
  delete e;
  // TODO: test matrice fraction when implemented
}
