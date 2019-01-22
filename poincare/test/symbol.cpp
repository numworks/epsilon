#include <quiz.h>
#include <poincare/expression.h>
#include <cmath>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_parse_symbol) {
  assert_parsed_expression_type("π", ExpressionNode::Type::Constant);
  assert_parsed_expression_type("ℯ", ExpressionNode::Type::Constant);
  assert_parsed_expression_type("𝐢", ExpressionNode::Type::Constant);
  assert_parsed_expression_type("1.2ᴇ3", ExpressionNode::Type::Decimal);
  assert_parsed_expression_type("ans", ExpressionNode::Type::Symbol);
}


QUIZ_CASE(poincare_symbol_approximate) {
  assert_parsed_expression_evaluates_to<double>("π", "3.1415926535898");
  assert_parsed_expression_evaluates_to<float>("ℯ", "2.718282");
  assert_parsed_expression_evaluates_to<double>("1.2ᴇ3", "1200");
}

