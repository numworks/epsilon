#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_fraction_approximate) {
  GlobalContext globalContext;
  Expression * f = Expression::parse("1/2");
  assert(f->approximate(globalContext) == 0.5f);
  delete f;
}

QUIZ_CASE(poincare_fraction_evaluate) {
  GlobalContext globalContext;
  Expression * a = Expression::parse("1/2");
  Expression * e = a->evaluate(globalContext);
  assert(e->approximate(globalContext) == 0.5f);
  delete a;
  delete e;

  a = Expression::parse("[[1,2][3,4][5,6]]/2");
  e = a->evaluate(globalContext);
  assert(e->operand(0)->approximate(globalContext) == 0.5f);
  assert(e->operand(1)->approximate(globalContext) == 1.0f);
  assert(e->operand(2)->approximate(globalContext) == 1.5f);
  assert(e->operand(3)->approximate(globalContext) == 2.0f);
  assert(e->operand(4)->approximate(globalContext) == 2.5f);
  assert(e->operand(5)->approximate(globalContext) == 3.0f);
  delete a;
  delete e;
  // TODO: test matrice fraction when implemented
}
