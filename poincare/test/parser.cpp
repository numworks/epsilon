#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <math.h>
#include <assert.h>

using namespace Poincare;

QUIZ_CASE(poincare_parser) {
  GlobalContext globalContext;
  char text0[10] = {'1', '.', '2', '*', Ion::Charset::Exponential, '^', '(', '1', ')', 0};
  Expression * a = Expression::parse(text0);
  float f =1.2f*M_E;
  assert(a->approximate(globalContext) == f);

  char text1[10] = {Ion::Charset::Exponential, '^', '2', '*', Ion::Charset::Exponential, '^', '(', '1', ')', 0};
  a = Expression::parse(text1);
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

  a = Expression::parse("-.003");
  f = -0.003f;
  assert(a->approximate(globalContext) == f);

  char text2[10] = {'.', '0', '2', Ion::Charset::Exponent, '2', 0};
  a = Expression::parse(text2);
  f = 2.0f;
  assert(a->approximate(globalContext) == f);
}