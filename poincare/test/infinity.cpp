#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_infinity) {

  // 0 and infinity
  assert_parsed_expression_simplify_to("0/0", Undefined::Name());
  assert_parsed_expression_simplify_to("0/inf", "0");
  assert_parsed_expression_simplify_to("inf/0", Undefined::Name());
  assert_parsed_expression_simplify_to("0×inf", Undefined::Name());
  assert_parsed_expression_simplify_to("3×inf/inf", "undef");
  assert_parsed_expression_simplify_to("1ᴇ1000", "inf");
  assert_parsed_expression_simplify_to("-1ᴇ1000", "-inf");
  assert_parsed_expression_simplify_to("-1ᴇ-1000", "0");
  assert_parsed_expression_simplify_to("1ᴇ-1000", "0");
  assert_parsed_expression_evaluates_to<double>("1×10^1000", "inf");

  assert_parsed_expression_simplify_to("inf^0", "undef");
  assert_parsed_expression_simplify_to("1^inf", "1^inf");
  assert_parsed_expression_simplify_to("1^(X^inf)", "1^(X^inf)");
  assert_parsed_expression_simplify_to("inf^(-1)", "0");
  assert_parsed_expression_simplify_to("(-inf)^(-1)", "0");
  assert_parsed_expression_simplify_to("inf^(-√(2))", "0");
  assert_parsed_expression_simplify_to("(-inf)^(-√(2))", "0");
  assert_parsed_expression_simplify_to("inf^2", "inf");
  assert_parsed_expression_simplify_to("(-inf)^2", "inf");
  assert_parsed_expression_simplify_to("inf^√(2)", "inf");
  assert_parsed_expression_simplify_to("(-inf)^√(2)", "inf×(-1)^√(2)");
  assert_parsed_expression_simplify_to("inf^x", "inf^x");
  assert_parsed_expression_simplify_to("1/inf+24", "24");
  assert_parsed_expression_simplify_to("ℯ^(inf)/inf", "0×ℯ^inf");

  // Logarithm
  assert_parsed_expression_simplify_to("log(inf,0)", "undef");
  assert_parsed_expression_simplify_to("log(inf,1)", "undef");
  assert_parsed_expression_simplify_to("log(0,inf)", "undef");
  assert_parsed_expression_simplify_to("log(1,inf)", "0");
  assert_parsed_expression_simplify_to("log(inf,inf)", "undef");

  assert_parsed_expression_simplify_to("ln(inf)", "inf");
  assert_parsed_expression_simplify_to("log(inf,-3)", "log(inf,-3)");
  assert_parsed_expression_simplify_to("log(inf,3)", "inf");
  assert_parsed_expression_simplify_to("log(inf,0.3)", "-inf");
  assert_parsed_expression_simplify_to("log(inf,x)", "log(inf,x)");
  assert_parsed_expression_simplify_to("ln(inf)*0", "undef");

}
