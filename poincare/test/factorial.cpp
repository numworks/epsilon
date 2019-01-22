#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_factorial_simplify) {
  assert_parsed_expression_simplify_to("1/3!", "1/6");
  assert_parsed_expression_simplify_to("5!", "120");
  assert_parsed_expression_simplify_to("(1/3)!", Undefined::Name());
  assert_parsed_expression_simplify_to("π!", Undefined::Name());
  assert_parsed_expression_simplify_to("ℯ!", Undefined::Name());
}
