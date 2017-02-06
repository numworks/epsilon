#include <quiz.h>
#include <poincare.h>
#include <math.h>
#include <assert.h>

QUIZ_CASE(poincare_parser) {
  GlobalContext globalContext;
  Expression * a = Expression::parse("1.2*e^(1)");
  float f =1.2f*M_E;
  assert(a->approximate(globalContext) == f);

  a = Expression::parse("e^2*e^(1)");
  f = powf(M_E, 2.0f)*M_E;
  assert(a->approximate(globalContext) == f);

  a = Expression::parse("2*3^4+2");
  f = 2.0f*powf(3.0f, 4.0f)+2.0f;
  assert(a->approximate(globalContext) == f);

  a = Expression::parse("-2*3^4+2");
  f = -2.0f*powf(3.0f, 4.0f)+2.0f;
  assert(a->approximate(globalContext) == f);

  a = Expression::parse("-sin(3)*2-3");
  f = -sinf(3.0f)*2.0f-3.0f;
  assert(a->approximate(globalContext) == f);
}