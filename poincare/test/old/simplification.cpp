#include <poincare/symbol_context.h>
#include <poincare/test/helpers/symbol_store.h>

#include "../approximation//helper.h"
#include "../helper.h"
#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

QUIZ_CASE(poincare_simplification_decimal) {
  // Big decimals are handled as m*10^e
  assert_parsed_expression_simplify_to("1ᴇ1000", "10^1000");
  assert_parsed_expression_simplify_to("-1ᴇ1000", "-10^1000");
  assert_parsed_expression_simplify_to("1ᴇ-1000", "1/10^1000");
  assert_parsed_expression_simplify_to("45.678ᴇ200", "45678×10^197");
  assert_parsed_expression_simplify_to("-45.678ᴇ200", "-45678×10^197");
}

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

QUIZ_CASE(poincare_simplification_power) {
  assert_parsed_expression_simplify_to("3^4", "81");
  assert_parsed_expression_simplify_to("3^(-4)", "1/81");
  assert_parsed_expression_simplify_to("(-3)^3", "-27");
  assert_parsed_expression_simplify_to("1256^(1/3)×x", "2×root(157,3)×x");
  assert_parsed_expression_simplify_to("1256^(-1/3)", "root(24649,3)/314");
  assert_parsed_expression_simplify_to("32^(-1/5)", "1/2");
  assert_parsed_expression_simplify_to("(2+3-4)^(x)", "1");
  assert_parsed_expression_simplify_to("1^x", "1");
  assert_parsed_expression_simplify_to("x^1", "x");
  assert_parsed_expression_simplify_to("0^3", "0");
  assert_parsed_expression_simplify_to("0^0", "undef");
  assert_parsed_expression_simplify_to("0^i", "undef");
  assert_parsed_expression_simplify_to("0^(-3)", "undef");
  assert_parsed_expression_simplify_to("0^(-5+ln(5))", "undef");
  assert_parsed_expression_simplify_to("0^(5+ln(5))", "0");
  assert_parsed_expression_simplify_to("4^0.5", "2");
  assert_parsed_expression_simplify_to("8^0.5", "2×√(2)");
  assert_parsed_expression_simplify_to("(12^4×3)^(0.5)", "144×√(3)");
  assert_parsed_expression_simplify_to("(π^3)^4", "π^12");
  assert_parsed_expression_simplify_to("(P×Q)^3", "P^3×Q^3");
  assert_parsed_expression_simplify_to("(12^4×x)^(0.5)", "144×√(x)");
  assert_parsed_expression_simplify_to("√(32)", "4×√(2)");
  assert_parsed_expression_simplify_to("√(-1)", "i");
  assert_parsed_expression_simplify_to("√(-1×√(-1))", "√(2)/2-√(2)/2×i");
  assert_parsed_expression_simplify_to("√(3^2)", "3");
  assert_parsed_expression_simplify_to("2^(2+π)", "4×2^π");
  assert_parsed_expression_simplify_to("√(5513219850886344455940081)",
                                       "2348024669991");
  assert_parsed_expression_simplify_to("√(154355776)", "12424");
  assert_parsed_expression_simplify_to("√(π)^2", "π");
  assert_parsed_expression_simplify_to("√(π^2)", "π");
  assert_parsed_expression_simplify_to("√((-π)^2)", "π");
  assert_parsed_expression_simplify_to("√(x×144)", "12×√(x)");
  assert_parsed_expression_simplify_to("√(x×144×π^2)", "12×π×√(x)");
  assert_parsed_expression_simplify_to("√(x×144×π)", "12×√(π)×√(x)");
  assert_parsed_expression_simplify_to("(-1)×(2+(-4×√(2)))", "-2+4×√(2)");
  assert_parsed_expression_simplify_to("x^(1/2)", "√(x)");
  assert_parsed_expression_simplify_to("x^(-1/2)", "1/√(x)");
  assert_parsed_expression_simplify_to("x^(1/7)", "root(x,7)");
  assert_parsed_expression_simplify_to("x^(-1/7)", "1/root(x,7)");
  assert_parsed_expression_simplify_to("1/(3√(2))", "√(2)/6");
  assert_parsed_expression_simplify_to("e^ln(3)", "3");
  assert_parsed_expression_simplify_to("e^ln(√(3))", "√(3)");
  assert_parsed_expression_simplify_to("π^log(√(3),π)", "√(3)");
  assert_parsed_expression_simplify_to("10^log(π)", "π");
  assert_parsed_expression_simplify_to("10^log(4)", "4");
  assert_parsed_expression_simplify_to("10^(3*log(4))", "64");
  assert_parsed_expression_simplify_to("10^(3*log(π))", "π^3");
  assert_parsed_expression_simplify_to("10^(π*log(3))", "3^π");
  assert_parsed_expression_simplify_to("10^(log(12)+log(10))", "120");
  assert_parsed_expression_simplify_to("10^(log(π)+log(10))", "10×π");
  assert_parsed_expression_simplify_to("10^(3*log(5)+log(4))", "500");
  assert_parsed_expression_simplify_to("10^(3*log(π)+log(4))", "4×π^3");
  assert_parsed_expression_simplify_to("10^(3+log(4))", "4000");
  assert_parsed_expression_simplify_to("10^(3+2*log(4))", "16000");
  assert_parsed_expression_simplify_to("10^(3+2*log(π))", "1000×π^2");
  assert_parsed_expression_simplify_to("e^ln(65)", "65");
  assert_parsed_expression_simplify_to("e^(2ln(3))", "9");
  assert_parsed_expression_simplify_to("e^ln(πe)", "π×e");
  assert_parsed_expression_simplify_to("e^log(πe)", "e^(log(e)+log(π))");
  assert_parsed_expression_simplify_to("√(e^2)", "e");
  assert_parsed_expression_simplify_to("999^(10000/3)", "999^(10000/3)");
  assert_parsed_expression_simplify_to("root(4,4)", "√(2)");
  assert_parsed_expression_simplify_to("root(2^6*3^24*5^9*7^3,12)",
                                       "9×root(3500,4)");
  assert_parsed_expression_simplify_to("1/√(2)", "√(2)/2");
  assert_parsed_expression_simplify_to("root(8/9,3)", "(2×root(3,3))/3");
  assert_parsed_expression_simplify_to("√(2)×root(8,4)", "2×root(2,4)");
  assert_parsed_expression_simplify_to("e^ln(π)", "π");
  assert_parsed_expression_simplify_to("10^log(1.23)", "123/100");
  assert_parsed_expression_simplify_to("2^log(3,2)", "3");
  assert_parsed_expression_simplify_to(
      "1881676377434183981909562699940347954480361860897069^(1/3)",
      "1.2345678912346ᴇ17");
  /* This does not reduce but should not as the prime decomposition involves
   * factors above k_maxNumberOfPrimeFactors. */
  assert_parsed_expression_simplify_to("1002101470343^(1/3)",
                                       "root(1002101470343,3)");
  assert_parsed_expression_simplify_to("π×π×π", "π^3");
  assert_parsed_expression_simplify_to("(x+π)^(3)", "x^3+3×π×x^2+3×π^2×x+π^3");
  assert_parsed_expression_simplify_to("(5+√(2))^(-8)",
                                       "(1446241-1003320×√(2))/78310985281");
  assert_parsed_expression_simplify_to(
      "(5×π+√(2))^(-5)",
      "1/"
      "(3125×π^5+3125×√(2)×π^4+2500×π^3+500×√(2)×π^2+100×π+4×√(2))");
  assert_parsed_expression_simplify_to("(1+√(2)+√(3))^5",
                                       "296+120×√(6)+184×√(3)+224×√(2)");
  assert_parsed_expression_simplify_to(
      "(π+√(2)+√(3)+x)^(-3)",
      "1/"
      "(x^3+3×π×x^2+3×√(3)×x^2+3×√(2)×x^2+3×π^2×x+6×√(3)×π×x+6×√(2)×π×x+"
      "6×√(6)×x+15×x+π^3+3×√(3)×π^2+3×√(2)×π^2+6×√(6)×π+15×π+9×√(3)+11×√(2)"
      ")");
  assert_parsed_expression_simplify_to("1.0066666666667^60",
                                       "(10066666666667/10000000000000)^60");
  assert_parsed_expression_simplify_to("2^(6+π+x)", "64×2^(x+π)");
  assert_parsed_expression_simplify_to("i^(2/3)", "1/2+√(3)/2×i");
  assert_parsed_expression_simplify_to("e^(i×π/3)", "1/2+√(3)/2×i");
  assert_parsed_expression_simplify_to("(-1)^2", "1");
  assert_parsed_expression_simplify_to("(-1)^3", "-1");
  assert_parsed_expression_simplify_to("(-1)^2006", "1");
  assert_parsed_expression_simplify_to("(-1)^2007", "-1");
  assert_parsed_expression_simplify_to("(-2)^2006", "(-2)^2006");
  assert_parsed_expression_simplify_to("(-2)^2007", "(-2)^2007");
#if 0
  // TODO_PCJ: these tests fail with emscripten
  assert_parsed_expression_simplify_to("(-1)^(1/2)", "i");
  assert_parsed_expression_simplify_to("(-1)^(-1/2)", "-i");
  assert_parsed_expression_simplify_to("(-1)^(1/3)", "1/2+√(3)/2×i");
#endif
  assert_parsed_expression_simplify_to("(x^2)^3", "x^6", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("(x^2)^3", "x^6", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("(x^(2/3))^3", "x^2", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("(x^(2/3))^3", "x^2", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("(x^(3/4))^4", "dep(x^3,{x^(3/4)})",
                                       User, Radian, MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("(x^(3/4))^4", "x^3", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("√(-x)", "√(-x)");
  assert_parsed_expression_simplify_to("√(x)^2", "dep(x,{√(x)})", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("√(x)^2", "x", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("√(-3)^2", "nonreal", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("(-0.0001)^6.3", "nonreal", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("√(3.3×10^(-37))",
                                       "√(33)/10000000000000000000");
  assert_parsed_expression_simplify_to("√(3.3×10^(-38))",
                                       "√(330)/100000000000000000000");
  assert_parsed_expression_simplify_to("√(3.3×10^(-39))",
                                       "√(33)/100000000000000000000");
  assert_parsed_expression_simplify_to("(√(2)^√(2))^√(2)", "2");

  // Principal angle of root of unity
  assert_parsed_expression_simplify_to("(-5)^(-1/3)",
                                       "root(25,3)/10-root(16875,6)/10×i", User,
                                       Radian, MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("1+((8+√(6))^(1/2))^-1+(8+√(6))^(1/2)",
                                       "1+√(8+√(6))+(√(58)×√(8-√(6)))/58", User,
                                       Radian, MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)",
                                       "[[-59/4,27/4][81/8,-37/8]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^3", "[[37,54][81,118]]");
  assert_parsed_expression_simplify_to("(3_m^2)^3", "27×_m^6");
  assert_parsed_expression_simplify_to("(i×floor(-abs(x)))^(2/3)",
                                       "(i×floor(-abs(x)))^(2/3)");
  // Denesting of square roots
  assert_parsed_expression_simplify_to("√(2+√(3))", "(√(2)+√(6))/2");
  assert_parsed_expression_simplify_to("√(3-√(7))", "√(3-√(7))");
  assert_parsed_expression_simplify_to("√(-2+√(3))", "(√(6)-√(2))/2×i", User,
                                       Radian, MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("√(17+4×√(13))", "2+√(13)");
  assert_parsed_expression_simplify_to("√(√(1058)-√(896))",
                                       "-root(98,4)+4×root(2,4)");
  assert_parsed_expression_simplify_to("√(57×√(17)+68×√(10))",
                                       "root(4913,4)+2×root(1700,4)");
  // √(a^2±2ab+b^2)
  assert_parsed_expression_simplify_to("√((π+1)^2)", "π+1");
  assert_parsed_expression_simplify_to("√((π-1)^2)", "π-1");
  assert_parsed_expression_simplify_to("√(1/(ln(2)^2-2πln(2)+π^2))",
                                       "1/(-ln(2)+π)");
  assert_parsed_expression_simplify_to("(-2)^(n-1)", "-(-2)^n/2");
  assert_parsed_expression_simplify_to("e^(ln(x))", "dep(x,{nonNull(x)})");
  // Special cases with (a*b)^x = |a|^x*(sign(a)*b)
  assert_parsed_expression_simplify_to("√(-arcsin(-e-1))", "√(arcsin(1+e))");
  assert_parsed_expression_simplify_to("((-e^3)×cos(2))^(1/3)",
                                       "e×root(-cos(2),3)");
  // assert this does not crash
  assert_parsed_expression_simplify_to("90000000005^(x+0.5)",
                                       "90000000005^((2×x+1)/2)");
  assert_parsed_expression_simplify_to("(-123456789012345)^3",
                                       "(-123456789012345)^3");
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
