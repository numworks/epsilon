#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include <cmath>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_tobase_simplify) {
  assert_parsed_expression_simplify_to("123>:10", "123");
  assert_parsed_expression_simplify_to("123>:2", "1111011:2");
  assert_parsed_expression_simplify_to("123>:8", "173:8");
  assert_parsed_expression_simplify_to("123>:16", "7b:16");
  assert_parsed_expression_simplify_to("123>:3", "11120:3");
  assert_parsed_expression_simplify_to("123>:32", "3r:32)");
}
