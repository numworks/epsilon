#include <poincare/symbol_context.h>
#include <poincare/test/helpers/symbol_store.h>

#include "../approximation//helper.h"
#include "../helper.h"
#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

QUIZ_CASE(poincare_simplification_infinity) {
  // 0 and infinity
  assert_parsed_expression_simplify_to("0/0", "undef");
  assert_parsed_expression_simplify_to("0/inf", "0");
  assert_parsed_expression_simplify_to("inf/0", "undef");
  assert_parsed_expression_simplify_to("0×inf", "undef");
  assert_parsed_expression_simplify_to("0×inf×π", "undef");
  assert_parsed_expression_simplify_to("3×inf/inf", "undef");
  assert_parsed_expression_simplify_to("1ᴇ1001", "∞");
  assert_parsed_expression_simplify_to("-1ᴇ1001", "-∞");
  assert_parsed_expression_simplify_to("-1ᴇ-1001", "0");
  assert_parsed_expression_simplify_to("1ᴇ-1001", "0");
  // assert_parsed_expression_simplify_to("1×10^1000", "∞");

  // Power
  assert_parsed_expression_simplify_to("0^inf", "0");
  assert_parsed_expression_simplify_to("0^(-inf)", "undef");
  assert_parsed_expression_simplify_to("1^inf", "undef");
  assert_parsed_expression_simplify_to("1^(-inf)", "undef");
  assert_parsed_expression_simplify_to("(-1)^inf", "undef");
  assert_parsed_expression_simplify_to("(-1)^(-inf)", "undef");
  assert_parsed_expression_simplify_to("2^inf", "∞");
  simplified_approximates_to<float>("2^inf", "∞");
  assert_parsed_expression_simplify_to("2^(-inf)", "0");
  simplified_approximates_to<float>("2^(-inf)", "0");
  assert_parsed_expression_simplify_to("(-2)^inf", "(-2)^∞");
  simplified_approximates_to<float>("(-2)^inf", "undef");
  assert_parsed_expression_simplify_to("(-2)^(-inf)",
                                       "(-2)^\U00000012-∞\U00000013");
#if TODO_PCJ
  simplified_approximates_to<float>("(-2)^(-inf)", "0");
#endif
  assert_parsed_expression_simplify_to("0.2^inf", "0");
  simplified_approximates_to<float>("0.2^inf", "0");
  assert_parsed_expression_simplify_to("0.2^(-inf)", "∞");
  simplified_approximates_to<float>("0.2^(-inf)", "∞");
  assert_parsed_expression_simplify_to("(-0.2)^inf", "(-1/5)^∞");
#if TODO_PCJ
  simplified_approximates_to<float>("(-0.2)^inf", "0");
#endif
  assert_parsed_expression_simplify_to("(-0.2)^(-inf)",
                                       "(-1/5)^\U00000012-∞\U00000013");
  simplified_approximates_to<float>("(-0.2)^(-inf)", "undef");
  assert_parsed_expression_simplify_to("i^inf", "undef");
  assert_parsed_expression_simplify_to("i^(-inf)", "undef");
  assert_parsed_expression_simplify_to("(3+4i)^inf", "undef");
  assert_parsed_expression_simplify_to("(3+4i)^(-inf)", "undef");
  assert_parsed_expression_simplify_to("inf^0", "undef");
  assert_parsed_expression_simplify_to("1^(X^inf)", "1^(X^∞)");
  assert_parsed_expression_simplify_to("inf^(-1)", "0");
  assert_parsed_expression_simplify_to("(-inf)^(-1)", "0");
  assert_parsed_expression_simplify_to("inf^(-√(2))", "0");
  assert_parsed_expression_simplify_to("(-inf)^(-√(2))", "0");
  assert_parsed_expression_simplify_to("inf^2", "∞");
  assert_parsed_expression_simplify_to("(-inf)^2", "∞");
  assert_parsed_expression_simplify_to("inf^√(2)", "∞");
  assert_parsed_expression_simplify_to("(-inf)^√(2)", "∞×(-1)^√(2)");
  assert_parsed_expression_simplify_to("inf^x", "∞^x");
  assert_parsed_expression_simplify_to("1/inf+24", "24");
  assert_parsed_expression_simplify_to("e^(inf)/inf", "0×e^(∞)");

  // Logarithm
  assert_parsed_expression_simplify_to("log(inf,0)", "undef");
  assert_parsed_expression_simplify_to("log(inf,1)", "undef");
  assert_parsed_expression_simplify_to("log(0,inf)", "undef");
  assert_parsed_expression_simplify_to("log(1,inf)", "0");
  assert_parsed_expression_simplify_to("log(inf,inf)", "undef");

  assert_parsed_expression_simplify_to("ln(inf)", "∞");
  assert_parsed_expression_simplify_to("log(inf,-3)", "log(∞,-3)");
  assert_parsed_expression_simplify_to("log(inf,3)", "∞");
  assert_parsed_expression_simplify_to("log(inf,0.3)", "-∞");
  assert_parsed_expression_simplify_to("log(inf,x)", "log(∞,x)");
  assert_parsed_expression_simplify_to("ln(inf)*0", "undef");
}

QUIZ_CASE(poincare_simplification_mix) {
  // TODO_PCJ: broken on the CI but can't reproduce on bob with clang16
  // assert_parsed_expression_simplify_to(
  // "(√(2)+√(2)×i)/2(√(2)+√(2)×i)/2(√(2)+√(2)×i)/2", "√(2)/32-√(2)/32×i");
}
