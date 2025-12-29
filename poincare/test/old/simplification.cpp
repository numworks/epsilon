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

QUIZ_CASE(poincare_simplification_logarithm) {
  assert_parsed_expression_simplify_to("log(0,0)", "undef");
  assert_parsed_expression_simplify_to("log(0,1)", "undef");
  assert_parsed_expression_simplify_to("log(1,0)", "undef");
  assert_parsed_expression_simplify_to("log(2,0)", "undef");
  assert_parsed_expression_simplify_to("log(0,14)", "undef");
  assert_parsed_expression_simplify_to("log(0,0.14)", "undef");
  assert_parsed_expression_simplify_to("log(0,0.14+i)", "undef");
  assert_parsed_expression_simplify_to("log(2,1)", "undef");
  assert_parsed_expression_simplify_to("log(x,1)", "undef");
  assert_parsed_expression_simplify_to("log(12925)",
                                       "log(47)+log(11)+2×log(5)");
  assert_parsed_expression_simplify_to("ln(12925)", "ln(47)+ln(11)+2×ln(5)");
  assert_parsed_expression_simplify_to(
      "log(1742279/12925, 6)",
      "-log(47,6)+log(17,6)+3×log(11,6)+log(7,6)-2×log(5,6)");
  assert_parsed_expression_simplify_to("ln(2/3)", "-ln(3)+ln(2)");
  assert_parsed_expression_simplify_to("log(1742279/12925, -6)",
                                       "log(158389/1175,-6)");
  assert_parsed_expression_simplify_to("ln(√(2))", "ln(2)/2");
  assert_parsed_expression_simplify_to("ln(e^3)", "3");
  assert_parsed_expression_simplify_to("log(10)", "1");
  assert_parsed_expression_simplify_to("log(√(3),√(3))", "1");
  assert_parsed_expression_simplify_to("log(1/√(2))", "-log(2)/2");
  assert_parsed_expression_simplify_to("log(-i)", "log(-i)");
  assert_parsed_expression_simplify_to("ln(e^(iπ/7))", "π/7×i");
  assert_parsed_expression_simplify_to("log(10^24)", "24");
  assert_parsed_expression_simplify_to("log((23π)^4,23π)", "4");
  assert_parsed_expression_simplify_to("log(10^(2+π))", "π+2");
  assert_parsed_expression_simplify_to(
      "ln(1881676377434183981909562699940347954480361860897069)",
      "ln(1.8816763774342ᴇ51)");
  /* log(1002101470343) does no reduce to 3×log(10007) because it involves
   * prime factors above k_biggestPrimeFactor */
  assert_parsed_expression_simplify_to("log(1002101470343)",
                                       "log(1002101470343)");
  assert_parsed_expression_simplify_to("log(64,2)", "6");
  assert_parsed_expression_simplify_to("log(2,64)", "1/6");
  assert_parsed_expression_simplify_to("log(1476225,5)", "10×log(3,5)+2");

  assert_parsed_expression_simplify_to("log(100)", "2");
  assert_parsed_expression_simplify_to("log(1000000)", "6");
  assert_parsed_expression_simplify_to("log(70992768,14)",
                                       "log(11,14)+log(3,14)+2×log(2,14)+5");
  assert_parsed_expression_simplify_to("log(1/6991712,14)", "-log(13,14)-5");
  assert_parsed_expression_simplify_to("log(4,10)", "2×log(2)");
}

QUIZ_CASE(poincare_simplification_mix) {
  // Root at denominator
  assert_parsed_expression_simplify_to("1/(√(2)+√(3))", "√(3)-√(2)");
  assert_parsed_expression_simplify_to("1/(5+√(3))", "(5-√(3))/22");
  assert_parsed_expression_simplify_to("1/(√(2)+4)", "(4-√(2))/14");
  assert_parsed_expression_simplify_to("1/(2√(2)-4)", "(-2-√(2))/4");
  assert_parsed_expression_simplify_to("1/(-2√(2)+4)", "(2+√(2))/4");
  assert_parsed_expression_simplify_to("45^2", "2025");
  assert_parsed_expression_simplify_to("1/(√(2)ln(3))", "√(2)/(2×ln(3))");
  assert_parsed_expression_simplify_to("√(3/2)", "√(6)/2");

  // Common operation mix
  assert_parsed_expression_simplify_to("(√(2)×π + √(2)×e)/√(2)", "e+π");
  assert_parsed_expression_simplify_to("π+(3√(2)-2√(3))/25",
                                       "(25×π-2×√(3)+3×√(2))/25");
  assert_parsed_expression_simplify_to("ln(2+3)", "ln(5)");
  assert_parsed_expression_simplify_to(
      "3×P×Q×R+4×cos(2)-2×P×Q×R+P×Q×R+ln(3)+4×P×Q-5×P×Q×R+cos(3)×ln(5)+cos(2)-"
      "45×cos(2)",
      "-3×P×Q×R+4×P×Q+cos(3)×ln(5)+ln(3)-40×cos(2)");
  assert_parsed_expression_simplify_to("2×P+3×cos(2)+3+4×ln(5)+5×P+2×ln(5)+1+0",
                                       "7×P+4+3×cos(2)+6×ln(5)");
  assert_parsed_expression_simplify_to(
      "2.3×P+3×cos(2)+3+4×ln(5)+5×P+2×ln(5)+1.2+0.235",
      "(1460×P+1200×ln(5)+600×cos(2)+887)/200");
  assert_parsed_expression_simplify_to("2×P×Q×R+2.3×P×Q+3×P^2+5.2×P×Q×R+4×P^2",
                                       "(72×P×Q×R+70×P^2+23×P×Q)/10");
  assert_parsed_expression_simplify_to("(P×Q)^2×P+4×P^3", "P^3×Q^2+4×P^3");
  assert_parsed_expression_simplify_to("(P×3)^2×P+4×P^3", "13×P^3");
  assert_parsed_expression_simplify_to("P^2^2×P+4×P^3", "P^5+4×P^3");
  assert_parsed_expression_simplify_to(
      "0.5+4×P×Q-2.3+2×P×Q-2×Q×P^R-cos(4)+2×P^R×Q+P×Q×R×X",
      "dep((5×P×Q×R×X+30×P×Q-5×cos(4)-9)/5,{P^R})");
  assert_parsed_expression_simplify_to("(1+√(2))/5", "(1+√(2))/5");
  assert_parsed_expression_simplify_to("(2+√(6))^2", "10+4×√(6)");
  assert_parsed_expression_simplify_to("tan(3)ln(2)+π", "tan(3)×ln(2)+π");
  assert_parsed_expression_simplify_to("abs(dim({4, 5}))", "2");

  // Romplex
  assert_parsed_expression_simplify_to("i", "i");
  assert_parsed_expression_simplify_to("√(-33)", "√(33)×i");
  assert_parsed_expression_simplify_to("i^(3/5)",
                                       "(√(2)×√(5-√(5)))/4+(1+√(5))/4×i");
  assert_parsed_expression_simplify_to("iiii", "1");
  assert_parsed_expression_simplify_to("√(-i)", "√(2)/2-√(2)/2×i");
  assert_parsed_expression_simplify_to("P×cos(9)iiln(2)", "-P×cos(9)×ln(2)");
  // TODO_PCJ: broken on the CI but can't reproduce on bob with clang16
  // assert_parsed_expression_simplify_to(
  // "(√(2)+√(2)×i)/2(√(2)+√(2)×i)/2(√(2)+√(2)×i)/2", "√(2)/32-√(2)/32×i");
  assert_parsed_expression_simplify_to("root(5^((-i)3^9),i)",
                                       "e^\x12-19683×ln(5)+10084×π\x13");
  assert_parsed_expression_simplify_to("i^i", "1/e^(π/2)");
  assert_parsed_expression_simplify_to("i/(1+i×√(x))", "i/(√(x)×i+1)");
  assert_parsed_expression_simplify_to("x+i/(1+i×√(x))",
                                       "(x^(3/2)×i+x+i)/(√(x)×i+1)");
  assert_parsed_expression_simplify_to("√(√(√(√(√(√(i))))))",
                                       "√(√(√(√(√(√(i))))))");

  // TODO: for this to work, we must know the sign of cos(9)
  // assert_parsed_expression_simplify_to("log(cos(9)^ln(6), cos(9))",
  // "ln(2)+ln(3)");
  // TODO: for this to work, we must know the sign of cos(9)
  // assert_parsed_expression_simplify_to("log(cos(9)^ln(6), 9)",
  // "ln(6)×log(cos(9), 9)");
  assert_parsed_expression_simplify_to("(((√(6)-√(2))/4)/((√(6)+√(2))/4))+1",
                                       "3-√(3)");
  // TODO: get rid of complex at denominator?
  assert_parsed_expression_simplify_to("1/√(i) × (√(2)-i×√(2))", "-2×i");
  simplified_approximates_to<double>(
      "abs(√(300000.0003^23))", "9.702740901018ᴇ62", k_degreeCartesianCtx, 13);
}
