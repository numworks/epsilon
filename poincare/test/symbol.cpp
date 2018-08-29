#include <quiz.h>
#include <poincare.h>
#include <cmath>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_parse_symbol) {
  assert_parsed_expression_type("P", Expression::Type::Symbol);
  assert_parsed_expression_type("X", Expression::Type::Symbol);
  assert_parsed_expression_type("I", Expression::Type::Symbol);
  assert_parsed_expression_type("1.2E3", Expression::Type::Decimal);
  assert_parsed_expression_type("ans", Expression::Type::Symbol);
}


QUIZ_CASE(poincare_symbol_approximate) {
  Complex<double> a[1] = {Complex<double>::Float(M_PI)};
  assert_parsed_expression_evaluates_to("P", a);
  Complex<float> b[1] = {Complex<float>::Float(M_E)};
  assert_parsed_expression_evaluates_to("X", b);
  Complex<double> c[1] = {Complex<double>::Float(1200.0)};
  assert_parsed_expression_evaluates_to("1.2E3", c);
}

