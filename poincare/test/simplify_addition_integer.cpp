#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_simplify_addition_integer) {
  {
    Expression * e = Expression::parse("3+0");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::Integer);
  }
  {
    Expression * e = Expression::parse("3+foo+2");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::Addition);
  }
}
