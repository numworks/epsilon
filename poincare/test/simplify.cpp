#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_simplify) {
  {
    Expression * e = Expression::parse((char *)"3*0");
    Expression * e2 = e->simplify();
    assert(e2->type() == Integer::Type);
  }
  {
    Expression * e = Expression::parse((char *)"0*foo");
    Expression * e2 = e->simplify();
    assert(e2->type() == Integer::Type);
  }
}
