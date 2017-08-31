#include <quiz.h>
#include <poincare.h>
#include <cmath>
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
  Complex<double> a[1] = {Complex<double>::Float(M_PI)};
  assert_parsed_expression_evaluates_to("P", a);
  Complex<float> b[1] = {Complex<float>::Float(M_E)};
  assert_parsed_expression_evaluates_to("X", b);
  Complex<double> c[1] = {Complex<double>::Float(1200.0)};
  assert_parsed_expression_evaluates_to("1.2E3", c);
}

