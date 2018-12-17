#include <quiz.h>
#include <poincare/expression.h>
#include <cmath>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_parse_symbol) {
  assert_parsed_expression_type("P", ExpressionNode::Type::Constant);
  assert_parsed_expression_type("X", ExpressionNode::Type::Constant);
  assert_parsed_expression_type("I", ExpressionNode::Type::Constant);
  assert_parsed_expression_type("1.2E3", ExpressionNode::Type::Decimal);
  assert_parsed_expression_type("ans", ExpressionNode::Type::Symbol);
}


QUIZ_CASE(poincare_symbol_approximate) {
  assert_parsed_expression_evaluates_to<double>("P", "3.1415926535898");
  assert_parsed_expression_evaluates_to<float>("X", "2.718282");
  assert_parsed_expression_evaluates_to<double>("1.2E3", "1200");
}

