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
  assert_parsed_expression_evaluates_to<double>("P", "3.1415926535898");
  assert_parsed_expression_evaluates_to<float>("X", "2.718282");
  assert_parsed_expression_evaluates_to<double>("1.2E3", "1200");
}

