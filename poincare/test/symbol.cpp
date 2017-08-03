#include <quiz.h>
#include <poincare.h>
#include <math.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

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
  Complex a[1] = {Complex::Float(M_PI)};
  assert_parsed_expression_evaluate_to("P", a, 1);
  Complex b[1] = {Complex::Float(M_E)};
  assert_parsed_expression_evaluate_to("X", b, 1);
  Complex c[1] = {Complex::Float(1200.0f)};
  assert_parsed_expression_evaluate_to("1.2E3", c, 1);
}

