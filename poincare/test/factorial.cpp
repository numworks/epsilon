#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_factorial_simplify) {
  assert_parsed_expression_simplify_to("1/3!", "1/6");
  assert_parsed_expression_simplify_to("5!", "120");
  assert_parsed_expression_simplify_to("(1/3)!", "undef");
  assert_parsed_expression_simplify_to("P!", "undef");
  assert_parsed_expression_simplify_to("X!", "undef");
}
