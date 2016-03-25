#include <quiz.h>
#include <poincare.h>
#include <assert.h>

QUIZ_CASE(poincare_parse_trigo) {
  {
    Expression * e = Expression::parse("sin(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::Sine);
  }
  {
    Expression * e = Expression::parse("cos(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::Cosine);
  }
  {
    Expression * e = Expression::parse("tan(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::Tangent);
  }
}
