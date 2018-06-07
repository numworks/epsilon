#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_store_evaluate) {
  assert_parsed_expression_evaluates_to<float>("1+42>A", "43");
  assert_parsed_expression_evaluates_to<double>("0.123+I>B", "0.123+I");
}

QUIZ_CASE(poincare_store_simplify) {
  assert_parsed_expression_simplify_to("1+2>A", "A");
  assert_parsed_expression_simplify_to("1+2>x", "x");
}
