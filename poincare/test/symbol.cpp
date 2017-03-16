#include <quiz.h>
#include <poincare.h>
#include <math.h>
#include <ion.h>
#include <assert.h>

using namespace Poincare;

QUIZ_CASE(poincare_parse_symbol) {
  char piText[2] = {Ion::Charset::SmallPi, 0};
  Expression * e = Expression::parse(piText);
  assert(e->type() == Expression::Type::Symbol);
  delete e;

  char eText[2] = {Ion::Charset::Exponential, 0};
  e = Expression::parse(eText);
  assert(e->type() == Expression::Type::Symbol);
  delete e;

  char iText[10] = {Ion::Charset::IComplex, 0};
  e = Expression::parse(iText);
  assert(e->type() == Expression::Type::Complex);
  delete e;

  char floatText[10] = {'1', '.', '2', Ion::Charset::Exponent, '3', 0};
  e = Expression::parse(floatText);
  assert(e->type() == Expression::Type::Complex);
  delete e;

  e = Expression::parse("ans");
  assert(e->type() == Expression::Type::Symbol);
  delete e;
}


QUIZ_CASE(poincare_symbol_approximate) {
  GlobalContext globalContext;
  char piText[2] = {Ion::Charset::SmallPi, 0};
  Expression * e = Expression::parse(piText);
  assert(fabsf(e->approximate(globalContext)-M_PI) <= 0.0001f);
  delete e;

  char eText[2] = {Ion::Charset::Exponential, 0};
  e = Expression::parse(eText);
  assert(fabsf(e->approximate(globalContext)-M_E) <= 0.0001f);

  delete e;

  char floatText[10] = {'1', '.', '2', Ion::Charset::Exponent, '3', 0};
  e = Expression::parse(floatText);
  assert(fabsf(e->approximate(globalContext)-1200.0f) <= 0.0001f);
  delete e;
}

