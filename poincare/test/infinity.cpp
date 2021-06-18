#include "helper.h"

QUIZ_CASE(poincare_infinity_symbol) {
  assert_parsed_expression_simplify_to("inf", "∞");
  assert_parsed_expression_simplify_to("oo", "∞");
  assert_parsed_expression_simplify_to("∞", "∞");
}
