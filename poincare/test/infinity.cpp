#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"
#if POINCARE_TESTS_PRINT_EXPRESSIONS
#include "../src/expression_debug.h"
#include <iostream>
using namespace std;
#endif

using namespace Poincare;

QUIZ_CASE(poincare_infinity) {

  // 0 and infinity
  assert_parsed_expression_simplify_to("0/0", Undefined::Name());
  assert_parsed_expression_simplify_to("0/inf", "0");
  assert_parsed_expression_simplify_to("inf/0", Undefined::Name());
  assert_parsed_expression_simplify_to("0*inf", Undefined::Name());
  assert_parsed_expression_simplify_to("3*inf/inf", "undef");
  assert_parsed_expression_simplify_to("1E1000", "inf");
  assert_parsed_expression_simplify_to("-1E1000", "-inf");
  assert_parsed_expression_simplify_to("-1E-1000", "0");
  assert_parsed_expression_simplify_to("1E-1000", "0");
  assert_parsed_expression_evaluates_to<double>("1*10^1000", "inf");

  assert_parsed_expression_simplify_to("inf^(-1)", "0");
  assert_parsed_expression_simplify_to("(-inf)^(-1)", "0");
  assert_parsed_expression_simplify_to("inf^(-R(2))", "0");
  assert_parsed_expression_simplify_to("(-inf)^(-R(2))", "0");
  assert_parsed_expression_simplify_to("inf^2", "inf");
  assert_parsed_expression_simplify_to("(-inf)^2", "inf");
  assert_parsed_expression_simplify_to("inf^R(2)", "inf");
  assert_parsed_expression_simplify_to("(-inf)^R(2)", "inf*(-1)^R(2)");
  assert_parsed_expression_simplify_to("inf^x", "inf^x");
  assert_parsed_expression_simplify_to("1/inf+24", "24");

}
