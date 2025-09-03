#include <poincare/context.h>
#include <poincare/test/helpers/symbol_store.h>

#include "../approximation//helper.h"
#include "../helper.h"
#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

QUIZ_CASE(poincare_simplification_based_integer) {
  assert_parsed_expression_simplify_to("0b10011", "19");
  assert_parsed_expression_simplify_to("0x2A", "42");
}

QUIZ_CASE(poincare_simplification_decimal) {
  assert_parsed_expression_simplify_to("-2.3", "-23/10");
  assert_parsed_expression_simplify_to("-232.2ᴇ-4", "-1161/50000");
  assert_parsed_expression_simplify_to("0000.000000ᴇ-2", "0");
  assert_parsed_expression_simplify_to(".000000", "0");
  assert_parsed_expression_simplify_to("0000", "0");
  // Big decimals are handled as m*10^e
  assert_parsed_expression_simplify_to("1ᴇ1000", "10^1000");
  assert_parsed_expression_simplify_to("-1ᴇ1000", "-10^1000");
  assert_parsed_expression_simplify_to("1ᴇ-1000", "1/10^1000");
  assert_parsed_expression_simplify_to("45.678ᴇ200", "45678×10^197");
  assert_parsed_expression_simplify_to("-45.678ᴇ200", "-45678×10^197");
}

QUIZ_CASE(poincare_simplification_rational) {
  // 1/MaxParsedIntegerString()
  constexpr static size_t k_bufferSizeOfMax = 32;
  char bufferMax[k_bufferSizeOfMax] = "1/";
  size_t bufferLengthOfMax = strlen(bufferMax);
  strlcpy(bufferMax + bufferLengthOfMax, MaxParsedIntegerString(),
          k_bufferSizeOfMax - bufferLengthOfMax);
  assert_parsed_expression_simplify_to(bufferMax, bufferMax);
  // 1/OverflowedIntegerString()
  constexpr static size_t k_bufferSizeOfInf = 400;
  char bufferInf[k_bufferSizeOfInf] = "1/";
  size_t bufferLengthOfInf = strlen(bufferInf);
  strlcpy(bufferInf + bufferLengthOfInf, BigOverflowedIntegerString(),
          k_bufferSizeOfInf - bufferLengthOfInf);
  assert_parse_to_integer_overflow(bufferInf);
  // MaxParsedIntegerString()
  assert_parsed_expression_simplify_to(MaxParsedIntegerString(),
                                       MaxParsedIntegerString());
  // OverflowedIntegerString()
  assert_parse_to_integer_overflow(OverflowedIntegerString());
  assert_parse_to_integer_overflow(BigOverflowedIntegerString());
  // ApproximatedParsedIntegerString()
  assert_parsed_expression_simplify_to(ApproximatedParsedIntegerString(),
                                       "1ᴇ30");
  // -OverflowedIntegerString()
  bufferInf[0] = '-';
  bufferLengthOfInf = 1;
  strlcpy(bufferInf + bufferLengthOfInf, BigOverflowedIntegerString(),
          k_bufferSizeOfInf - bufferLengthOfInf);
  assert_parse_to_integer_overflow(bufferInf);

  assert_parsed_expression_simplify_to("-1/3", "-1/3");
  assert_parsed_expression_simplify_to("22355/45325", "4471/9065");
  assert_parsed_expression_simplify_to("0000.000000", "0");
  assert_parsed_expression_simplify_to(".000000", "0");
  assert_parsed_expression_simplify_to("0000", "0");
  assert_parsed_expression_simplify_to("0.1234567", "1234567/10000000");
  assert_parsed_expression_simplify_to("123.4567", "1234567/10000");
  assert_parsed_expression_simplify_to("0.1234", "617/5000");
  assert_parsed_expression_simplify_to("0.1234000", "617/5000");
  assert_parsed_expression_simplify_to("001234000", "1234000");
  assert_parsed_expression_simplify_to("001.234000ᴇ3", "1234");
  assert_parsed_expression_simplify_to("001234000ᴇ-4", "617/5");
  assert_parsed_expression_simplify_to("3/4+5/4-12+1/567", "-5669/567");
  assert_parsed_expression_simplify_to("34/78+67^(-1)", "1178/2613");
  assert_parsed_expression_simplify_to("12348/34564", "3087/8641");
  assert_parsed_expression_simplify_to("1-0.3-0.7", "0");
  assert_parsed_expression_simplify_to("123456789123456789+112233445566778899",
                                       "235690234690235688");
  assert_parsed_expression_simplify_to("56^56", "56^56");
  assert_parsed_expression_simplify_to("999^999", "999^999");
  assert_parsed_expression_simplify_to("999^-999", "1/999^999");
  assert_parsed_expression_simplify_to("0^0", "undef");
  assert_parsed_expression_simplify_to("π^0", "1");
  assert_parsed_expression_simplify_to("(-3)^0", "1");
  assert_parsed_expression_simplify_to("2ᴇ200/2ᴇ200", "1");
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

QUIZ_CASE(poincare_simplification_addition) {
  assert_parsed_expression_simplify_to("1/x^2+3", "(3×x^2+1)/x^2", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("1+x", "x+1");
  assert_parsed_expression_simplify_to("1/2+1/3+1/4+1/5+1/6+1/7", "223/140");
  assert_parsed_expression_simplify_to("1+x+4-y-2x", "-x-y+5");
  assert_parsed_expression_simplify_to("2+1", "3");
  assert_parsed_expression_simplify_to("1+2", "3");
  assert_parsed_expression_simplify_to("1+2+3+4+5+6+7", "28");
  assert_parsed_expression_simplify_to("(0+0)", "0");
  assert_parsed_expression_simplify_to("1+2+3+4+5+M+6+7", "M+28");
  assert_parsed_expression_simplify_to("1+M+2+P+3", "M+P+6");
  assert_parsed_expression_simplify_to("-2+6", "4");
  assert_parsed_expression_simplify_to("-2-6", "-8");
  assert_parsed_expression_simplify_to("-M", "-M");
  assert_parsed_expression_simplify_to("M-M", "dep(0,{M})");
  assert_parsed_expression_simplify_to("-5π+3π", "-2×π");
  assert_parsed_expression_simplify_to("1-3+M-5+2M-4M", "-M-7");
  assert_parsed_expression_simplify_to("M+P-M-P", "dep(0,{M,P})");
  assert_parsed_expression_simplify_to("M+P+(-1)×M+(-1)×P", "dep(0,{M,P})");
  assert_parsed_expression_simplify_to("2+13cos(2)-23cos(2)", "-10×cos(2)+2");
  assert_parsed_expression_simplify_to("1+1+ln(2)+(5+3×2)/9-4/7+1/98",
                                       "(882×ln(2)+2347)/882");
  assert_parsed_expression_simplify_to("1+2+0+cos(2)", "cos(2)+3");
  assert_parsed_expression_simplify_to("M-M+2cos(2)+P-P-cos(2)",
                                       "dep(cos(2),{M,P})");
  assert_parsed_expression_simplify_to("x+3+π+2×x", "3×x+π+3");
  assert_parsed_expression_simplify_to("1/(x+1)+1/(π+2)",
                                       "(x+π+3)/(π×x+2×x+π+2)");
  assert_parsed_expression_simplify_to("1/x^2+1/(x^2×π)", "(π+1)/(π×x^2)");
  assert_parsed_expression_simplify_to("1/x^2+1/(x^3×π)", "(π×x+1)/(π×x^3)");
  assert_parsed_expression_simplify_to("4x/x^2+3π/(x^3×π)", "(4×x^2+3)/x^3");
  assert_parsed_expression_simplify_to("3^(1/2)+π^2+1", "π^2+1+√(3)");
  assert_parsed_expression_simplify_to("3^(1/2)+2^(-2×3^(1/2)×e^π)/2",
                                       "(1+2×2^(2×√(3)×e^(π))×√(3))/"
                                       "(2×2^(2×√(3)×e^(π)))");
  assert_parsed_expression_simplify_to("((4π+5)/10)-2.3", "(2×π-9)/5");
  assert_parsed_expression_simplify_to("4×_°+3×_'+2×_\"-(3×_°+2×_'+1×_\")",
                                       "(3661×π)/648000×_rad");
  assert_parsed_expression_simplify_to(
      "[[1,2+i][3,4][5,6]]+[[1,2+i][3,4][5,6]]", "[[2,4+2×i][6,8][10,12]]");
  assert_parsed_expression_simplify_to("3+[[1,2][3,4]]", "undef");
  assert_parsed_expression_simplify_to("[[1][3][5]]+[[1,2+i][3,4][5,6]]",
                                       "undef");
  // Beautification order
  assert_parsed_expression_simplify_to("√(2)+4+3×π+√(5)+2×√(5)",
                                       "3×π+4+3×√(5)+√(2)");
  assert_parsed_expression_simplify_to(
      "(√(2)+1+x+y)^3",
      "x^3+y^3+3×x×y^2+3×x^2×y+3×√(2)×x^2+3×x^2+3×√(2)×y^2+3×y^2+6×√(2)×x×y+"
      "6×x×y+6×√(2)×x+9×x+6×√(2)×y+9×y+7+5×√(2)");
  // cos^2+sin^2
  assert_parsed_expression_simplify_to("cos(x)^2+sin(x)^2", "dep(1,{x})");
  assert_parsed_expression_simplify_to("2xln(x)cos(x)^2+2xln(x)sin(x)^2",
                                       "dep(2×x×ln(x),{nonNull(x)})");
  assert_parsed_expression_simplify_to("5cos(3x+2)^2+5sin(3x+2)^2+3",
                                       "dep(8,{x})");
  assert_parsed_expression_simplify_to("4cos(x)^2+3sin(x)^2",
                                       "4×cos(x)^2+3×sin(x)^2");
}

QUIZ_CASE(poincare_simplification_multiplication) {
  assert_parsed_expression_simplify_to("undef×x", "undef");
  assert_parsed_expression_simplify_to("0×x+P", "dep(P,{x})");
  assert_parsed_expression_simplify_to("0×x×0×32×cos(3)", "dep(0,{x})");
  assert_parsed_expression_simplify_to("3×M^4×P^x×P^2×(M^2+2)×2×1.2",
                                       "(36×M^6×P^(x+2)+72×M^4×P^(x+2))/5");
  assert_parsed_expression_simplify_to("M×(P+Q)×(X+3)",
                                       "M×P×X+M×Q×X+3×M×P+3×M×Q");
  assert_parsed_expression_simplify_to("M/P", "M/P");
  assert_parsed_expression_simplify_to("(1/2)×M/P", "M/(2×P)");
  assert_parsed_expression_simplify_to("1+2+3+4+5+6", "21");
  assert_parsed_expression_simplify_to("1-2+3-4+5-6", "-3");
  assert_parsed_expression_simplify_to("987654321123456789×998877665544332211",
                                       "986545842648570754445552922919330479");
  assert_parsed_expression_simplify_to("2/3", "2/3");
  assert_parsed_expression_simplify_to("9/17+5/4", "121/68");
  assert_parsed_expression_simplify_to("1/2×3/4", "3/8");
  assert_parsed_expression_simplify_to("0×2/3", "0");
  assert_parsed_expression_simplify_to("1+(1/(1+1/(1+1/(1+1))))", "8/5");
  assert_parsed_expression_simplify_to("1+2/(3+4/(5+6/(7+8)))", "155/101");
  assert_parsed_expression_simplify_to("3/4×16/12", "1");
  assert_parsed_expression_simplify_to("3/4×(8+8)/12", "1");
  assert_parsed_expression_simplify_to("916791/794976477", "305597/264992159");
  assert_parsed_expression_simplify_to("321654987123456789/112233445566778899",
                                       "3249040273974311/1133671167341201");
  assert_parsed_expression_simplify_to("0.1+0.2", "3/10");
  assert_parsed_expression_simplify_to("2^3", "8");
  assert_parsed_expression_simplify_to("(-1)×(-1)", "1");
  assert_parsed_expression_simplify_to("(-2)^2", "4");
  assert_parsed_expression_simplify_to("(-3)^3", "-27");
  assert_parsed_expression_simplify_to("(1/2)^-1", "2");
  assert_parsed_expression_simplify_to("√(2)×√(3)", "√(6)");
  assert_parsed_expression_simplify_to("2×2^π", "2×2^π");
  assert_parsed_expression_simplify_to("M^3×P×M^(-3)", "dep(P,{M^0})");
  assert_parsed_expression_simplify_to("M^3×M^(-3)", "dep(1,{M^0})");
  assert_parsed_expression_simplify_to("2^π×(1/2)^π", "1");
  assert_parsed_expression_simplify_to("-iπ^3×(-iπ^3)×(-iπ^3)", "π^9×i");
  assert_parsed_expression_simplify_to("(x+1)×(x+2)", "x^2+3×x+2");
  assert_parsed_expression_simplify_to("(x+1)×(x-1)", "x^2-1");
  assert_parsed_expression_simplify_to("11π/(22π+11π)", "1/3");
  assert_parsed_expression_simplify_to("11/(22π+11π)", "1/(3×π)");
  assert_parsed_expression_simplify_to("-11/(22π+11π)", "-1/(3×π)");
  assert_parsed_expression_simplify_to("M^2×P×M^(-2)×P^(-2)", "dep(1/P,{M^0})");
  assert_parsed_expression_simplify_to("M^(-1)×P^(-1)", "1/(M×P)");
  assert_parsed_expression_simplify_to("x+x", "2×x");
  assert_parsed_expression_simplify_to("2×x+x", "3×x");
  assert_parsed_expression_simplify_to("x×2+x", "3×x");
  assert_parsed_expression_simplify_to("2×x+2×x", "4×x");
  assert_parsed_expression_simplify_to("x×2+2×n", "2×n+2×x");
  assert_parsed_expression_simplify_to("x+x+n+n", "2×n+2×x");
  assert_parsed_expression_simplify_to("x-x-n+n", "dep(0,{n,x})");
  assert_parsed_expression_simplify_to("x+n-x-n", "dep(0,{n,x})");
  assert_parsed_expression_simplify_to("x-x", "dep(0,{x})");
  assert_parsed_expression_simplify_to("π×3^(1/2)×(5π)^(1/2)×(4/5)^(1/2)",
                                       "2×√(3)×π^(3/2)");
  assert_parsed_expression_simplify_to("12^(1/4)×(π/6)×(12×π)^(1/4)",
                                       "(√(3)×π^(5/4))/3");
  assert_parsed_expression_simplify_to(
      "[[1,2+i][3,4][5,6]]×[[1,2+i,3,4][5,6+i,7,8]]",
      "[[11+5×i,13+9×i,17+7×i,20+8×i][23,30+7×i,37,44][35,46+11×i,57,68]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]×[[1,3][5,6]]×[[2,3][4,6]]",
                                       "[[82,123][178,267]]");
  assert_parsed_expression_simplify_to("0*[[1,0][0,1]]^500",
                                       "0×[[1,0][0,1]]^500");
  assert_parsed_expression_simplify_to("x^5/x^3", "dep(x^2,{nonNull(x)})");
  assert_parsed_expression_simplify_to("x^5*x^3", "x^8");
  assert_parsed_expression_simplify_to("x^3/x^5", "1/x^2");
  assert_parsed_expression_simplify_to("x^0", "dep(1,{x^0})");
  assert_parsed_expression_simplify_to("π^5/π^3", "π^2", SystemForAnalysis,
                                       Radian, MetricUnitFormat, Cartesian,
                                       ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("π^5*π^3", "π^8", SystemForAnalysis,
                                       Radian, MetricUnitFormat, Cartesian,
                                       ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("π^3/π^5", "1/π^2", SystemForAnalysis,
                                       Radian, MetricUnitFormat, Cartesian,
                                       ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("π^0", "1", SystemForAnalysis, Radian,
                                       MetricUnitFormat, Cartesian,
                                       ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("π^π/π^(π-1)", "π", SystemForAnalysis,
                                       Radian, MetricUnitFormat, Cartesian,
                                       ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to(
      "x^5/x^3", "dep(x^2,{nonNull(x)})", SystemForAnalysis, Radian,
      MetricUnitFormat, Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("x^5×x^3", "x^8", SystemForAnalysis,
                                       Radian, MetricUnitFormat, Cartesian,
                                       ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("x^3/x^5", "1/x^2", SystemForAnalysis,
                                       Radian, MetricUnitFormat, Cartesian,
                                       ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("x^0", "dep(1,{x^0})", SystemForAnalysis,
                                       Radian, MetricUnitFormat, Cartesian,
                                       ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("x^π/x^(π-1)", "x", SystemForAnalysis,
                                       Radian, MetricUnitFormat, Cartesian,
                                       ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("x^π/x^(π+1)", "1/x", SystemForAnalysis,
                                       Radian, MetricUnitFormat, Cartesian,
                                       ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to(
      "2^x×2^(-x)", "dep(1,{x})", SystemForAnalysis, Radian, MetricUnitFormat,
      Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to(
      "y^x×y^(-x)", "dep(1,{x,y})", SystemForAnalysis, Radian, MetricUnitFormat,
      Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to(
      "x/√(x)", "dep(√(x),{-ln(x)/2})", SystemForAnalysis, Radian,
      MetricUnitFormat, Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to(
      "x^(1/2)×x^(1/2)", "x", SystemForAnalysis, Radian, MetricUnitFormat,
      Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("x^(1/2)×x^(1/2)", "dep(x,{√(x)})", User,
                                       Radian, MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("2*3^x*3^(-x)", "dep(2,{x})");
  assert_parsed_expression_simplify_to("10-1/(3^x)", "(10×3^x-1)/3^x");
  assert_parsed_expression_simplify_to("2×cos(π/12)×e^(5πi/12)",
                                       "1/2+\U000000122+√(3)\U00000013/2×i");
  // Do not factorize exponent if the multiplication result is over DBL_MAX
  assert_parsed_expression_simplify_to(
      "((1.63871ᴇ182)^(1/256))*((1.93871ᴇ157)^(1/256))",
      "root("
      "193871000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000000000000,256)×root("
      "163871000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000,256)");
}

#if 0
void assert_parsed_unit_simplify_to_with_prefixes(
    const OUnit::Representative* representative) {
  int numberOfPrefixes;
  const OUnit::Prefix* prefixes;
  constexpr static size_t bufferSize = 12;
  char buffer[bufferSize] = "1×";
  if (representative->isOutputPrefixable()) {
    numberOfPrefixes = OUnit::Prefix::k_numberOfPrefixes;
    prefixes = OUnit::k_prefixes;
  } else {
    numberOfPrefixes = 1;
    prefixes = OUnit::Prefix::EmptyPrefix();
  }
  for (int i = 0; i < numberOfPrefixes; i++) {
    if (representative->canPrefix(prefixes + i, true) &&
        representative->canPrefix(prefixes + i, false)) {
      OUnit::Builder(representative, prefixes + i)
          .serialize(buffer + strlen("1×"), bufferSize - strlen("1×"),
                     DecimalMode,
                     Preferences::VeryShortNumberOfSignificantDigits);
      assert_parsed_expression_simplify_to(buffer, buffer);
    }
  }
}
#endif

QUIZ_CASE(poincare_simplification_units) {
  /* SI base units */
  assert_parsed_expression_simplify_to("_s", "1×_s");
  assert_parsed_expression_simplify_to("_m", "1×_m");
  assert_parsed_expression_simplify_to("_kg", "1×_kg");
  assert_parsed_expression_simplify_to("_A", "1×_A");
  assert_parsed_expression_simplify_to("_K", "1×_K");
  assert_parsed_expression_simplify_to("_mol", "1×_mol");
  assert_parsed_expression_simplify_to("_cd", "1×_cd");
  assert_parsed_expression_simplify_to("-_s", "-1×_s");

  /* Inverses of SI base units */
  assert_parsed_expression_simplify_to("_s^-1", "1×_s^(-1)");
  assert_parsed_expression_simplify_to("_m^-1", "1×_m^(-1)");
  assert_parsed_expression_simplify_to("_kg^-1", "1×_kg^(-1)");
  assert_parsed_expression_simplify_to("_A^-1", "1×_A^(-1)");
  assert_parsed_expression_simplify_to("_K^-1", "1×_K^(-1)");
  assert_parsed_expression_simplify_to("_mol^-1", "1×_mol^(-1)");
  assert_parsed_expression_simplify_to("_cd^-1", "1×_cd^(-1)");

  /* Power of SI units */
  assert_parsed_expression_simplify_to("_s^3", "1×_s^3");
  assert_parsed_expression_simplify_to("_m^2", "1×_m^2");
  assert_parsed_expression_simplify_to("_m^3", "1×_m^3");
  assert_parsed_expression_simplify_to("_m^(1/2)", "1×_m^(1/2)");

  /* Possible improvements */
  /* Ignored derived metrics :
   * -> Possible solution : Favor unities from user input. We do not want to
   *    favor positive exponents to avoid a Velocity being displayed as _m*_Hz
   * assert_parsed_expression_simplify_to("_Hz", "_Hz");
   * assert_parsed_expression_simplify_to("_S", "_S");
   */
  /* Non unitary exponents on Derived metrics :
   * -> See CanSimplifyUnitProduct in multiplication.cpp
   * assert_parsed_expression_simplify_to("_C^3", "1×_C^3");
   * assert_parsed_expression_simplify_to("_N^(1/2)", "1×_N^(1/2)");
   */
  /* Taking exponents complexity into account :
   * -> See note on metrics in CanSimplifyUnitProduct in multiplication.cpp
   * assert_parsed_expression_simplify_to("_C×_s", "1×_C×_s");
   * assert_parsed_expression_simplify_to("_C^10", "1×_C^10");
   * assert_parsed_expression_simplify_to("_ha", "1×_ha");
   * FIXME : int8_t norm metric overflow, only visible with a non constant norm
   * assert_parsed_expression_simplify_to("_C^130", "1×_C^130"); */
  assert_parsed_expression_simplify_to("_m_s^-2", "1×_m×_s^(-2)");

  /* SI derived units with special names and symbols */
  assert_parsed_expression_simplify_to("_kg×_m×_s^(-2)", "1×_N");
  assert_parsed_expression_simplify_to("_kg×_m^(-1)×_s^(-2)", "1×_Pa");
  assert_parsed_expression_simplify_to("_kg×_m^2×_s^(-2)", "1×_J");
  assert_parsed_expression_simplify_to("_kg×_m^2×_s^(-3)", "1×_W");
  assert_parsed_expression_simplify_to("_A×_s", "1×_C");
  assert_parsed_expression_simplify_to("_kg×_m^2×_s^(-3)×_A^(-1)", "1×_V");
  assert_parsed_expression_simplify_to("_m^(-2)×_kg^(-1)×_s^4×_A^2", "1×_F");
  assert_parsed_expression_simplify_to("_kg×_m^2×_s^(-3)×_A^(-2)", "1×_Ω");
  assert_parsed_expression_simplify_to("_kg×_m^2×_s^(-2)×_A^(-1)", "1×_Wb");
  assert_parsed_expression_simplify_to("_kg×_s^(-2)×_A^(-1)", "1×_T");
  assert_parsed_expression_simplify_to("_kg×_m^2×_s^(-2)×_A^(-2)", "1×_H");
  assert_parsed_expression_simplify_to("_mol×_s^-1", "1×_kat");

  /* Displayed order of magnitude */
  assert_parsed_expression_simplify_to("100_kg", "100×_kg");
  assert_parsed_expression_simplify_to("1_min", "1×_min");
  assert_parsed_expression_simplify_to("0.1_m", "1×_dm");
  assert_parsed_expression_simplify_to("180_MΩ", "180×_MΩ");
  assert_parsed_expression_simplify_to("180_MH", "180×_MH");

  /* Test simplification of all possible (prefixed) unit symbols.
   * Some symbols are however excluded:
   *  - At present, some units will not appear as simplification output:
   *    t, Hz, S, ha, L. These exceptions are tested below. */
  assert_parsed_expression_simplify_to("_s", "1×_s");
  assert_parsed_expression_simplify_to("_min", "1×_min");
  assert_parsed_expression_simplify_to("_h", "1×_h");
  assert_parsed_expression_simplify_to("_day", "1×_day");
  assert_parsed_expression_simplify_to("_week", "1×_week");
  assert_parsed_expression_simplify_to("_month", "1×_month");
  assert_parsed_expression_simplify_to("_year", "1×_year");
#if 0
  assert_parsed_unit_simplify_to_with_prefixes(
      OUnit::k_distanceRepresentatives);
#endif
  assert_parsed_expression_simplify_to("_au", "1×_au");
  assert_parsed_expression_simplify_to("_ly", "1×_ly");
  assert_parsed_expression_simplify_to("_pc", "1×_pc");
  assert_parsed_expression_simplify_to("_in", "1×_in", User, Radian, Imperial);
  assert_parsed_expression_simplify_to("_ft", "1×_ft", User, Radian, Imperial);
  assert_parsed_expression_simplify_to("_yd", "1×_yd", User, Radian, Imperial);
  assert_parsed_expression_simplify_to("_mi", "1×_mi", User, Radian, Imperial);
#if 0
  assert_parsed_unit_simplify_to_with_prefixes(OUnit::k_massRepresentatives);
#endif
  assert_parsed_expression_simplify_to("_t", "1×_t");
  assert_parsed_expression_simplify_to("_oz", "1×_oz", User, Radian, Imperial);
  assert_parsed_expression_simplify_to("_lb", "1×_lb", User, Radian, Imperial);
  assert_parsed_expression_simplify_to("_shtn", "1×_shtn", User, Radian,
                                       Imperial);
#if 0
  assert_parsed_unit_simplify_to_with_prefixes(OUnit::k_currentRepresentatives);
  assert_parsed_unit_simplify_to_with_prefixes(
      OUnit::k_temperatureRepresentatives);
#endif
  assert_parsed_expression_simplify_to("_°C", "1×_°C");
  assert_parsed_expression_simplify_to("_°F", "1×_°F");
#if 0
  assert_parsed_unit_simplify_to_with_prefixes(
      OUnit::k_amountOfSubstanceRepresentatives);
  assert_parsed_unit_simplify_to_with_prefixes(
      OUnit::k_luminousIntensityRepresentatives);
  assert_parsed_unit_simplify_to_with_prefixes(OUnit::k_forceRepresentatives);
  assert_parsed_unit_simplify_to_with_prefixes(
      OUnit::k_pressureRepresentatives);
  assert_parsed_unit_simplify_to_with_prefixes(
      OUnit::k_pressureRepresentatives + 1);
#endif
  assert_parsed_expression_simplify_to("_atm", "1×_atm");
#if 0
  assert_parsed_unit_simplify_to_with_prefixes(OUnit::k_energyRepresentatives);
  assert_parsed_unit_simplify_to_with_prefixes(OUnit::k_energyRepresentatives +
                                               1);
  assert_parsed_unit_simplify_to_with_prefixes(OUnit::k_powerRepresentatives);
  assert_parsed_unit_simplify_to_with_prefixes(
      OUnit::k_electricChargeRepresentatives);
  assert_parsed_unit_simplify_to_with_prefixes(
      OUnit::k_electricPotentialRepresentatives);
  assert_parsed_unit_simplify_to_with_prefixes(
      OUnit::k_electricCapacitanceRepresentatives);
  assert_parsed_unit_simplify_to_with_prefixes(
      OUnit::k_electricResistanceRepresentatives);
  assert_parsed_unit_simplify_to_with_prefixes(
      OUnit::k_magneticFieldRepresentatives);
  assert_parsed_unit_simplify_to_with_prefixes(
      OUnit::k_magneticFluxRepresentatives);
  assert_parsed_unit_simplify_to_with_prefixes(
      OUnit::k_inductanceRepresentatives);
  assert_parsed_unit_simplify_to_with_prefixes(
      OUnit::k_catalyticActivityRepresentatives);
#endif

  /* Units that do not appear as output */
  assert_parsed_expression_simplify_to("_Hz", "1×_s^(-1)");
  assert_parsed_expression_simplify_to("_S", "1×_Ω^(-1)");
  assert_parsed_expression_simplify_to("_L", "1×_dm^3");
  assert_parsed_expression_simplify_to("_ha", "10000×_m^2");

  /* Imperial units */
  assert_parsed_expression_simplify_to("_lgtn", "1.0160469088×_t");
  assert_parsed_expression_simplify_to("_lgtn", "1.12×_shtn", User, Radian,
                                       Imperial);
  assert_parsed_expression_simplify_to("_in", "2.54×_cm");
  assert_parsed_expression_simplify_to("_in", "1×_in", User, Radian, Imperial);
  assert_parsed_expression_simplify_to("_ft", "1×_ft", User, Radian, Imperial);
  assert_parsed_expression_simplify_to("_yd", "1×_yd", User, Radian, Imperial);
  assert_parsed_expression_simplify_to("1_qt", "1×_qt", User, Radian, Imperial);
  assert_parsed_expression_simplify_to("1_qt", "946.352946×_cm^3");

  /* Tests for non-absolute units */
  assert_parsed_expression_simplify_to("273.15×_K→_°C", "0×_°C");
  assert_parsed_expression_simplify_to("0×_°C", "0×_°C");
  assert_parsed_expression_simplify_to("-32×_°F", "-32×_°F");
  assert_parsed_expression_simplify_to("273.16×_K", "273.16×_K");
  assert_parsed_expression_simplify_to("100×_°C→_K", "373.15×_K");
  assert_parsed_expression_simplify_to("-100×_°C→_K", "173.15×_K");
  assert_parsed_expression_simplify_to("(12_m/6_m)×_°C", "2×_°C");
  assert_parsed_expression_simplify_to("_°C+_°C", "undef");
  assert_parsed_expression_simplify_to("_°C+_°F", "undef");
  assert_parsed_expression_simplify_to("_K+_°F", "undef");
  assert_parsed_expression_simplify_to("2*20_°F", "undef");
  assert_parsed_expression_simplify_to("_°C^2", "undef");
  assert_parsed_expression_simplify_to("1/(-3_°C)", "undef");
  assert_parsed_expression_simplify_to("-1×100×_°C→_K", "undef");
  assert_parsed_expression_simplify_to("_K×_°C", "undef");
  assert_parsed_expression_simplify_to("°C→x", "_°C→x");
  assert_parsed_expression_simplify_to("123°C→x", "123×_°C→x");
  assert_parsed_expression_simplify_to("-4.56°C→x", "-4.56×_°C→x");

  /* Rational exponents */
  assert_parsed_expression_simplify_to("√(_m)", "1×_m^(1/2)");
  assert_parsed_expression_simplify_to("√(_N)", "1×_kg^(1/2)×_m^(1/2)×_s^(-1)");
  assert_parsed_expression_simplify_to("√(_N)",
                                       "1.5527410012845×_lb^(1/2)×_yd^(1/"
                                       "2)×_s^(-1)",
                                       User, Radian, Imperial);
  assert_parsed_expression_simplify_to("_C^0.3", "1×_A^(3/10)×_s^(3/10)");
  assert_parsed_expression_simplify_to("_kat_kg^-2.8",
                                       "1×_mol×_kg^(-14/5)×_s^(-1)");

  /* Unit sum/subtract */
  assert_parsed_expression_simplify_to("_m+_m", "2×_m");
  assert_parsed_expression_simplify_to("_m-_m", "0×_m");

  /* Usual physical quantities */
  // Charge density
  assert_parsed_expression_simplify_to("_A×_s×_m^(-3)", "1×_C×_m^(-3)");
  // Thermal conductivity _W×_m^-1×_K^-1
  assert_parsed_expression_simplify_to("_kg×_m×_s^(-3)×_K^(-1)",
                                       "1×_N×_K^(-1)×_s^(-1)");
  // Thermal resistance
  assert_parsed_expression_simplify_to("_K×_kg^(-1)×_m^(-2)×_s^3",
                                       "1×_W^(-1)×_K");
  // Electrical field
  assert_parsed_expression_simplify_to("_kg×_m×_s^(-3)×_A^(-1)",
                                       "1×_V×_m^(-1)");
  assert_parsed_expression_simplify_to("_kg×_m^2×_s^(-1)",
                                       "1×_J×_s");  // Action
  // Entropy | Heat capacity
  assert_parsed_expression_simplify_to("_kg×_m^2×_s^(-2)×_K^(-1)",
                                       "1×_J×_K^(-1)");
  // Specific heat capacity _J×_K^-1×_kg^-1
  assert_parsed_expression_simplify_to("_m^2×_s^(-2)×_K^(-1)",
                                       "1×_K^(-1)×_m^2×_s^(-2)");
  // Molar heat capacity
  assert_parsed_expression_simplify_to("_kg×_m^2×_s^(-2)×_K^(-1)×_mol^(-1)",
                                       "1×_J×_mol^(-1)×_K^(-1)");
  // Volumetric heat capacity _J×_K^-1×_m^-3
  assert_parsed_expression_simplify_to("_kg×_m^(-1)×_s^(-2)×_K^(-1)",
                                       "1×_Pa×_K^(-1)");
  // Heat transfer coefficient _W×_m^-2×_K^-1
  assert_parsed_expression_simplify_to("_kg×_s^(-3)×_K^(-1)",
                                       "1×_K^(-1)×_kg×_s^(-3)");
  // Thermal conductivity
  assert_parsed_expression_simplify_to("_kg×_m^2×_s^(-3)×_K^(-1)",
                                       "1×_W×_K^(-1)");
  // Electrical conductivity _S×_m^-1
  assert_parsed_expression_simplify_to("_kg^(-1)×_m^(-3)×_s^3×_A^2",
                                       "1×_Ω^(-1)×_m^(-1)");
  // Stiffness _N×_m^-1
  assert_parsed_expression_simplify_to("_kg×_s^(-2)", "1×_kg×_s^(-2)");
  // Power density _W×_m^-3
  assert_parsed_expression_simplify_to("_kg×_m^(-1)×_s^(-3)", "1×_Pa×_s^(-1)");
  assert_parsed_expression_simplify_to("_kg×_m^3×_s^(-3)×_A^(-1)",
                                       "1×_V×_m");  // Electric flux
  // Superficial thermal resistance _m^2×_K×_W^-1
  assert_parsed_expression_simplify_to("_K×_kg^(-1)×_s^(3)",
                                       "1×_K×_kg^(-1)×_s^3");
  // Thermodynamic beta
  assert_parsed_expression_simplify_to("_kg^(-1)×_m^(-2)×_s^2", "1×_J^(-1)");
  assert_parsed_expression_simplify_to("_kg×_m^(-1)×_s^(-1)",
                                       "1×_Pa×_s");  // Dynamic viscosity
  // Gray/Sievert _J×_kg^(-1)
  assert_parsed_expression_simplify_to("_m^2×_s^(-2)", "1×_m^2×_s^(-2)");
  // Angular momentum _N×_m×_s
  assert_parsed_expression_simplify_to("_m^2×_kg×_s^(-1)", "1×_J×_s");
  // Energy density _J×_m^-3
  assert_parsed_expression_simplify_to("_m^(-1)×_kg×_s^(-2)", "1×_Pa");
  // Spectral power _W×_m^-1
  assert_parsed_expression_simplify_to("_m×_kg×_s^(-3)", "1×_N×_s^(-1)");
  // Compressibility
  assert_parsed_expression_simplify_to("_m×_kg^(-1)×_s^2", "1×_Pa^(-1)");
  // Molar conductivity _S×_m^2×_mol^-1
  assert_parsed_expression_simplify_to("_kg^(-1)×_s^3×_A^2×_mol^(-1)",
                                       "1×_Ω^(-1)×_mol^(-1)×_m^2");
  // Polarization density
  assert_parsed_expression_simplify_to("_m^(-2)×_s×_A", "1×_C×_m^(-2)");
  // Exposure
  assert_parsed_expression_simplify_to("_kg^(-1)×_s×_A", "1×_C×_kg^(-1)");
  // Electrical resistivity
  assert_parsed_expression_simplify_to("_kg×_m^3×_s^(-3)×_A^(-2)", "1×_Ω×_m");
  // Dipole moment
  assert_parsed_expression_simplify_to("_m^(-1)×_s×_A", "1×_C×_m^(-1)");
  // Electron mobility _m^2×_V^-1×_s^-1
  assert_parsed_expression_simplify_to("_kg^(-1)×_s^2×_A", "1×_T^(-1)");
  // Magnetic reluctance
  assert_parsed_expression_simplify_to("_m^(-2)×_kg^(-1)×_s^2×_A^2",
                                       "1×_H^(-1)");
  // Magnetic vector potential _Wb×_m^-1 and Magnetic rigidity _T×_m
  assert_parsed_expression_simplify_to("_m×_kg×_s^(-2)×_A^(-1)",
                                       "1×_N×_A^(-1)");
  // Magnetic moment
  assert_parsed_expression_simplify_to("_m^3×_kg×_s^(-2)×_A^(-1)", "1×_Wb×_m");
  // Magnetic susceptibility _H^-1×_m
  assert_parsed_expression_simplify_to("_m^(-1)×_kg^(-1)×_s^2×_A^2",
                                       "1×_N^(-1)×_A^2");

  // Physical constants
  // Gravitational constant G _N×_m^2×_kg^-2
  assert_parsed_expression_simplify_to("_kg^(-1)×_m^3×_s^(-2)",
                                       "1×_kg^(-1)×_m^3×_s^(-2)");
  // Vacuum electric permittivity µ0 _H×_m^-1
  assert_parsed_expression_simplify_to("_kg×_m×_s^(-2)×_A^(-2)",
                                       "1×_N×_A^(-2)");
  // Vacuum magnetic permeability 𝝴0
  assert_parsed_expression_simplify_to("_A^2×_s^4×_kg^(-1)×_m^(-3)",
                                       "1×_F×_m^(-1)");
  // Stefan–Boltzmann constant _W×_m^-2×_K^-4
  assert_parsed_expression_simplify_to("_kg×_s^(-3)×_K^(-4)",
                                       "1×_K^(-4)×_kg×_s^(-3)");

  /* Keep SI units for infinity float results, Remove unit for undefined
   * expression */
  assert_parsed_expression_simplify_to("inf×_s", "∞×_s");
  assert_parsed_expression_simplify_to("-inf×_oz", "-∞×_kg");
  assert_parsed_expression_simplify_to("2_s+3_s-5_s", "0×_s");
  assert_parsed_expression_simplify_to("log(0)×_s", "undef");
  assert_parsed_expression_simplify_to("log(undef)*_s", "undef");

  /* Units with invalid exponent */
  assert_parsed_expression_simplify_to("_s^(_s)", "undef");
  assert_parsed_expression_simplify_to("_s^(π)", "undef");

  /* Inhomogeneous expressions */
  assert_parsed_expression_simplify_to("1+_s", "undef");
  assert_parsed_expression_simplify_to("0+_A", "undef");
  assert_parsed_expression_simplify_to("i_A", "undef");
  assert_parsed_expression_simplify_to("_A+i", "undef");
  assert_parsed_expression_simplify_to("_m+_s", "undef");
  assert_parsed_expression_simplify_to("_m^2+_m", "undef");
  assert_parsed_expression_simplify_to("acos(_s)", "undef");
  assert_parsed_expression_simplify_to("arcosh(_s)", "undef");
  assert_parsed_expression_simplify_to("arg(_s)", "undef");
  assert_parsed_expression_simplify_to("asin(_s)", "undef");
  assert_parsed_expression_simplify_to("arsinh(_s)", "undef");
  assert_parsed_expression_simplify_to("arctan(_s)", "undef");
  assert_parsed_expression_simplify_to("artanh(_s)", "undef");
  assert_parsed_expression_simplify_to("binomcdf(_s,2,3)", "undef");
  assert_parsed_expression_simplify_to("binomcdf(2,_s,3)", "undef");
  assert_parsed_expression_simplify_to("binomcdf(2,3,_s)", "undef");
  assert_parsed_expression_simplify_to("binomial(_s,2)", "undef");
  assert_parsed_expression_simplify_to("binomial(2,_s)", "undef");
  assert_parsed_expression_simplify_to("binompdf(_s,2,3)", "undef");
  assert_parsed_expression_simplify_to("binompdf(2,_s,3)", "undef");
  assert_parsed_expression_simplify_to("binompdf(2,3,_s)", "undef");
  assert_parsed_expression_simplify_to("conj(_s)", "undef");
  assert_parsed_expression_simplify_to("cos(_s)", "undef");
  assert_parsed_expression_simplify_to("cosh(_s)", "undef");
  assert_parsed_expression_simplify_to("cross(_s,[[1][2][3]])", "undef");
  assert_parsed_expression_simplify_to("det(_s)", "undef");
  assert_parsed_expression_simplify_to("det([[_s]])", "undef");
  assert_parsed_expression_simplify_to("det([[cos(3gon)]])",
                                       "cos(\U000000123×π\U00000013/200)");
  assert_parsed_expression_simplify_to("diff(_s,x,0)", "undef");
  assert_parsed_expression_simplify_to("diff(0,x,_s)", "undef");
  assert_parsed_expression_simplify_to("dim(_s)", "undef");
  assert_parsed_expression_simplify_to("dot(_s,[[1][2][3]])", "undef");
  assert_parsed_expression_simplify_to("factor(_s)", "undef");
  assert_parsed_expression_simplify_to("(_s)!", "undef");
  assert_parsed_expression_simplify_to("frac(_s)", "undef");
  assert_parsed_expression_simplify_to("gcd(1,_s)", "undef");
  assert_parsed_expression_simplify_to("gcd(_s,1)", "undef");
  assert_parsed_expression_simplify_to("gcd(1,2,3,_s)", "undef");
  assert_parsed_expression_simplify_to("identity(_s)", "undef");
  assert_parsed_expression_simplify_to("im(_s)", "undef");
  assert_parsed_expression_simplify_to("int(_s,x,0,1)", "undef");
  assert_parsed_expression_simplify_to("int(0,x,_s,1)", "undef");
  assert_parsed_expression_simplify_to("int(0,x,0,_s)", "undef");
  assert_parsed_expression_simplify_to("invbinom(_s,2,3)", "undef");
  assert_parsed_expression_simplify_to("invbinom(2,_s,3)", "undef");
  assert_parsed_expression_simplify_to("invbinom(2,3,_s)", "undef");
  assert_parsed_expression_simplify_to("invnorm(_s,2,3)", "undef");
  assert_parsed_expression_simplify_to("invnorm(2,_s,3)", "undef");
  assert_parsed_expression_simplify_to("invnorm(2,3,_s)", "undef");
  assert_parsed_expression_simplify_to("inverse(_s)", "undef");
  assert_parsed_expression_simplify_to("lcm(1,_s)", "undef");
  assert_parsed_expression_simplify_to("lcm(_s,1)", "undef");
  assert_parsed_expression_simplify_to("lcm(1,2,3,_s)", "undef");
  assert_parsed_expression_simplify_to("ln(_s)", "undef");
  assert_parsed_expression_simplify_to("log(_s)", "undef");
  assert_parsed_expression_simplify_to("log(_s,2)", "undef");
  assert_parsed_expression_simplify_to("log(1,_s)", "undef");
  assert_parsed_expression_simplify_to("log(_N^2,_N)", "undef");
  assert_parsed_expression_simplify_to("norm(_s)", "undef");
  assert_parsed_expression_simplify_to("normcdf(_s,2,3)", "undef");
  assert_parsed_expression_simplify_to("normcdf(2,_s,3)", "undef");
  assert_parsed_expression_simplify_to("normcdf(2,3,_s)", "undef");
  assert_parsed_expression_simplify_to("normcdfrange(_s,2,3,4)", "undef");
  assert_parsed_expression_simplify_to("normcdfrange(2,_s,3,4)", "undef");
  assert_parsed_expression_simplify_to("normcdfrange(2,3,_s,4)", "undef");
  assert_parsed_expression_simplify_to("normcdfrange(2,3,4,_s)", "undef");
  assert_parsed_expression_simplify_to("normpdf(_s,2,3)", "undef");
  assert_parsed_expression_simplify_to("normpdf(2,_s,3)", "undef");
  assert_parsed_expression_simplify_to("normpdf(2,3,_s)", "undef");
  assert_parsed_expression_simplify_to("permute(_s,2)", "undef");
  assert_parsed_expression_simplify_to("permute(2,_s)", "undef");
  assert_parsed_expression_simplify_to("product(_s,x,0,1)", "undef");
  assert_parsed_expression_simplify_to("product(1,x,_s,1)", "undef");
  assert_parsed_expression_simplify_to("product(1,x,0,_s)", "undef");
  assert_parsed_expression_simplify_to("quo(_s,1)", "undef");
  assert_parsed_expression_simplify_to("quo(1,_s)", "undef");
  assert_parsed_expression_simplify_to("re(_s)", "undef");
  assert_parsed_expression_simplify_to("ref(_s)", "undef");
  assert_parsed_expression_simplify_to("ref([[_s]])", "undef");
  assert_parsed_expression_simplify_to("ref([[cos(3gon)]])", "[[1]]");
  assert_parsed_expression_simplify_to("rem(_s,1)", "undef");
  assert_parsed_expression_simplify_to("rem(1,_s)", "undef");
  assert_parsed_expression_simplify_to("round(1,_s)", "undef");
  assert_parsed_expression_simplify_to("round(1_s,_s)", "undef");
  assert_parsed_expression_simplify_to("round(1_s,0.1)", "undef");
  assert_parsed_expression_simplify_to("round(1_s,0.1)+abs(1_s)", "undef");
  assert_parsed_expression_simplify_to("rref(_s)", "undef");
  assert_parsed_expression_simplify_to("sin(_s)", "undef");
  assert_parsed_expression_simplify_to("sinh(_s)", "undef");
  assert_parsed_expression_simplify_to("sum(_s,x,0,1)", "undef");
  assert_parsed_expression_simplify_to("sum(0,x,_s,1)", "undef");
  assert_parsed_expression_simplify_to("sum(0,x,0,_s)", "undef");
  assert_parsed_expression_simplify_to("tan(_s)", "undef");
  assert_parsed_expression_simplify_to("tanh(_s)", "undef");
  assert_parsed_expression_simplify_to("trace(_s)", "undef");
  assert_parsed_expression_simplify_to("transpose(_s)", "undef");

  /* Valid expressions */
  assert_parsed_expression_simplify_to("-2×_A", "-2×_A");
  assert_parsed_expression_simplify_to("cos(1_s/1_s)", "cos(1)");
  assert_parsed_expression_simplify_to("sin(90_°)", "1", User, Radian);
  assert_parsed_expression_simplify_to("cos(π_rad/4)", "√(2)/2", User, Degree);
  assert_parsed_expression_simplify_to("cot((π/4)_rad)", "1", User, Gradian);
  assert_parsed_expression_simplify_to("1_m+π_m+√(2)_m-cos(15)_m",
                                       "6.3154941288217×_m");
  assert_parsed_expression_simplify_to("√(16×_m^2)", "4×_m");
  assert_parsed_expression_simplify_to("1×_A_kg", "2.2046226218488×_A×_lb",
                                       User, Radian, Imperial);
  assert_parsed_expression_simplify_to("abs(_s)", "1×_s");
  assert_parsed_expression_simplify_to("abs(3_m)", "3×_m");
  assert_parsed_expression_simplify_to("ceil(3.3_m)", "undef");
  assert_parsed_expression_simplify_to("floor(_s)", "undef");
  assert_parsed_expression_simplify_to("floor(3.3_m)", "undef");
  assert_parsed_expression_simplify_to("round(3.3_m, 0)", "undef");
  assert_parsed_expression_simplify_to("round(_s,1)", "undef");
  assert_parsed_expression_simplify_to("sign(-2_m)", "undef");
  assert_parsed_expression_simplify_to("sign(4_m)", "undef");
  assert_parsed_expression_simplify_to("sign(_s)", "undef");
  assert_parsed_expression_simplify_to(
      "abs(2_m) + ceil(3_m) + floor(4_m) + round(5_m, 1)", "undef");
  assert_parsed_expression_simplify_to("sign(3_m) + 2", "undef");
  assert_parsed_expression_simplify_to("1/_m+1/_km", "1.001×_m^(-1)");
  assert_parsed_expression_simplify_to("10000_kg", "10×_t");
  assert_parsed_expression_simplify_to("1000000_kg", "1×_kt");
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

QUIZ_CASE(poincare_simplification_factorial) {
  assert_parsed_expression_simplify_to("1/3!", "1/6");
  assert_parsed_expression_simplify_to("5!", "120");
  assert_parsed_expression_simplify_to("(1/3)!", "undef");
  assert_parsed_expression_simplify_to("π!", "undef");
  assert_parsed_expression_simplify_to("e!", "undef");
}

QUIZ_CASE(poincare_simplification_percent) {
  assert_parsed_expression_simplify_to("20%", "20/100");
  assert_parsed_expression_simplify_to("20%%", "(20/100)/100");
  assert_parsed_expression_simplify_to("80*20%", "80×20/100");
  assert_parsed_expression_simplify_to("80/(20%)", "80/(20/100)");
  assert_parsed_expression_simplify_to("80+20%", "80×(1+20/100)");
  assert_parsed_expression_simplify_to("20%+80+20%", "(80+20/100)×(1+20/100)");
  assert_parsed_expression_simplify_to("80+20%+20%",
                                       "(80×(1+20/100))×(1+20/100)");
  assert_parsed_expression_simplify_to("80-20%", "80×(1-20/100)");
  assert_parsed_expression_simplify_to("80+20-20%", "100×(1-20/100)");
  assert_parsed_expression_simplify_to("80+10*20%", "80+10×20/100");
  assert_parsed_expression_simplify_to("80-10*20%", "80-10×20/100");
  assert_parsed_expression_simplify_to("80+20%*10", "80+10×20/100");
  assert_parsed_expression_simplify_to("80-20%*10", "80-10×20/100");
  assert_parsed_expression_simplify_to("80+20%π", "80+π×20/100");
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

QUIZ_CASE(poincare_simplification_random) {
  assert_parsed_expression_simplify_to("1/random()+1/3+1/4", "7/12+1/random()");
  assert_parsed_expression_simplify_to("random()+random()",
                                       "random()+random()");
  assert_parsed_expression_simplify_to("random()-random()",
                                       "random()-random()");
  assert_parsed_expression_simplify_to("abs(random()-random())",
                                       "abs(random()-random())");
  assert_parsed_expression_simplify_to("1/random()+1/3+1/4+1/random()",
                                       "7/12+1/random()+1/random()");
  assert_parsed_expression_simplify_to("random()×random()",
                                       "random()×random()");
  assert_parsed_expression_simplify_to("random()/random()",
                                       "random()/random()");
  assert_parsed_expression_simplify_to("3^random()×3^random()",
                                       "3^(random()+random())");
  assert_parsed_expression_simplify_to("random()×ln(2)×3+random()×ln(2)×5",
                                       "(3×random()+5×random())×ln(2)");
}

QUIZ_CASE(poincare_simplification_randint) {
  // assert_parsed_expression_simplify_to("randint(1,1)", "1", User);
  //  Randint is not simplified if ReductionTarget = SystemForApproximation
  assert_parsed_expression_simplify_to(
      "randint(1,3)", "randint(1,3)", SystemForApproximation, Radian,
      MetricUnitFormat, Cartesian, ReplaceDefinedSymbols, false);
}

QUIZ_CASE(poincare_simplification_function) {
  assert_parsed_expression_simplify_to("abs(π)", "π");
  assert_parsed_expression_simplify_to("abs(-π)", "π");
  assert_parsed_expression_simplify_to("abs(1+i)",
                                       "√(2)");  // Fails because of metric
  assert_parsed_expression_simplify_to("abs(0)", "0");
  assert_parsed_expression_simplify_to("abs(x*y)", "abs(x)×abs(y)");
  assert_parsed_expression_simplify_to("abs(x^π)", "abs(x)^π");
  assert_parsed_expression_simplify_to("abs(i)", "1", SystemForApproximation,
                                       Radian, MetricUnitFormat, Cartesian,
                                       ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("abs(√(√(√(√(√(√(i)))))))", "1");
  assert_parsed_expression_simplify_to("abs(√(√(√(√(√(√(πi)))))))",
                                       "root(π,64)");
  assert_parsed_expression_simplify_to("arg(0)", "undef");
  assert_parsed_expression_simplify_to("arg(123)", "0");
  assert_parsed_expression_simplify_to("arg(-1ᴇ123)", "π");
  assert_parsed_expression_simplify_to("arg(i)", "π/2");
  assert_parsed_expression_simplify_to("arg(-6i)", "-π/2");
  assert_parsed_expression_simplify_to("arg(1+i)", "π/4");
  assert_parsed_expression_simplify_to("binomial(20,3)", "1140");
  assert_parsed_expression_simplify_to("binomial(20,10)", "184756");
  assert_parsed_expression_simplify_to("binomial(10,20)", "0");
  assert_parsed_expression_simplify_to("binomial(10.34,0)", "1");
  assert_parsed_expression_simplify_to("binomial(3.34,-1)", "0");
  assert_parsed_expression_simplify_to("binomial(-10,10)", "92378");
  assert_parsed_expression_simplify_to("binomial(2.5,3)", "binomial(5/2,3)");
  assert_parsed_expression_simplify_to("binomial(-200,120)",
                                       "binomial(-200,120)");
  assert_parsed_expression_simplify_to("binomial(400,1)", "binomial(400,1)");
  assert_parsed_expression_simplify_to("ceil(-1.3)", "-1");
  assert_parsed_expression_simplify_to("ceil(2π)", "7");
  assert_parsed_expression_simplify_to("ceil(123456789012345678901234567892/3)",
                                       "41152263004115226300411522631");
  assert_parsed_expression_simplify_to("ceil(123456789*π)", "387850942");
  assert_parsed_expression_simplify_to("conj(1/2)", "1/2");
  assert_parsed_expression_simplify_to("quo(19,3)", "6");
  assert_parsed_expression_simplify_to("quo(19,0)", "∞");
  assert_parsed_expression_simplify_to("quo(-19,3)", "-7");
  assert_parsed_expression_simplify_to("quo(19,0)", "∞");
  assert_parsed_expression_simplify_to("rem(19,3)", "1");
  assert_parsed_expression_simplify_to("rem(-19,3)", "2");
  assert_parsed_expression_simplify_to("rem(19,0)", "undef");
  assert_parsed_expression_simplify_to(
      "99!",
      "933262154439441526816992388562667004907159682643816214685929638952175999"
      "932299156089414639761565182862536979208272237582511852109168640000000000"
      "000000000000");
  assert_parsed_expression_simplify_to("factor(-10008/6895)",
                                       "-(2^3×3^2×139)/(5×7×197)");
  assert_parsed_expression_simplify_to("factor(1008/6895)",
                                       "(2^4×3^2)/(5×197)");
  assert_parsed_expression_simplify_to("factor(10007)", "10007");
  assert_parsed_expression_simplify_to("factor(10007^2)", "undef");
  assert_parsed_expression_simplify_to("factor(i)", "undef");
  assert_parsed_expression_simplify_to("floor(-1.3)", "-2");
  assert_parsed_expression_simplify_to("floor(2π)", "6");
  assert_parsed_expression_simplify_to(
      "floor(123456789012345678901234567892/3)",
      "41152263004115226300411522630");
  assert_parsed_expression_simplify_to("floor(123456789*π)", "387850941");
  assert_parsed_expression_simplify_to("frac(-1.3)", "7/10");
  assert_parsed_expression_simplify_to("gcd(123,278)", "1");
  assert_parsed_expression_simplify_to("gcd(11,121)", "11");
  assert_parsed_expression_simplify_to("gcd(56,112,28,91)", "7");
  assert_parsed_expression_simplify_to("gcd(-32,-32)", "32");
  assert_parsed_expression_simplify_to("im(1+5×i)", "5");
  assert_parsed_expression_simplify_to("lcm(123,278)", "34194");
  assert_parsed_expression_simplify_to("lcm(11,121)", "121");
  assert_parsed_expression_simplify_to("lcm(11,121, 3)", "363");
  assert_parsed_expression_simplify_to("lcm(-32,-32)", "32");
  assert_parsed_expression_simplify_to("√(4)", "2");
  assert_parsed_expression_simplify_to("re(1+5×i)", "1");
  assert_parsed_expression_simplify_to("root(4,3)", "root(4,3)");
  assert_parsed_expression_simplify_to("root(4,π)", "4^(1/π)");
  assert_parsed_expression_simplify_to("root(27,3)", "3");
  assert_parsed_expression_simplify_to("round(4.235,2)", "106/25");
  assert_parsed_expression_simplify_to("round(4.23,0)", "4");
  assert_parsed_expression_simplify_to("round(4.9,0)", "5");
  assert_parsed_expression_simplify_to("round(12.9,-1)", "10");
  assert_parsed_expression_simplify_to("round(12.9,-2)", "0");
  assert_parsed_expression_simplify_to("round(4.235)", "4");
  assert_parsed_expression_simplify_to("round(0.235)", "0");
  /* This tests that checks for overflow during its reduction. We need to input
   * the expression as 10^30*... so that the number 3.5E303 is reduced as a
   * Rational instead of being parsed as a Float because it's too long. */
  assert_parsed_expression_simplify_to(
      "round(10^30*10^30*10^30*10^10*10^10*10^30*10^30*10^30*10^30*10^30*10^30*"
      "10^13*3,5)",
      "round("
      "300000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000,5)");
  assert_parsed_expression_simplify_to("sign(-23)", "-1");
  assert_parsed_expression_simplify_to("sign(-i)", "undef");
  assert_parsed_expression_simplify_to("sign(0)", "0");
  assert_parsed_expression_simplify_to("sign(inf)", "1");
  assert_parsed_expression_simplify_to("sign(-inf)", "-1");
  assert_parsed_expression_simplify_to("sign(undef)", "undef");
  assert_parsed_expression_simplify_to("sign(23)", "1");
  assert_parsed_expression_simplify_to("sign(log(18))", "1");
  assert_parsed_expression_simplify_to("sign(-√(2))", "-1");
  assert_parsed_expression_simplify_to("sign(x)", "sign(x)");
  assert_parsed_expression_simplify_to("sign(2+i)", "undef");
  /* Test with no symbolic computation to check that n inside a sum expression
   * is not replaced by Undefined */
  assert_parsed_expression_simplify_to("sum(n,n,1,5)", "15", User, Radian,
                                       MetricUnitFormat, Cartesian,
                                       ReplaceAllSymbols);
  assert_parsed_expression_simplify_to("sum(1/n,n,1,2)", "3/2", User, Radian,
                                       MetricUnitFormat, Cartesian,
                                       ReplaceAllSymbols);
  assert_parsed_expression_simplify_to("permute(99,4)", "90345024");
  assert_parsed_expression_simplify_to("permute(20,-10)", "undef");
  assert_parsed_expression_simplify_to("re(1/2)", "1/2");
}

QUIZ_CASE(poincare_simplification_trigonometry_functions) {
  // -- sin/cos -> tan
  assert_parsed_expression_simplify_to("sin(x)/cos(x)", "tan(x)");
  assert_parsed_expression_simplify_to("cos(x)/sin(x)", "cot(x)");
  // 1/tan = cot if tan(x) != undef
  assert_parsed_expression_simplify_to("1/tan(x)",
                                       "dep(cot(x),{nonNull(cos(x))})");
  assert_parsed_expression_simplify_to("sin(x)×π/cos(x)", "π×tan(x)");
  assert_parsed_expression_simplify_to("sin(x)/(π×cos(x))", "tan(x)/π");
  assert_parsed_expression_simplify_to("1×tan(2)×tan(5)", "tan(2)×tan(5)");
  assert_parsed_expression_simplify_to("tan(62π/21)", "-tan(π/21)");
  assert_parsed_expression_simplify_to("cos(26π/21)/sin(25π/17)",
                                       "cos((5×π)/21)/sin((8×π)/17)");
  assert_parsed_expression_simplify_to("cos(62π/21)×π×3/sin(62π/21)",
                                       "-(3×π)/tan(π/21)");
  assert_parsed_expression_simplify_to("cos(62π/21)/(π×3×sin(62π/21))",
                                       "-1/(3×π×tan(π/21))");
  assert_parsed_expression_simplify_to("sin(62π/21)×π×3/cos(62π/21)",
                                       "-3×π×tan(π/21)");
  assert_parsed_expression_simplify_to("sin(62π/21)/(π×3cos(62π/21))",
                                       "-tan(π/21)/(3×π)");
  assert_parsed_expression_simplify_to("-cos(π/62)ln(3)/(sin(π/62)π)",
                                       "-ln(3)/(π×tan(π/62))");
  assert_parsed_expression_simplify_to("-2cos(π/62)ln(3)/(sin(π/62)π)",
                                       "-(2×cot(π/62)×ln(3))/π");
  // -- cos
  assert_parsed_expression_simplify_to("cos(0)", "1");
  assert_parsed_expression_simplify_to("cos(π)", "-1");
  assert_parsed_expression_simplify_to("cos(π×4/7)", "-cos((3×π)/7)");
  assert_parsed_expression_simplify_to("cos(π×35/29)", "-cos((6×π)/29)");
  assert_parsed_expression_simplify_to("cos(-π×35/29)", "-cos((6×π)/29)");
  assert_parsed_expression_simplify_to("cos(π×340000)", "1");
  assert_parsed_expression_simplify_to("cos(-π×340001)", "-1");
  assert_parsed_expression_simplify_to("cos(-π×√(2))", "cos(√(2)×π)");
  assert_parsed_expression_simplify_to("cos(1311π/6)", "0");
  assert_parsed_expression_simplify_to("cos(π/12)", "(√(2)+√(6))/4");
  assert_parsed_expression_simplify_to("cos(-π/12)", "(√(2)+√(6))/4");
  assert_parsed_expression_simplify_to("cos(-π17/8)", "√(2+√(2))/2");
  assert_parsed_expression_simplify_to("cos(41π/6)", "-√(3)/2");
  assert_parsed_expression_simplify_to("cos(π/4+1000π)", "√(2)/2");
  assert_parsed_expression_simplify_to("cos(-π/3)", "1/2");
  assert_parsed_expression_simplify_to("cos(41π/5)", "(1+√(5))/4");
  assert_parsed_expression_simplify_to("cos(7π/10)", "-(√(2)×√(5-√(5)))/4");
  assert_parsed_expression_simplify_to("cos(0)", "1", User, Degree);
  assert_parsed_expression_simplify_to("cos(180)", "-1", User, Degree);
  assert_parsed_expression_simplify_to("cos(720/7)", "-cos(540/7)", User,
                                       Degree);
  assert_parsed_expression_simplify_to("cos(6300/29)", "-cos(1080/29)", User,
                                       Degree);
  assert_parsed_expression_simplify_to("cos(-6300/29)", "-cos(1080/29)", User,
                                       Degree);
  assert_parsed_expression_simplify_to("cos(61200000)", "1", User, Degree);
  assert_parsed_expression_simplify_to("cos(-61200180)", "-1", User, Degree);
  assert_parsed_expression_simplify_to("cos(-180×√(2))", "cos(180×√(2))", User,
                                       Degree);
  assert_parsed_expression_simplify_to("cos(39330)", "0", User, Degree);
  assert_parsed_expression_simplify_to("cos(15)", "(√(2)+√(6))/4", User,
                                       Degree);
  assert_parsed_expression_simplify_to("cos(-15)", "(√(2)+√(6))/4", User,
                                       Degree);
  assert_parsed_expression_simplify_to("cos(-765/2)", "√(2+√(2))/2", User,
                                       Degree);
  assert_parsed_expression_simplify_to("cos(7380/6)", "-√(3)/2", User, Degree);
  assert_parsed_expression_simplify_to("cos(180045)", "√(2)/2", User, Degree);
  assert_parsed_expression_simplify_to("cos(-60)", "1/2", User, Degree);
  assert_parsed_expression_simplify_to("cos(7380/5)", "(1+√(5))/4", User,
                                       Degree);
  assert_parsed_expression_simplify_to("cos(112.5)", "-√(2-√(2))/2", User,
                                       Degree);
  // -- sin
  assert_parsed_expression_simplify_to("sin(0)", "0");
  assert_parsed_expression_simplify_to("sin(π)", "0");
  assert_parsed_expression_simplify_to("sin(π×35/29)", "-sin((6×π)/29)");
  assert_parsed_expression_simplify_to("sin(-π×35/29)", "sin((6×π)/29)");
  assert_parsed_expression_simplify_to("sin(π×340000)", "0");
  assert_parsed_expression_simplify_to("sin(π×340001)", "0");
  assert_parsed_expression_simplify_to("sin(-π×340001)", "0");
  assert_parsed_expression_simplify_to("sin(π/12)", "(√(6)-√(2))/4");
  assert_parsed_expression_simplify_to("sin(-π/12)", "(-√(6)+√(2))/4");
  assert_parsed_expression_simplify_to("sin(-π×√(2))", "-sin(√(2)×π)");
  assert_parsed_expression_simplify_to("sin(1311π/6)", "1");
  assert_parsed_expression_simplify_to("sin(-π17/8)", "-√(2-√(2))/2");
  assert_parsed_expression_simplify_to("sin(41π/6)", "1/2");
  assert_parsed_expression_simplify_to("sin(-3π/10)", "(-1-√(5))/4");
  assert_parsed_expression_simplify_to("sin(π/4+1000π)", "√(2)/2");
  assert_parsed_expression_simplify_to("sin(-π/3)", "-√(3)/2");
  assert_parsed_expression_simplify_to("sin(17π/5)", "-(√(2)×√(5+√(5)))/4");
  assert_parsed_expression_simplify_to("sin(π/5)", "(√(2)×√(5-√(5)))/4");
  assert_parsed_expression_simplify_to("sin(0)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(6300/29)", "-sin(1080/29)", User,
                                       Degree);
  assert_parsed_expression_simplify_to("sin(-6300/29)", "sin(1080/29)", User,
                                       Degree);
  assert_parsed_expression_simplify_to("sin(61200000)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(61200180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(-61200180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(15)", "(√(6)-√(2))/4", User,
                                       Degree);
  assert_parsed_expression_simplify_to("sin(-15)", "(-√(6)+√(2))/4", User,
                                       Degree);
  assert_parsed_expression_simplify_to("sin(-180×√(2))", "-sin(180×√(2))", User,
                                       Degree);
  assert_parsed_expression_simplify_to("sin(39330)", "1", User, Degree);
  assert_parsed_expression_simplify_to("sin(-765/2)", "-√(2-√(2))/2", User,
                                       Degree);
  assert_parsed_expression_simplify_to("sin(1230)", "1/2", User, Degree);
  assert_parsed_expression_simplify_to("sin(180045)", "√(2)/2", User, Degree);
  assert_parsed_expression_simplify_to("sin(-60)", "-√(3)/2", User, Degree);
  assert_parsed_expression_simplify_to("sin(612)", "-(√(2)×√(5+√(5)))/4", User,
                                       Degree);
  assert_parsed_expression_simplify_to("sin(36)", "(√(2)×√(5-√(5)))/4", User,
                                       Degree);
  // -- tan
  assert_parsed_expression_simplify_to("tan(0)", "0");
  assert_parsed_expression_simplify_to("tan(π)", "0");
  assert_parsed_expression_simplify_to("tan(3×π/2)", "undef");
  assert_parsed_expression_simplify_to("tan(π/2)", "undef");
  assert_parsed_expression_simplify_to("tan(90)", "undef", User, Degree);
  assert_parsed_expression_simplify_to("tan(100)", "undef", User, Gradian);
  assert_parsed_expression_simplify_to("tan(π×35/29)", "tan((6×π)/29)");
  assert_parsed_expression_simplify_to("tan(-π×35/29)", "-tan((6×π)/29)");
  assert_parsed_expression_simplify_to("tan(π×340000)", "0");
  assert_parsed_expression_simplify_to("tan(π×340001)", "0");
  assert_parsed_expression_simplify_to("tan(-π×340001)", "0");
  assert_parsed_expression_simplify_to("tan(π/12)", "2-√(3)");
  assert_parsed_expression_simplify_to("tan(-π/12)", "-2+√(3)");
  assert_parsed_expression_simplify_to("tan(-π×√(2))", "-tan(√(2)×π)");
  assert_parsed_expression_simplify_to("tan(1311π/6)", "undef");
  assert_parsed_expression_simplify_to("tan(-π17/8)", "1-√(2)");
  assert_parsed_expression_simplify_to("tan(41π/6)", "-√(3)/3");
  assert_parsed_expression_simplify_to("tan(π/4+1000π)", "1");
  assert_parsed_expression_simplify_to("tan(-π/3)", "-√(3)");
  /*  Currently simplifies to -((-1+√(5))×√(2-(2×√(5))/5))/4.
   * TODO: contract to -√(1-2×√(5)/5) */
  assert_parsed_expression_simplify_to("tan(-π/10)", "-√(1-2×√(5)/5)");
  assert_parsed_expression_simplify_to("tan(0)", "0", User, Degree);
  assert_parsed_expression_simplify_to("tan(180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("tan(6300/29)", "tan(1080/29)", User,
                                       Degree);
  assert_parsed_expression_simplify_to("tan(-6300/29)", "-tan(1080/29)", User,
                                       Degree);
  assert_parsed_expression_simplify_to("tan(61200000)", "0", User, Degree);
  assert_parsed_expression_simplify_to("tan(61200180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("tan(-61200180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("tan(15)", "2-√(3)", User, Degree);
  assert_parsed_expression_simplify_to("tan(-15)", "-2+√(3)", User, Degree);
  assert_parsed_expression_simplify_to("tan(-180×√(2))", "-tan(180×√(2))", User,
                                       Degree);
  assert_parsed_expression_simplify_to("tan(39330)", "undef", User, Degree);
  assert_parsed_expression_simplify_to("tan(-382.5)", "1-√(2)", User, Degree);
  assert_parsed_expression_simplify_to("tan(1230)", "-√(3)/3", User, Degree);
  assert_parsed_expression_simplify_to("tan(180045)", "1", User, Degree);
  assert_parsed_expression_simplify_to("tan(-60)", "-√(3)", User, Degree);
  assert_parsed_expression_simplify_to("tan(tan(tan(tan(9))))",
                                       "tan(tan(tan(tan(9))))");
  // 1/tan = cot if tan(x) != undef
  assert_parsed_expression_simplify_to("1/tan(2)", "cot(2)");
  assert_parsed_expression_simplify_to("tan(3)*sin(3)/cos(3)", "tan(3)^2");
  // -- acos
  assert_parsed_expression_simplify_to("acos(-1/2)", "(2×π)/3");
  assert_parsed_expression_simplify_to("acos(-1.2)", "-arccos(6/5)+π");
  assert_parsed_expression_simplify_to("acos(cos(2/3))", "2/3");
  assert_parsed_expression_simplify_to("acos(cos(3/2))", "3/2");
  assert_parsed_expression_simplify_to("cos(acos(3/2))", "3/2");
  assert_parsed_expression_simplify_to("cos(acos(2/3))", "2/3");

  assert_parsed_expression_simplify_to("acos(cos(12))", "-12+4×π");
  assert_parsed_expression_simplify_to("acos(cos(2*1ᴇ10))",
                                       "arccos(cos(20000000000))");
  assert_parsed_expression_simplify_to("acos(cos(inf))", "arccos(cos(∞))");
  assert_parsed_expression_simplify_to("acos(cos(9))", "9-2×π");
  assert_parsed_expression_simplify_to("acos(cos(10^125))",
                                       "arccos(cos(10^125))");
  assert_parsed_expression_simplify_to("acos(cos(1/0))", "undef");
  assert_parsed_expression_simplify_to("acos(cos(-8.8))", "44/5-2×π");
  assert_parsed_expression_simplify_to("acos(cos(π+26))", "-26+9×π");
  assert_parsed_expression_simplify_to("acos(cos(0))", "0");
  assert_parsed_expression_simplify_to("acos(cos(9π))", "π");
  assert_parsed_expression_simplify_to("acos(cos(2*1ᴇ10))", "160", User,
                                       Degree);
  assert_parsed_expression_simplify_to("acos(cos(180+50))", "130", User,
                                       Degree);
  assert_parsed_expression_simplify_to(
      "arccos(cos(2345995537929342462976×π^5-36850577388590589246720×π^4+"
      "231537533966682879807360×π^3-727391989955238208647840×π^2+"
      "1142577399842170168717980×π-717897987691852588770249))",
      "arccos(cos(2345995537929342462976×π^5-36850577388590589246720×π^4+"
      "231537533966682879807360×π^3-727391989955238208647840×π^2+"
      "1142577399842170168717980×π-717897987691852588770249))");

  assert_parsed_expression_simplify_to("acos(cos(4π/7))", "(4×π)/7");
  assert_parsed_expression_simplify_to("acos(-cos(2))", "π-2");
  assert_parsed_expression_simplify_to("acos(-1/2)", "120", User, Degree);
  assert_parsed_expression_simplify_to("acos(-1.2)", "-arccos(6/5)+180", User,
                                       Degree);
  assert_parsed_expression_simplify_to("acos(cos(2/3))", "2/3", User, Degree);
  assert_parsed_expression_simplify_to("acos(cos(190))", "170", User, Degree);
  assert_parsed_expression_simplify_to("acos(cos(75))", "75", User, Degree);
  assert_parsed_expression_simplify_to("cos(acos(190))", "190", User, Degree);
  assert_parsed_expression_simplify_to("cos(acos(75))", "75", User, Degree);
  assert_parsed_expression_simplify_to("acos(cos(12))", "12", User, Degree);
  assert_parsed_expression_simplify_to("acos(cos(720/7))", "720/7", User,
                                       Degree);
  // -- asin
  assert_parsed_expression_simplify_to("asin(-1/2)", "-π/6");
  assert_parsed_expression_simplify_to("asin(-1.2)", "-arcsin(6/5)");
  assert_parsed_expression_simplify_to("asin(sin(2/3))", "2/3");
  assert_parsed_expression_simplify_to("sin(asin(2/3))", "2/3");
  assert_parsed_expression_simplify_to("sin(asin(3/2))", "3/2");
  assert_parsed_expression_simplify_to("asin(sin(3/2))", "3/2");
  assert_parsed_expression_simplify_to("asin(sin(3.6))", "-18/5+π");
  assert_parsed_expression_simplify_to("asin(sin(-2.23))", "223/100-π");
  assert_parsed_expression_simplify_to("asin(sin(-18.39))", "-1839/100+6×π");

  assert_parsed_expression_simplify_to("asin(sin(12))", "12-4×π");
  assert_parsed_expression_simplify_to("asin(sin(2+π))", "2-π");
  assert_parsed_expression_simplify_to("asin(sin(90+6800))", "50", User,
                                       Degree);
  assert_parsed_expression_simplify_to("asin(sin(60-9×9×9))", "51", User,
                                       Degree);

  assert_parsed_expression_simplify_to("asin(sin(-π/7))", "-π/7");
  assert_parsed_expression_simplify_to("asin(sin(-√(2)))", "-√(2)");
  assert_parsed_expression_simplify_to("asin(-1/2)", "-30", User, Degree);
  assert_parsed_expression_simplify_to("asin(-1.2)", "-arcsin(6/5)", User,
                                       Degree);
  assert_parsed_expression_simplify_to("asin(sin(75))", "75", User, Degree);
  assert_parsed_expression_simplify_to("sin(asin(75))", "75", User, Degree);
  assert_parsed_expression_simplify_to("sin(asin(190))", "190", User, Degree);
  assert_parsed_expression_simplify_to("asin(sin(32))", "32", User, Degree);
  assert_parsed_expression_simplify_to("asin(sin(400))", "40", User, Degree);
  assert_parsed_expression_simplify_to("asin(sin(-180/7))", "-180/7", User,
                                       Degree);
  assert_parsed_expression_simplify_to("arcsin(sin(10^10))",
                                       "arcsin(sin(10000000000))");
  // -- atan
  assert_parsed_expression_simplify_to("atan(-1)", "-π/4");
  assert_parsed_expression_simplify_to("atan(-1.2)", "-arctan(6/5)");
  assert_parsed_expression_simplify_to("atan(tan(2/3))", "2/3");
  assert_parsed_expression_simplify_to("tan(atan(2/3))", "2/3");
  // If target != User, sin/cos is not reduced to tan(x)
  assert_parsed_expression_simplify_to(
      "atan(sin(2/3)/cos(2/3))", "2/3", SystemForAnalysis, Radian,
      MetricUnitFormat, Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("tan(atan(5/2))", "5/2");
  assert_parsed_expression_simplify_to("atan(tan(5/2))", "5/2-π");
  assert_parsed_expression_simplify_to("atan(tan(-π/7))", "-π/7");
  assert_parsed_expression_simplify_to("atan(√(3))", "π/3");
  assert_parsed_expression_simplify_to("atan(tan(-√(2)))", "-√(2)");
  assert_parsed_expression_simplify_to("atan(-1)", "-45", User, Degree);
  assert_parsed_expression_simplify_to("atan(-1.2)", "-arctan(6/5)", User,
                                       Degree);
  assert_parsed_expression_simplify_to("atan(tan(-45))", "-45", User, Degree);
  assert_parsed_expression_simplify_to("tan(atan(120))", "120", User, Degree);
  assert_parsed_expression_simplify_to("tan(atan(2293))", "2293", User, Degree);
  assert_parsed_expression_simplify_to("atan(tan(2293))", "-47", User, Degree);
  assert_parsed_expression_simplify_to("atan(tan(1808))", "8", User, Degree);
  assert_parsed_expression_simplify_to("atan(tan(-180/7))", "-180/7", User,
                                       Degree);
  assert_parsed_expression_simplify_to("atan(√(3))", "60", User, Degree);
  assert_parsed_expression_simplify_to("atan(∞)", "π/2", User, Radian);
  assert_parsed_expression_simplify_to("atan(-∞)", "-π/2", User, Radian);
  assert_parsed_expression_simplify_to("tan(atan(∞))", "undef", User, Radian);
  assert_parsed_expression_simplify_to("atan(1/x)",
                                       "dep((π×sign(x)-2×arctan(x))/2,{1/x})");
  assert_parsed_expression_simplify_to(
      "atan(1/x)", "dep(90×sign(x)-arctan(x),{1/x})", User, Degree);
  assert_parsed_expression_simplify_to(
      "atan(1/x)", "dep(100×sign(x)-arctan(x),{1/x})", User, Gradian);
  assert_parsed_expression_simplify_to("atan(cos(x)/sin(x))", "arctan(cot(x))");
  assert_parsed_expression_simplify_to("atan(cos(π/7)/sin(π/7))",
                                       "\U000000125×π\U00000013/14");
  assert_parsed_expression_simplify_to("atan(cos(4)/sin(4))",
                                       "\U000000123×π-8\U00000013/2");
  assert_parsed_expression_simplify_to("atan(cos(1.57079632)/sin(1.57079632))",
                                       "arctan(cot(9817477/6250000))");

  // cos(asin)
  assert_parsed_expression_simplify_to("cos(asin(x))", "√(-x^2+1)", User,
                                       Degree);
  assert_parsed_expression_simplify_to("cos(asin(-x))", "√(-x^2+1)", User,
                                       Degree);
  // cos(atan)
  assert_parsed_expression_simplify_to("cos(atan(x))", "1/√(x^2+1)", User,
                                       Degree);
  assert_parsed_expression_simplify_to("cos(atan(-x))", "1/√(x^2+1)", User,
                                       Degree);
  // sin(acos)
  assert_parsed_expression_simplify_to("sin(acos(x))", "√(-x^2+1)", User,
                                       Degree);
  assert_parsed_expression_simplify_to("sin(acos(-x))", "√(-x^2+1)", User,
                                       Degree);
  // sin(atan)
  assert_parsed_expression_simplify_to("sin(atan(x))", "x/√(x^2+1)", User,
                                       Degree);
  assert_parsed_expression_simplify_to("sin(atan(-x))", "-x/√(x^2+1)", User,
                                       Degree);
  // tan(acos)
  assert_parsed_expression_simplify_to("tan(acos(x))", "√(-x^2+1)/x", User,
                                       Degree);
  assert_parsed_expression_simplify_to("tan(acos(-x))", "-√(-x^2+1)/x", User,
                                       Degree);
  // tan(asin)
  assert_parsed_expression_simplify_to("tan(asin(x))", "x/√(-x^2+1)", User,
                                       Degree);
  assert_parsed_expression_simplify_to("tan(asin(-x))", "-x/√(-x^2+1)", User,
                                       Degree);

  // Mix
  assert_parsed_expression_simplify_to("sin(atan(3/4))", "3/5", User, Degree);
}

QUIZ_CASE(poincare_simplification_matrix) {
  // OMatrix can't contain matrix or lists
  assert_parsed_expression_simplify_to("[[[[1,2][3,4]],2][3,4]]", "undef");
  assert_parsed_expression_simplify_to("[[{9,8,7},2][3,4]]", "undef");

  // OMatrix don't bubble-up undefined/unreal or unit
  assert_parsed_expression_simplify_to("[[1,1/0][3,4]]", "[[1,undef][3,4]]");
  assert_parsed_expression_simplify_to("[[1,(-1)^(1/2)][3,4]]",
                                       "[[1,nonreal][3,4]]", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("[[1,3_kg][3,4]]", "undef", User, Radian,
                                       MetricUnitFormat, Real);

  // Addition OMatrix
  assert_parsed_expression_simplify_to("1+[[1,2,3][4,5,6]]", "undef");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]+1", "undef");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]+[[1,2,3][4,5,6]]",
                                       "undef");
  assert_parsed_expression_simplify_to("2+[[1,2,3][4,5,6]]+[[1,2,3][4,5,6]]",
                                       "undef");
  assert_parsed_expression_simplify_to(
      "[[1,2,3][4,5,6]]+cos(2)+[[1,2,3][4,5,6]]", "undef");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]+[[1,2,3][4,5,6]]",
                                       "[[2,4,6][8,10,12]]");

  // Multiplication OMatrix
  assert_parsed_expression_simplify_to("2*[[1,2,3][4,5,6]]",
                                       "[[2,4,6][8,10,12]]");
  assert_parsed_expression_simplify_to(
      "[[1,2,3][4,5,6]]×√(2)", "[[√(2),2×√(2),3×√(2)][4×√(2),5×√(2),6×√(2)]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]*[[1,2,3][4,5,6]]",
                                       "[[9,12,15][19,26,33]]");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]*[[1,2][2,3][5,6]]",
                                       "[[20,26][44,59]]");
  assert_parsed_expression_simplify_to("[[1,2,3,4][4,5,6,5]]*[[1,2][2,3][5,6]]",
                                       "undef");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)*[[1,2][3,4]]",
                                       "[[11/2,-5/2][-15/4,7/4]]");
  assert_parsed_expression_simplify_to(
      "[[1,2][3,4]]^(-3)*[[1,2,3][3,4,5]]*[[1,2][3,2][4,5]]*4",
      "[[-176,-186][122,129]]");

  // Power OMatrix
  assert_parsed_expression_simplify_to(
      "[[1,2,3][4,5,6][7,8,9]]^3",
      "[[468,576,684][1062,1305,1548][1656,2034,2412]]");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]^(-1)", "undef");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-1)",
                                       "[[-2,1][3/2,-1/2]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^0", "[[1,0][0,1]]");
  assert_parsed_expression_simplify_to("[[1][2]]^0", "undef");
  assert_parsed_expression_simplify_to("[[1][2]]^1", "undef");

  // Determinant
  assert_parsed_expression_simplify_to("det(π+π)", "undef");
  assert_parsed_expression_simplify_to("det([[π+π]])", "2×π");
  assert_parsed_expression_simplify_to("det([[1,2][3,4]])", "-2");
  assert_parsed_expression_simplify_to("det([[2,2][3,4]])", "2");
  assert_parsed_expression_simplify_to("det([[2,2][3,4][3,4]])", "undef");
  assert_parsed_expression_simplify_to("det([[2,2][3,3]])", "0");
  assert_parsed_expression_simplify_to("det([[1,2,3][4,5,6][7,8,9]])", "0");
  assert_parsed_expression_simplify_to("det([[1,2,3][4,5,6][7,8,9]])", "0");
  assert_parsed_expression_simplify_to("det([[1,2,3][4π,5,6][7,8,9]])",
                                       "24×π-24");
  assert_parsed_expression_simplify_to("det(identity(5))", "1");

  // Dimension
  assert_parsed_expression_simplify_to("dim(3)", "undef");
  assert_parsed_expression_simplify_to("dim([[1/√(2),1/2,3][2,1,-3]])",
                                       "[[2,3]]");

  // Inverse
  assert_parsed_expression_simplify_to("inverse([[1/√(2),1/2,3][2,1,-3]])",
                                       "undef");
  assert_parsed_expression_simplify_to("inverse([[1,2][3,4]])",
                                       "[[-2,1][3/2,-1/2]]");
  assert_parsed_expression_simplify_to("inverse([[π,2×π][3,2]])",
                                       "[[-1/(2×π),1/2][3/(4×π),-1/4]]");

  // Divison : should be undefined
  assert_parsed_expression_simplify_to("[[1,2][3,4]]/[[1,2][-2,3]]", "undef");
  assert_parsed_expression_simplify_to("1/identity(2)^500", "undef");

  // Trace
  assert_parsed_expression_simplify_to("trace([[1/√(2),1/2,3][2,1,-3]])",
                                       "undef");
  assert_parsed_expression_simplify_to("trace([[√(2),2][4,3+log(3)]])",
                                       "log(3)+3+√(2)");
  assert_parsed_expression_simplify_to("trace(√(2)+log(3))", "undef");

  // Transpose
  assert_parsed_expression_simplify_to("transpose([[1/√(2),1/2,3][2,1,-3]])",
                                       "[[√(2)/2,2][1/2,1][3,-3]]");
  assert_parsed_expression_simplify_to("transpose(√(4))", "undef");

  // Ref and Rref
  assert_parsed_expression_simplify_to("ref([[1,1/√(2),√(4)]])",
                                       "[[1,√(2)/2,2]]");
  assert_parsed_expression_simplify_to("rref([[1,1/√(2),√(4)]])",
                                       "[[1,√(2)/2,2]]");
  assert_parsed_expression_simplify_to("ref([[1,0,√(4)][0,1,1/√(2)][0,0,1]])",
                                       "[[1,0,2][0,1,√(2)/2][0,0,1]]");
  assert_parsed_expression_simplify_to("rref([[1,0,√(4)][0,1,1/√(2)][0,0,0]])",
                                       "[[1,0,2][0,1,√(2)/2][0,0,0]]");
  assert_parsed_expression_simplify_to(
      "ref([[1,0,3,4][5,7,6,8][0,10,11,12]])",
      "[[1,7/5,6/5,8/5][0,1,11/10,6/5][0,0,1,204/167]]");
  assert_parsed_expression_simplify_to(
      "rref([[1,0,3,4][5,7,6,8][0,10,11,12]])",
      "[[1,0,0,56/167][0,1,0,-24/167][0,0,1,204/167]]");
  assert_parsed_expression_simplify_to("ref([[1,0][5,6][0,10]])",
                                       "[[1,6/5][0,1][0,0]]");
  assert_parsed_expression_simplify_to("rref([[1,0][5,6][0,10]])",
                                       "[[1,0][0,1][0,0]]");
  assert_parsed_expression_simplify_to("ref([[0,0][0,0][0,0]])",
                                       "[[0,0][0,0][0,0]]");
  assert_parsed_expression_simplify_to("rref([[0,0][0,0][0,0]])",
                                       "[[0,0][0,0][0,0]]");
  assert_parsed_expression_simplify_to("rref([[0,1][1ᴇ-100,1]])",
                                       "[[1,0][0,1]]");
  assert_parsed_expression_simplify_to("ref([[0,2,-1][5,6,7][12,11,10]])",
                                       "[[1,11/12,5/6][0,1,-1/2][0,0,1]]");
  assert_parsed_expression_simplify_to("rref([[0,2,-1][5,6,7][12,11,10]])",
                                       "[[1,0,0][0,1,0][0,0,1]]");
  /* Results for ref depend on the implementation. In any case :
   * - Rows with only zeros must be at the bottom.
   * - Leading coefficient of other rows must be to the right (strictly) of the
   * - one above.
   * - (Optional, but sometimes recommended) Leading coefficients must be 1. */
  assert_parsed_expression_simplify_to("ref([[3,9][2,5]])", "[[1,3][0,1]]");
  assert_parsed_expression_simplify_to("ref([[3,2][5,7]])", "[[1,7/5][0,1]]");
  assert_parsed_expression_simplify_to("ref([[3,11][5,7]])", "[[1,7/5][0,1]]");
  assert_parsed_expression_simplify_to("ref([[2,5][2,7]])", "[[1,5/2][0,1]]");
  assert_parsed_expression_simplify_to("ref([[3,12][-4,1]])",
                                       "[[1,-1/4][0,1]]");
  assert_parsed_expression_simplify_to("ref([[0,1][1ᴇ-100,1]])",
                                       "[[1,10^100][0,1]]");
  // Can't canonize if a child cannot be approximated
  assert_parsed_expression_simplify_to("ref([[0,0][x,0]])",
                                       "ref([[0,0][x,0]])");

  // Cross product
  assert_parsed_expression_simplify_to("cross([[0][1/√(2)][0]],[[0][0][1]])",
                                       "[[√(2)/2][0][0]]");
  assert_parsed_expression_simplify_to("cross([[1,2,3]],[[4][7][8]])", "undef");
  assert_parsed_expression_simplify_to("cross([[1,2,3]],[[4,7,8]])",
                                       "[[-5,4,-1]]");
  assert_parsed_expression_simplify_to("cross([[1,π,i]],[[iπ,iπ^2,-π]])",
                                       "[[0,0,0]]");

  // Dot product
  assert_parsed_expression_simplify_to("dot([[1/√(2)][0][0]],[[1][0][0]])",
                                       "√(2)/2");
  assert_parsed_expression_simplify_to("dot([[1,1,0]],[[0][0][1]])", "undef");
  assert_parsed_expression_simplify_to("dot([[1,1,0]],[[0,0,1]])", "0");
  assert_parsed_expression_simplify_to("dot([[1,1,1]],[[0,π,i]])", "π+i");

  // Vector norm
  assert_parsed_expression_simplify_to("norm([[1/√(2)][0][0]])", "√(2)/2");
  assert_parsed_expression_simplify_to("norm([[1][2][3]])", "√(14)");
  assert_parsed_expression_simplify_to("norm([[1,i+1,π,-5]])", "√(π^2+28)");
}

QUIZ_CASE(poincare_simplification_functions_of_matrices) {
  assert_parsed_expression_simplify_to("abs([[1,-1][2,-3]])", "undef");
  assert_parsed_expression_simplify_to("acos([[1/√(2),1/2][1,-1]])", "undef");
  assert_parsed_expression_simplify_to("asin([[1,0]])", "undef");
  assert_parsed_expression_simplify_to("binomial([[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("binomial(1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("binomial([[0,180]],[[1]])", "undef");
  assert_parsed_expression_simplify_to("ceil([[0.3,180]])", "undef");
  assert_parsed_expression_simplify_to("arg([[1,1+i]])", "undef");
  assert_parsed_expression_simplify_to("conj([[1,1+i]])", "undef");
  assert_parsed_expression_simplify_to("cos([[π/3,0][π/7,π/2]])", "undef");
  assert_parsed_expression_simplify_to("cos([[0,π]])", "undef");
  assert_parsed_expression_simplify_to("diff([[0,180]],x,1)", "undef");
  assert_parsed_expression_simplify_to("diff(1,x,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("quo([[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("quo(1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("quo([[0,180]],[[1]])", "undef");
  assert_parsed_expression_simplify_to("rem([[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("rem(1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("rem([[0,180]],[[1]])", "undef");
  assert_parsed_expression_simplify_to("factor([[0,180]])", "undef");
  assert_parsed_expression_simplify_to("[[1,3]]!", "undef");
  assert_parsed_expression_simplify_to("floor([[1/√(2),1/2][1,-1.3]])",
                                       "undef");
  assert_parsed_expression_simplify_to("frac([[0.3,180]])", "undef");
  assert_parsed_expression_simplify_to("gcd([[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("gcd(1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("gcd([[0,180]],[[1]])", "undef");
  assert_parsed_expression_simplify_to("gcd(1,2,[[1]])", "undef");
  assert_parsed_expression_simplify_to("arsinh([[0,π]])", "undef");
  assert_parsed_expression_simplify_to("artanh([[0,π]])", "undef");
  assert_parsed_expression_simplify_to("sinh([[0,π]])", "undef");
  assert_parsed_expression_simplify_to("im([[1,1+i]])", "undef");
  assert_parsed_expression_simplify_to("int([[0,180]],x,1,2)", "undef");
  assert_parsed_expression_simplify_to("int(1,x,[[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("int(1,x,1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("log([[2,3]])", "undef");
  assert_parsed_expression_simplify_to("log(5,[[2,3]])", "undef");
  assert_parsed_expression_simplify_to("ln([[2,3]])", "undef");
  assert_parsed_expression_simplify_to("root([[2,3]],5)", "undef");
  assert_parsed_expression_simplify_to("root(5,[[2,3]])", "undef");
  // Opposite is mapped on matrix
  assert_parsed_expression_simplify_to("-[[1/√(2),1/2,3][2,1,-3]]",
                                       "[[-√(2)/2,-1/2,-3][-2,-1,3]]");
  assert_parsed_expression_simplify_to("permute([[2,3]],5)", "undef");
  assert_parsed_expression_simplify_to("permute(5,[[2,3]])", "undef");
  assert_parsed_expression_simplify_to("product(1,x,[[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("product(1,x,1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("re([[1,i]])", "undef");
  assert_parsed_expression_simplify_to("round(1.3, [[2.1,3.4]])", "undef");
  assert_parsed_expression_simplify_to("round(1.3, [[2.1,3.4]])", "undef");
  assert_parsed_expression_simplify_to("sign([[2.1,3.4]])", "undef");
  assert_parsed_expression_simplify_to("sin([[π/3,0][π/7,π/2]])", "undef");
  assert_parsed_expression_simplify_to("sum(1,x,[[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("sum(1,x,1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("√([[2.1,3.4]])", "undef");
  assert_parsed_expression_simplify_to("[[2,3.4]]-[[0.1,3.1]]",
                                       "[[19/10,3/10]]");
  assert_parsed_expression_simplify_to("[[2,3.4]]-1", "undef");
  assert_parsed_expression_simplify_to("1-[[0.1,3.1]]", "undef");
}

QUIZ_CASE(poincare_simplification_store) {
  assert_parsed_expression_simplify_to("1+2→x", "3→x");
#if TODO_PCJ
  assert_parsed_expression_simplify_to(
      "0.2→f(x)", "0.2→f(x)", SystemForAnalysis, Radian, MetricUnitFormat,
      Cartesian, ReplaceDefinedSymbols, false, Radian, MetricUnitFormat,
      Cartesian, ReplaceDefinedSymbols, false);
#endif
  assert_parsed_expression_simplify_to("0.1+0.2→x", "3/10→x");
  assert_parsed_expression_simplify_to("a→x", "a→x");
  assert_parsed_expression_simplify_to("a→x", "undef→x", User, Radian,
                                       MetricUnitFormat, Cartesian,
                                       ReplaceAllSymbols);
}

QUIZ_CASE(poincare_simplification_store_matrix) {
  assert_parsed_expression_simplify_to("1+1→a", "2→a");
#if TODO_PCJ
  assert_parsed_expression_simplify_to("[[8]]→f(x)", "[[8]]→f(x)");
  assert_parsed_expression_simplify_to("[[x]]→f(x)", "[[x]]→f(x)");
#endif
}

QUIZ_CASE(poincare_simplification_store_correctly_parsed) {
  PoincareTest::SymbolStore symbolStore;
  assert_parsed_expression_simplify_to("abc", "a×b×c", symbolStore);
  assert_parsed_expression_simplify_to("\"abc\"", "\"abc\"", symbolStore);
  store("2→a", symbolStore);
  store("5→bc", symbolStore);
  assert_parsed_expression_simplify_to("abc", "10", symbolStore);  // a*bc
  assert_parsed_expression_simplify_to("aa", "4", symbolStore);
  store("10→aa", symbolStore);
  assert_parsed_expression_simplify_to("aa", "10", symbolStore);
  assert_parsed_expression_simplify_to("aaa", "20",
                                       symbolStore);  // Parsed to a*aa
  assert_parsed_expression_simplify_to("aaaaa", "200",
                                       symbolStore);  // Parsed to a*aa*aa
  assert_parsed_expression_simplify_to("acos(b)", "arccos(b)", symbolStore);
  assert_parsed_expression_simplify_to("aacos(b)", "2×arccos(b)", symbolStore);
  store("t→bar(t)", symbolStore);
  store("8→foo", symbolStore);
  assert_parsed_expression_simplify_to("foobar(x)", "8×x", symbolStore);
  store("t^2→foobar(t)", symbolStore);
  assert_parsed_expression_simplify_to("foobar(x)", "x^2", symbolStore);

  assert_parsed_expression_simplify_to("t", "1×_t", symbolStore);
  store("2→t", symbolStore);
  assert_parsed_expression_simplify_to("t", "2", symbolStore);
}

QUIZ_CASE(poincare_simplification_unit_convert) {
  assert_parsed_expression_simplify_to("10_m/_s→_km/_h",
                                       "36×_km×_h^\x12-1\x13");
  assert_parsed_expression_simplify_to("2_m→_km×_m/_m", "0.002×_km");
  assert_parsed_expression_simplify_to("10_m/_s→_km/_h",
                                       "36×_km×_h^\x12-1\x13");
  assert_parsed_expression_simplify_to("10_m^2→_mm×_km", "10×_km×_mm");
  assert_parsed_expression_simplify_to("2_h+2_min→_s", "7320×_s");
  assert_parsed_expression_simplify_to("2×_kg×_m^2×_s^(-2)→_J", "2×_J");
  assert_parsed_expression_simplify_to("300000×_m^3→_km^(2.3+0.7)",
                                       "3ᴇ-4×_km^3");
  assert_parsed_expression_simplify_to("4×_min→_s^3/_s^2", "240×_s");
  assert_parsed_expression_simplify_to("4×_N×3_N×2_N→_N^3", "24×_N^3");
  assert_parsed_expression_simplify_to("-5_cm→_m", "-0.05×_m");
  assert_parsed_expression_simplify_to("-5_cm→_m", "-0.05×_m", User, Radian,
                                       Imperial);
  assert_parsed_expression_simplify_to("10_m/_s→_km", "undef");

  assert_parsed_expression_simplify_to("π_rad→_'", "10800×_'");
  assert_parsed_expression_simplify_to("1_°+60_'+3600_\"→_°", "3×_°");
  assert_parsed_expression_simplify_to("1°+60'+3600\"→°", "3×_°");
  assert_parsed_expression_simplify_to("0_K→_°C", "-273.15×_°C");
  assert_parsed_expression_simplify_to("0_°C→_K", "273.15×_K");
  assert_parsed_expression_simplify_to("_°C→_K", "274.15×_K");
  assert_parsed_expression_simplify_to("0_K→_°F", "-459.67×_°F");
  assert_parsed_expression_simplify_to("0_°F→_K", "255.37222222222×_K");
  assert_parsed_expression_simplify_to("_°F→_K", "255.92777777778×_K");

  assert_parsed_expression_simplify_to("0_°→_rad", "0×_rad");
  assert_parsed_expression_simplify_to("180_°→_rad", "π×_rad");

  assert_parsed_expression_simplify_to("_hplanck→_eV×_s",
                                       "4.1356676969239ᴇ-15×_eV×_s");
  PoincareTest::SymbolStore symbolStore;
  store("2_kg→a", symbolStore);
  assert_parsed_expression_simplify_to("a→g", "2000×_g", symbolStore);
}

QUIZ_CASE(poincare_simplification_complex_format) {
  // Real
  assert_parsed_expression_simplify_to("i", "nonreal", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("√(-1)", "nonreal", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("√(-1)×√(-1)", "nonreal", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("ln(-2)", "nonreal", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("(-8)^(2/3)", "4", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("(-8)^(2/5)", "2×root(2,5)", User,
                                       Radian, MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("(-8)^(1/5)", "-root(8,5)", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("(-8)^(1/4)", "nonreal", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("(-8)^(1/3)", "-2", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("[[1,2+√(-1)]]", "[[1,nonreal]]", User,
                                       Radian, MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("atan(2)", "arctan(2)", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("atan(-2)", "-arctan(2)", User, Radian,
                                       MetricUnitFormat, Real);

  {
    // User defined variable
    assert_parsed_expression_simplify_to("a", "a", User, Radian,
                                         MetricUnitFormat, Real);
    // a = 2+i
    PoincareTest::SymbolStore symbolStore;
    store("2+i→a", symbolStore);
    assert_parsed_expression_simplify_to("a", "nonreal", symbolStore, User,
                                         Radian, MetricUnitFormat, Real);
    symbolStore.reset();
    // User defined function
    // f : x → x+1
    store("x+1+i→f(x)", symbolStore);
    assert_parsed_expression_simplify_to("f(3)", "nonreal", symbolStore, User,
                                         Radian, MetricUnitFormat, Real);
  }

  // Cartesian
  assert_parsed_expression_simplify_to("-2.3ᴇ3", "-2300", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("3", "3", User, Radian, MetricUnitFormat,
                                       Cartesian);
  assert_parsed_expression_simplify_to("∞", "∞", User, Radian, MetricUnitFormat,
                                       Cartesian);
  assert_parsed_expression_simplify_to("1+2+i", "3+i", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("-(5+2×i)", "-5-2×i", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("(5+2×i)", "5+2×i", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("i+i", "2×i", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("-2+2×i", "-2+2×i", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("(3+i)-(2+4×i)", "1-3×i", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("(2+3×i)×(4-2×i)", "14+8×i", User,
                                       Radian, MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("(3+i)/2", "3/2+1/2×i", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("(3+i)/(2+i)", "7/5-1/5×i", User, Radian,
                                       MetricUnitFormat, Cartesian);
  // The simplification of (3+i)^(2+i) in a Cartesian complex form generates to
  // many nodes
  // assert_parsed_expression_simplify_to("(3+i)^(2+i)",
  // "10×cos((-4×atan(3)+ln(2)+ln(5)+2×π)/2)×e^((2×atan(3)-π)/2)+10×sin((-4×atan(3)+ln(2)+ln(5)+2×π)/2)×e^((2×atan(3)-π)/2)i",
  // User, Radian, MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("(3+i)^(2+i)", "(i+3)^(i+2)", User,
                                       Radian, MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("√(1+6i)",
                                       "√(2+2×√(37))/2+√(-2+2×√(37))/2×i", User,
                                       Radian, MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("(1+i)^2", "2×i", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("2×i", "2×i", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("i!", "i!", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("3!", "6", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("x!", "x!", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("e", "e", User, Radian, MetricUnitFormat,
                                       Cartesian);
  assert_parsed_expression_simplify_to("π", "π", User, Radian, MetricUnitFormat,
                                       Cartesian);
  assert_parsed_expression_simplify_to("i", "i", User, Radian, MetricUnitFormat,
                                       Cartesian);

  assert_parsed_expression_simplify_to("arctan(2)", "arctan(2)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("arctan(-2)", "-arctan(2)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("abs(-3)", "3", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("abs(-3+i)", "√(10)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("arctan(2)", "arctan(2)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("arctan(2+i)", "arctan(2+i)", User,
                                       Radian, MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("binomial(10, 4)", "210", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("ceil(-1.3)", "-1", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("arg(-2)", "π", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("conj(-2)", "-2", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("conj(-2+2×i+i)", "-2-3×i", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("cos(12)", "cos(12)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to(
      "cos(12+i)", "cos(12)×cosh(1)-sin(12)×sinh(1)×i", User, Radian,
      MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("diff(3×x, x, 3)", "3", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("quo(34,x)", "quo(34,x)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("rem(5,3)", "2", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("floor(x)", "floor(x)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("frac(x)", "frac(x)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("gcd(x,y)", "gcd(x,y)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("gcd(x,gcd(y,z))", "gcd(x,y,z)", User,
                                       Radian, MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("gcd(3, 1, 2, x, x^2)", "gcd(1,x^2,x)",
                                       User, Radian, MetricUnitFormat,
                                       Cartesian);
  assert_parsed_expression_simplify_to("im(1+i)", "1", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("int(x^2, x, 1, 2)", "int(x^2,x,1,2)",
                                       User, Radian, MetricUnitFormat,
                                       Cartesian);
  assert_parsed_expression_simplify_to("lcm(x,y)", "lcm(x,y)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("lcm(x,lcm(y,z))", "lcm(x,y,z)", User,
                                       Radian, MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("lcm(3, 1, 2, x, x^2)", "lcm(6,x^2,x)",
                                       User, Radian, MetricUnitFormat,
                                       Cartesian);
  // TODO: dim is not simplified yet
  // assert_parsed_expression_simplify_to("dim(x)", "dim(x)", User, Radian,
  // MetricUnitFormat, Cartesian);

  assert_parsed_expression_simplify_to("root(2,i)", "cos(ln(2))-sin(ln(2))×i",
                                       User, Radian, MetricUnitFormat,
                                       Cartesian);
  assert_parsed_expression_simplify_to(
      "root(2,i+1)", "√(2)×cos((90×ln(2))/π)-√(2)×sin((90×ln(2))/π)×i", User,
      Degree, MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to(
      "root(2,i+1)", "√(2)×cos(ln(2)/2)-√(2)×sin(ln(2)/2)×i", User, Radian,
      MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("permute(10, 4)", "5040", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("random()", "random()", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("re(x)", "x", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("round(x,y)", "round(x,y)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("sign(x)", "sign(x)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("sin(23)", "sin(23)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to(
      "sin(23+i)", "sin(23)×cosh(1)+cos(23)×sinh(1)×i", User, Radian,
      MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("√(1-i)",
                                       "√(2+2×√(2))/2-√(-2+2×√(2))/2×i", User,
                                       Radian, MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("tan(23)", "tan(23)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("tan(23+i)", "tan(23+i)", User, Radian,
                                       MetricUnitFormat, Cartesian);
  assert_parsed_expression_simplify_to("[[1,√(-1)]]", "[[1,i]]", User, Radian,
                                       MetricUnitFormat, Cartesian);

  // User defined variable
  assert_parsed_expression_simplify_to("a", "a", User, Radian, MetricUnitFormat,
                                       Cartesian);
  {
    // a = 2+i
    PoincareTest::SymbolStore symbolStore;
    store("2+i→a", symbolStore);
    assert_parsed_expression_simplify_to("a", "2+i", symbolStore, User, Radian,
                                         MetricUnitFormat, Cartesian);
    symbolStore.reset();
    // User defined function
    // f : x → x+1
    store("x+1+i→f(x)", symbolStore);
    assert_parsed_expression_simplify_to("f(3)", "4+i", symbolStore, User,
                                         Radian, MetricUnitFormat, Cartesian);
  }
  // Polar
  assert_parsed_expression_simplify_to("-2.3ᴇ3", "2300×e^(π×i)", User, Radian,
                                       MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("3", "3", User, Radian, MetricUnitFormat,
                                       Polar);
  assert_parsed_expression_simplify_to("∞", "∞", User, Radian, MetricUnitFormat,
                                       Polar);
  assert_parsed_expression_simplify_to("1+2+i",
                                       "√(10)×e^((-2×arctan(3)+π)/2×i)", User,
                                       Radian, MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("1+2+i",
                                       "√(10)×e^((-π×arctan(3)+90×π)/180×i)",
                                       User, Degree, MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("-(5+2×i)",
                                       "√(29)×e^((-2×arctan(5/2)-π)/2×i)", User,
                                       Radian, MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("(5+2×i)",
                                       "√(29)×e^((-2×arctan(5/2)+π)/2×i)", User,
                                       Radian, MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("i+i", "2×e^(π/2×i)", User, Radian,
                                       MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("i+i", "2×e^(π/2×i)", User, Radian,
                                       MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("-2+2×i", "2×√(2)×e^((3×π)/4×i)", User,
                                       Radian, MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("(3+i)-(2+4×i)",
                                       "√(10)×e^((2×arctan(1/3)-π)/2×i)", User,
                                       Radian, MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("(2+3×i)×(4-2×i)",
                                       "2×√(65)×e^((-2×arctan(7/4)+π)/2×i)",
                                       User, Radian, MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("(3+i)/2", "√(10)/2×e^(arctan(1/3)×i)",
                                       User, Radian, MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("(3+i)/(2+i)",
                                       "√(2)×e^((2×arctan(7)-π)/2×i)", User,
                                       Radian, MetricUnitFormat, Polar);
  // TODO: simplify arctan(tan(x)) = x±k×pi?
  // assert_parsed_expression_simplify_to("(3+i)^(2+i)",
  // "10e^((2×arctan(3)-π)/2)×e^(((-4×arctan(3)+ln(2)+ln(5)+2π)/2)i)",
  //  User, Radian, MetricUnitFormat, Polar);
  // The simplification of (3+i)^(2+i) in a Polar complex form generates to many
  // nodes
  // assert_parsed_expression_simplify_to("(3+i)^(2+i)",
  // "10e^((2×arctan(3)-π)/2)×e^((arctan(tan((-4×arctan(3)+ln(2)+ln(5)+2×π)/2))+π)i)",
  //  User, Radian, MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("(3+i)^(2+i)", "(i+3)^(i+2)", User,
                                       Radian, MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("(1+i)^2", "2×e^(π/2×i)", User, Radian,
                                       MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("2×i", "2×e^(π/2×i)", User, Radian,
                                       MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("3!", "6", User, Radian,
                                       MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("x!", "x!", User, Radian,
                                       MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("e", "e", User, Radian, MetricUnitFormat,
                                       Polar);
  assert_parsed_expression_simplify_to("π", "π", User, Radian, MetricUnitFormat,
                                       Polar);
  assert_parsed_expression_simplify_to("i", "e^(π/2×i)", User, Radian,
                                       MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("abs(-3)", "3", User, Radian,
                                       MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("abs(-3+i)", "√(10)", User, Radian,
                                       MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("conj(2×e^(i×π/2))", "2×e^(-π/2×i)",
                                       User, Radian, MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("-2×e^(i×π/2)", "2×e^(-π/2×i)", User,
                                       Radian, MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("[[1,√(-1)]]", "[[1,e^(π/2×i)]]", User,
                                       Radian, MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("arctan(2)", "arctan(2)", User, Radian,
                                       MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("arctan(-2)", "arctan(2)×e^(π×i)", User,
                                       Radian, MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("cos(42π)", "-cos(42×π)×e^\x12π×i\x13",
                                       User, Degree, MetricUnitFormat, Polar);

  // User defined variable
  assert_parsed_expression_simplify_to("a", "a", User, Radian, MetricUnitFormat,
                                       Polar);
  {
    PoincareTest::SymbolStore symbolStore;
    // a = 2 + i
    store("2+i→a", symbolStore);
    assert_parsed_expression_simplify_to("a", "√(5)×e^((-2×arctan(2)+π)/2×i)",
                                         symbolStore, User, Radian,
                                         MetricUnitFormat, Polar);
    symbolStore.reset();
    // User defined function
    // f: x → x+1

    store("x+1+i→f(x)", symbolStore);
    assert_parsed_expression_simplify_to("f(3)", "√(17)×e^(arctan(1/4)×i)",
                                         symbolStore, User, Radian,
                                         MetricUnitFormat, Polar);
    symbolStore.reset();
  }
}

QUIZ_CASE(poincare_simplification_reduction_target) {
  // Replace sin/cos-->tan for ReductionTarget = User
  assert_parsed_expression_simplify_to(
      "sin(x)/(cos(x)×cos(x))", "sin(x)/cos(x)^2", SystemForAnalysis, Radian,
      MetricUnitFormat, Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to(
      "sin(x)/(cos(x)×cos(x))", "sin(x)/cos(x)^2", SystemForApproximation,
      Radian, MetricUnitFormat, Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("sin(x)/(cos(x)×cos(x))",
                                       "tan(x)/cos(x)", User);

  // Factorize on the same denominator only for ReductionTarget = User
  assert_parsed_expression_simplify_to("1/π+1/x", "1/π+1/x", SystemForAnalysis,
                                       Radian, MetricUnitFormat, Cartesian,
                                       ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to(
      "1/π+1/x", "1/π+1/x", SystemForApproximation, Radian, MetricUnitFormat,
      Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("1/π+1/x", "(x+π)/(π×x)", User);

  // Display in the form a+ib only for ReductionTarget = User
  assert_parsed_expression_simplify_to("1/(1+i)", "1/(1+i)", SystemForAnalysis,
                                       Radian, MetricUnitFormat, Cartesian,
                                       ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to(
      "1/(1+i)", "1/2-1/2×i", SystemForApproximation, Radian, MetricUnitFormat,
      Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("1/(1+i)", "1/2-1/2×i", User);
  assert_parsed_expression_simplify_to(
      "√(i×(i+2))", "√(-1+2×i)", SystemForApproximation, Radian,
      MetricUnitFormat, Cartesian, ReplaceDefinedSymbols, false);

  assert_parsed_expression_simplify_to(
      "sign(abs(x))", "sign(abs(x))", SystemForApproximation, Radian,
      MetricUnitFormat, Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("sign(abs(x))", "sign(abs(x))", User);

  // Reduction of abs(x) depends on the target and the complex format
  assert_parsed_expression_simplify_to(
      "x/abs(x)", "dep(1/sign(x),{1/x})", SystemForAnalysis, Radian,
      MetricUnitFormat, Real, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("x/abs(x)", "x/abs(x)", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to(
      "x/abs(x)", "x/abs(x)", SystemForAnalysis, Radian, MetricUnitFormat,
      Cartesian, ReplaceDefinedSymbols, false);

  // Expand multinome when ReductionTarget is not SystemForApproximation as it
  // increases precision loss
  assert_parsed_expression_simplify_to(
      "(2+x)^2", "(x+2)^2", SystemForApproximation, Radian, MetricUnitFormat,
      Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to(
      "(2+x)^2", "x^2+4×x+4", SystemForAnalysis, Radian, MetricUnitFormat,
      Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("(2+x)^2", "x^2+4×x+4", User);
}

QUIZ_CASE(poincare_simplification_user_function) {
  PoincareTest::SymbolStore symbolStore;
  // User defined function
  // f: x → x*3
  store("x*3→f(x)", symbolStore);
  assert_parsed_expression_simplify_to("f(1+1)", "6", symbolStore, User, Radian,
                                       MetricUnitFormat, Polar);
  assert_parsed_expression_simplify_to("f({2,3})", "{6,9}", symbolStore, User,
                                       Radian, MetricUnitFormat, Polar);
  // f: x → 3
  store("3→f(x)", symbolStore);
  assert_parsed_expression_simplify_to("f(1/0)", "undef", symbolStore, User,
                                       Radian, MetricUnitFormat, Polar);
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
      "abs(√(300000.0003^23))", "9.702740901018ᴇ62", degreeCartesianCtx, 13);
}

QUIZ_CASE(poincare_hyperbolic_trigonometry) {
  // Exact values
  assert_parsed_expression_simplify_to("sinh(0)", "0");
  assert_parsed_expression_simplify_to("cosh(0)", "1");
  assert_parsed_expression_simplify_to("tanh(0)", "0");
  assert_parsed_expression_simplify_to("arsinh(0)", "0");
  assert_parsed_expression_simplify_to("arcosh(1)", "0");
  assert_parsed_expression_simplify_to("artanh(0)", "0");

  // arcosh(cosh)
  assert_parsed_expression_simplify_to("arcosh(cosh(3))", "3");
  assert_parsed_expression_simplify_to("arcosh(cosh(0.5))", "1/2");
  assert_parsed_expression_simplify_to("arcosh(cosh(-3))", "3");
  assert_parsed_expression_simplify_to("arcosh(cosh(3))", "3", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("arcosh(cosh(0.5))", "1/2", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("arcosh(cosh(-3))", "3", User, Radian,
                                       MetricUnitFormat, Real);

  // cosh(acosh)
  assert_parsed_expression_simplify_to("cosh(arcosh(3))", "3");
  assert_parsed_expression_simplify_to("cosh(arcosh(0.5))", "1/2");
  assert_parsed_expression_simplify_to("cosh(arcosh(-3))", "-3");
  assert_parsed_expression_simplify_to("cosh(arcosh(3))", "3", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("cosh(arcosh(0.5))", "cosh(arcosh(1/2))",
                                       User, Radian, MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("cosh(arcosh(-3))", "cosh(arcosh(-3))",
                                       User, Radian, MetricUnitFormat, Real);

  // sinh(asinh)
  assert_parsed_expression_simplify_to("sinh(arsinh(3))", "3");
  assert_parsed_expression_simplify_to("sinh(arsinh(0.5))", "1/2");
  assert_parsed_expression_simplify_to("sinh(arsinh(-3))", "-3");
  assert_parsed_expression_simplify_to("sinh(arsinh(3))", "3", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("sinh(arsinh(0.5))", "1/2", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("sinh(arsinh(-3))", "-3", User, Radian,
                                       MetricUnitFormat, Real);

  // arsinh(sinh)
  assert_parsed_expression_simplify_to("arsinh(sinh(3))", "3");
  assert_parsed_expression_simplify_to("arsinh(sinh(0.5))", "1/2");
  assert_parsed_expression_simplify_to("arsinh(sinh(-3))", "-3");
  assert_parsed_expression_simplify_to("arsinh(sinh(3))", "3", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("arsinh(sinh(0.5))", "1/2", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("arsinh(sinh(-3))", "-3", User, Radian,
                                       MetricUnitFormat, Real);

  // tanh(atanh)
  assert_parsed_expression_simplify_to("tanh(artanh(3))", "3");
  assert_parsed_expression_simplify_to("tanh(artanh(0.5))", "1/2");
  assert_parsed_expression_simplify_to("tanh(artanh(-3))", "-3");
  assert_parsed_expression_simplify_to("tanh(artanh(3))", "3", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("tanh(artanh(0.5))", "1/2", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("tanh(artanh(-3))", "-3", User, Radian,
                                       MetricUnitFormat, Real);

  // artanh(tanh)
  assert_parsed_expression_simplify_to("artanh(tanh(3))", "3");
  assert_parsed_expression_simplify_to("artanh(tanh(0.5))", "1/2");
  assert_parsed_expression_simplify_to("artanh(tanh(-3))", "-3");
  assert_parsed_expression_simplify_to("artanh(tanh(3))", "3", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("artanh(tanh(0.5))", "1/2", User, Radian,
                                       MetricUnitFormat, Real);
  assert_parsed_expression_simplify_to("artanh(tanh(-3))", "-3", User, Radian,
                                       MetricUnitFormat, Real);
}

QUIZ_CASE(poincare_advanced_trigonometry) {
  // Exact values
  assert_parsed_expression_simplify_to("csc(0)", "undef");
  assert_parsed_expression_simplify_to("sec(0)", "1");
  assert_parsed_expression_simplify_to("cot(0)", "undef");
  assert_parsed_expression_simplify_to("arccsc(2/√(3))", "π/3");
  assert_parsed_expression_simplify_to("arcsec(2/√(3))", "π/6");
  assert_parsed_expression_simplify_to("arccot(0)", "π/2");
  assert_parsed_expression_simplify_to("csc(π/2)", "1");
  assert_parsed_expression_simplify_to("sec(π/2)", "undef");
  assert_parsed_expression_simplify_to("cot(π/2)", "0");
  assert_parsed_expression_simplify_to("arccsc(1)", "π/2");
  assert_parsed_expression_simplify_to("arcsec(1)", "0");
  assert_parsed_expression_simplify_to("arccot(1)", "π/4");

  // arcsec(sec)
  assert_parsed_expression_simplify_to("arcsec(sec(3))", "3");
  assert_parsed_expression_simplify_to("arcsec(sec(0.5))", "1/2");
  assert_parsed_expression_simplify_to("arcsec(sec(-3))", "3");

  // sec(asec)
  assert_parsed_expression_simplify_to("sec(arcsec(3))", "3");
  assert_parsed_expression_simplify_to("sec(arcsec(0.5))", "1/2");
  assert_parsed_expression_simplify_to("sec(arcsec(-3))",
                                       "sec(-arccos(1/3)+π)");

  // arccsc(csc)
  assert_parsed_expression_simplify_to("arccsc(csc(3))", "-3+π");
  assert_parsed_expression_simplify_to("arccsc(csc(0.5))", "1/2");
  assert_parsed_expression_simplify_to("arccsc(csc(-3))", "3-π");

  // csc(acsc)
  assert_parsed_expression_simplify_to("csc(arccsc(3))", "3");
  assert_parsed_expression_simplify_to("csc(arccsc(0.5))", "1/2");
  assert_parsed_expression_simplify_to("csc(arccsc(-3))", "-3");

  // arccot(cot)
  assert_parsed_expression_simplify_to("arccot(cot(3))", "-π+3");
  assert_parsed_expression_simplify_to("arccot(cot(0.5))", "1/2");
  assert_parsed_expression_simplify_to("arccot(cot(-3))", "π-3");
  assert_parsed_expression_simplify_to("arccot(3)", "arctan(1/3)", User);
  assert_parsed_expression_simplify_to(
      "arccot(3)", "arctan(1/3)", SystemForAnalysis, Radian, MetricUnitFormat,
      Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to(
      "arccot(3)", "arctan(1/3)", SystemForApproximation, Radian,
      MetricUnitFormat, Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to("arccot(x)", "arccot(x)", User);
  assert_parsed_expression_simplify_to(
      "arccot(x)", "arccot(x)", SystemForAnalysis, Radian, MetricUnitFormat,
      Cartesian, ReplaceDefinedSymbols, false);
  assert_parsed_expression_simplify_to(
      "arccot(x)", "arccot(x)", SystemForApproximation, Radian,
      MetricUnitFormat, Cartesian, ReplaceDefinedSymbols, false);

  // cot(acot)
  assert_parsed_expression_simplify_to("cot(arccot(3))", "3");
  assert_parsed_expression_simplify_to("cot(arccot(0.5))", "1/2");
  assert_parsed_expression_simplify_to("cot(arccot(-3))", "-3");
}

QUIZ_CASE(poincare_probability) {
  assert_parsed_expression_simplify_to("binompdf(2,4,-1)", "undef");
  assert_parsed_expression_simplify_to("binompdf(2,4,1.2)", "undef");
  assert_parsed_expression_simplify_to("binompdf(-inf,4,0.4)", "0");
  assert_parsed_expression_simplify_to("binompdf(inf,4,0.4)", "0");
  assert_parsed_expression_simplify_to("binomcdf(2,4,-1)", "undef");
  assert_parsed_expression_simplify_to("binomcdf(2,4,1.2)", "undef");
  assert_parsed_expression_simplify_to("binomcdf(-inf,4,0.4)", "0");
  assert_parsed_expression_simplify_to("binomcdf(inf,4,0.4)", "1");
  assert_parsed_expression_simplify_to("invbinom(1,4,-1)", "undef");
  assert_parsed_expression_simplify_to("invbinom(0,4,1.2)", "undef");
  assert_parsed_expression_simplify_to("invbinom(1,4,0.4)", "4");
  assert_parsed_expression_simplify_to("invbinom(0,4,0.4)", "undef");
  assert_parsed_expression_simplify_to("invbinom(1,4,1)", "4");
  assert_parsed_expression_simplify_to("invbinom(0,4,1)", "0");

  assert_parsed_expression_simplify_to("geompdf(1,0)", "undef");
  assert_parsed_expression_simplify_to("geomcdf(inf,0.5)", "1");
  assert_parsed_expression_simplify_to("invgeom(1,1)", "1");

  assert_parsed_expression_simplify_to("invnorm(-1.3,2,3)", "undef");
  assert_parsed_expression_simplify_to("invnorm(-1.3,2,3)", "undef");
  assert_parsed_expression_simplify_to("invnorm(-1.3,2,3)", "undef");
  assert_parsed_expression_simplify_to("invnorm(0,2,3)", "-∞");
  assert_parsed_expression_simplify_to("invnorm(0.5,2,3)", "2");
  assert_parsed_expression_simplify_to("invnorm(1,2,3)", "∞");
  assert_parsed_expression_simplify_to("invnorm(1.3,2,3)", "undef");
  // random can be 0
  assert_parsed_expression_simplify_to("invnorm(3/4,2,random())",
                                       "invnorm(3/4,2,random())");
  assert_parsed_expression_simplify_to("invnorm(0.5,2,0)", "undef");
  assert_parsed_expression_simplify_to("invnorm(0.5,2,-1)", "undef");
  assert_parsed_expression_simplify_to("normcdf(2,0,1)", "normcdf(2,0,1)");
  assert_parsed_expression_simplify_to("normcdfrange(-inf,inf,20,4)", "1");
  assert_parsed_expression_simplify_to("normcdfrange(1,2,0,1)",
                                       "normcdfrange(1,2,0,1)");
  assert_parsed_expression_simplify_to("normpdf(2,0,1)", "normpdf(2,0,1)");

  assert_parsed_expression_simplify_to("poissonpdf(1,0)", "undef");
  assert_parsed_expression_simplify_to("poissoncdf(inf,0.5)", "1");

  assert_parsed_expression_simplify_to("tpdf(-inf,0)", "undef");
  assert_parsed_expression_simplify_to("tpdf(-inf,2)", "0");
  assert_parsed_expression_simplify_to("tpdf(inf,2)", "0");
  assert_parsed_expression_simplify_to("tcdf(-inf,2)", "0");
  assert_parsed_expression_simplify_to("tcdf(inf,2)", "1");
  assert_parsed_expression_simplify_to("invt(0.5,0)", "undef");
  assert_parsed_expression_simplify_to("invt(0.5,12)", "0");
  assert_parsed_expression_simplify_to("invt(0.5,0)", "undef");

  assert_parsed_expression_simplify_to("invt(0.3,4)+normcdf(2,0,1)+invt(0.3,4)",
                                       "2×invt(3/10,4)+normcdf(2,0,1)");
}

QUIZ_CASE(poincare_simplification_system_circuit_breaker_handled) {
  // Try with ReductionTarget = User --> fails --> retry with ReductionTarget =
  // SystemForApproximation
  assert_parsed_expression_simplify_to("(π+i)^√(i×(i+2))", "(π+i)^√(-1+2×i)");
  // Multiplication overflows --> don't reduce
  assert_parsed_expression_simplify_to(
      "20^23×20^23×20^23×20^23×20^23×20^23×25^23×20^23×20^23×20^23×20^23×20^"
      "23×20^23×25^23",
      "20^23×20^23×20^23×20^23×20^23×20^23×25^23×20^23×20^23×20^23×20^23×20^"
      "23×20^23×25^23");
}

QUIZ_CASE(poincare_simplification_list) {
  assert_parsed_expression_simplify_to("{}", "{}");
  // Lists can't contain matrix or lists
  assert_parsed_expression_simplify_to("{{1,2},3}", "undef");
  assert_parsed_expression_simplify_to("{[[1,2][3,4]],2,3}", "undef");
  // Inner simplifications
  assert_parsed_expression_simplify_to("{1,2,3}", "{1,2,3}");
  assert_parsed_expression_simplify_to("{1,8/4,27/45}", "{1,2,3/5}");
  assert_parsed_expression_simplify_to("{1,2+2,3^2}", "{1,4,9}");
  assert_parsed_expression_simplify_to("{-1,1/0}", "{-1,undef}");

  // Operations on lists of different lengths
  assert_parsed_expression_simplify_to("{}+{1}+{2,3}", "undef");
  assert_parsed_expression_simplify_to("{1,2,3,4}×{-1,-2}", "undef");
  assert_parsed_expression_simplify_to("{1,4,9}^{1,1/2}", "undef");
  // Operations on lists of the same length
  assert_parsed_expression_simplify_to("{}×{}", "{}");
  assert_parsed_expression_simplify_to("{1,2,3}-{3,2,1}", "{-2,0,2}");
  assert_parsed_expression_simplify_to("{1,2,3,4}×{2,-2,2,-2}", "{2,-4,6,-8}");
  assert_parsed_expression_simplify_to("{4,8,16}^{1/2,1/3,1/4}", "{2,2,2}");
  // Operations on lists and scalars
  assert_parsed_expression_simplify_to("{}^(-1)", "{}");
  assert_parsed_expression_simplify_to("{1,2,3}+1", "{2,3,4}");
  assert_parsed_expression_simplify_to("11/{11,33,55,77}", "{1,1/3,1/5,1/7}");
  assert_parsed_expression_simplify_to("{1,4,9,16,25}^(1/2)", "{1,2,3,4,5}");

  {
    // Access to an element
    PoincareTest::SymbolStore symbolStore;
    store("{1,4,9}→l1", symbolStore);
    store("{}→l2", symbolStore);
    assert_parsed_expression_simplify_to("l1(1)", "1", symbolStore);
    assert_parsed_expression_simplify_to("l1(2)", "4", symbolStore);
    assert_parsed_expression_simplify_to("l1(3)", "9", symbolStore);
    assert_parsed_expression_simplify_to("l1(0)", "undef", symbolStore);
    assert_parsed_expression_simplify_to("l1(5)", "undef", symbolStore);
    assert_parsed_expression_simplify_to("l1(-2)", "undef", symbolStore);
    assert_parsed_expression_simplify_to("l1(1.23)", "undef", symbolStore);
    assert_parsed_expression_simplify_to("l2(1)", "undef", symbolStore);
    // Slice of a list
    assert_parsed_expression_simplify_to("l1(1,2)", "{1,4}", symbolStore);
    assert_parsed_expression_simplify_to("l1(2,3)", "{4,9}", symbolStore);
    assert_parsed_expression_simplify_to("l1(1,3)", "{1,4,9}", symbolStore);
    assert_parsed_expression_simplify_to("l1(2,2)", "{4}", symbolStore);
    assert_parsed_expression_simplify_to("l1(0,2)", "{1,4}", symbolStore);
    assert_parsed_expression_simplify_to("l1(1,5)", "{1,4,9}", symbolStore);
    assert_parsed_expression_simplify_to("l1(3,2)", "{}", symbolStore);
    assert_parsed_expression_simplify_to("l2(1,2)", "{}", symbolStore);
  }
  // Functions on lists
  // OList length
  assert_parsed_expression_simplify_to("dim({})", "0");
  assert_parsed_expression_simplify_to("dim({1,2,3})", "3");
  assert_parsed_expression_simplify_to("dim({{1,2,3,4,5}})", "undef");
  // Sum of elements
  assert_parsed_expression_simplify_to("sum({})", "0");
  assert_parsed_expression_simplify_to("sum({1,2,3})", "6");
  // Product of elements
  assert_parsed_expression_simplify_to("prod({})", "1");
  assert_parsed_expression_simplify_to("prod({1,4,9})", "36");
  // Sort a list of complexes
  assert_parsed_expression_simplify_to("sort({})", "{}");
  assert_parsed_expression_simplify_to("sort({4})", "{4}");
  assert_parsed_expression_simplify_to("sort({undef})", "{undef}");
  assert_parsed_expression_simplify_to("sort({i})", "sort({i})");
  assert_parsed_expression_simplify_to("sort({-1,5,2+6,-0})", "{-1,0,5,8}");
#if TODO_PCJ
  assert_parsed_expression_simplify_to("sort({-1,-2,-inf,inf})",
                                       "{-∞,-2,-1,∞}");
#endif
  assert_parsed_expression_simplify_to("sort({-1,undef,-2,-inf,inf})",
                                       "{-1,undef,-2,-∞,∞}");
  assert_parsed_expression_simplify_to("sort({-1,i,8,-0})", "sort({-1,i,8,0})");
  assert_parsed_expression_simplify_to("sort({-1,undef,1})", "{-1,undef,1}");
#if TOOD_PCJ
  assert_parsed_expression_simplify_to("sort(ln({7-π7,7}))",
                                       "sort({ln(-7×π+7),ln(7)})", User, Radian,
                                       MetricUnitFormat, Real);
#endif
  // Sort list of points
  assert_parsed_expression_simplify_to("sort({(8,1),(5,0),(5,-3),(1,0),(5,9)})",
                                       "{(1,0),(5,-3),(5,0),(5,9),(8,1)}");
  assert_parsed_expression_simplify_to("sort({(8,1),(5,i),(5,-3)})",
                                       "sort({(8,1),(5,i),(5,-3)})");
  assert_parsed_expression_simplify_to("sort({(undef,1),(6,1),(5,-3)})",
                                       "{(undef,1),(6,1),(5,-3)}");
  assert_parsed_expression_simplify_to(
      "sort({(inf,1),(6,1),(5,-3),(-inf,9),(-inf,1)})",
      "{(-∞,1),(-∞,9),(5,-3),(6,1),(∞,1)}");
  // Mean of a list
  assert_parsed_expression_simplify_to("mean({})", "undef");
  assert_parsed_expression_simplify_to("mean({1,2,3})", "2");
  assert_parsed_expression_simplify_to("mean({5,8,7,4,12})", "36/5");
  assert_parsed_expression_simplify_to("mean({1,6,3,4,5,2},{2,3,1,2,3,1})",
                                       "4");
  assert_parsed_expression_simplify_to("mean({1,6,3,undef,5,2},{2,3,1,2,3,1})",
                                       "undef");
  assert_parsed_expression_simplify_to("mean({5,8,7,4,12},{2})", "undef");
  assert_parsed_expression_simplify_to("mean({5,8,7,4,12},{0,0,0,0,0})",
                                       "undef");
  assert_parsed_expression_simplify_to("mean({5,8,7,4,12},{-2,4,4,4,4})",
                                       "undef");
  // Minimum of a list
  assert_parsed_expression_simplify_to("min({})", "undef");
  assert_parsed_expression_simplify_to("min({1,2,3})", "1");
  assert_parsed_expression_simplify_to("min({3,undef,-2})", "undef");
  // Do not simplify when a value can't be approximated
  assert_parsed_expression_simplify_to("min({3,x,-2})", "min({3,x,-2})");
  assert_parsed_expression_simplify_to("min({3,-inf,-2})", "-∞");
  assert_parsed_expression_simplify_to("min({-7,0,i})", "min({-7,0,i})");
  assert_parsed_expression_simplify_to("min({-7,undef,i})", "undef");

  // Maximum of a list
  assert_parsed_expression_simplify_to("max({})", "undef");
  assert_parsed_expression_simplify_to("max({1,2,3})", "3");
  assert_parsed_expression_simplify_to("max({3,undef,-2})", "undef");
  // Do not simplify when a value can't be approximated
  assert_parsed_expression_simplify_to("max({3,x,-2})", "max({3,x,-2})");
  assert_parsed_expression_simplify_to("max({3,inf,-2})", "∞");
  assert_parsed_expression_simplify_to("max({-7,0,i})", "max({-7,0,i})");
  assert_parsed_expression_simplify_to("max({-7,undef,i})", "undef");

  // Variance of a list
  assert_parsed_expression_simplify_to("var({})", "undef");
  assert_parsed_expression_simplify_to("var({1})", "0");
  assert_parsed_expression_simplify_to("var({1,2,3,4,5,6})", "35/12");
  assert_parsed_expression_simplify_to("var({1,2,3,4,5,6},{2,3,2,1,3,1})",
                                       "43/16");
  // Standard deviation of a list
  assert_parsed_expression_simplify_to("stddev({})", "undef");
  assert_parsed_expression_simplify_to("stddev({1})", "0");
  assert_parsed_expression_simplify_to("stddev({1,2,3,4,5,6})", "√(105)/6");
  assert_parsed_expression_simplify_to("stddev({1,2,3,4,5,6},{2,3,1,1,2,4})",
                                       "(2×√(157))/13");
  // Sample standard deviation of a list
  assert_parsed_expression_simplify_to("samplestddev({})", "undef");
  assert_parsed_expression_simplify_to("samplestddev({1})", "undef");
  assert_parsed_expression_simplify_to("samplestddev({1,2,3,4,5,6})",
                                       "√(14)/2");
  assert_parsed_expression_simplify_to(
      "samplestddev({1,2,3,4,5,6},{2,3,1,1,2,4})", "√(6123)/39");
  // Median of a list
  assert_parsed_expression_simplify_to("med({})", "undef");
  assert_parsed_expression_simplify_to("med({1})", "1");
  assert_parsed_expression_simplify_to("med({4,2,3,1,6})", "3");
  assert_parsed_expression_simplify_to("med({1,6,3,4,5,2})", "7/2");
  assert_parsed_expression_simplify_to("med({1,undef,2,3})", "undef");
  // Do not reduce if a child can't be approximated
  assert_parsed_expression_simplify_to("med({1,x,2,3})", "med({1,x,2,3})");
#if TODO_PCJ
  assert_parsed_expression_simplify_to("med({1,6,3,4,5,2},{1,2,1,1,2,2})", "4");
  assert_parsed_expression_simplify_to("med({1,6,3},{1,1,undef})", "undef");
  // Do not reduce if a child can't be approximated
  assert_parsed_expression_simplify_to("med({1,6,3},{1,1,x})",
                                       "med({1,6,3},{1,1,x})");
#endif
  // OList sequences
  assert_parsed_expression_simplify_to("sequence(1,k,1)", "{1}");
  assert_parsed_expression_simplify_to("sequence(k,k,10)",
                                       "{1,2,3,4,5,6,7,8,9,10}");
  assert_parsed_expression_simplify_to("sequence(1/(n-3),n,5)",
                                       "{-1/2,-1,undef,1,1/2}");
  assert_parsed_expression_simplify_to("sequence(x^2,x,3)", "{1,4,9}");
  {
    // Do not confuse u{n} and L*{n}
    PoincareTest::SymbolStore symbolStore;
    store("{3}→L", symbolStore);
    assert_parsed_expression_simplify_to("L{2}", "{6}",
                                         symbolStore);  // L*{2}
  }
}

QUIZ_CASE(poincare_simplification_functions_of_lists) {
  assert_parsed_expression_simplify_to("abs({1,-1,2,-3})", "{1,1,2,3}");
  assert_parsed_expression_simplify_to("acos({1/√(2),1/2,1,-1})",
                                       "{π/4,π/3,0,π}");
  assert_parsed_expression_simplify_to("acos({1,0})", "{0,π/2}");
  assert_parsed_expression_simplify_to("asin({1/√(2),1/2,1,-1})",
                                       "{π/4,π/6,π/2,-π/2}");
  assert_parsed_expression_simplify_to("asin({1,0})", "{π/2,0}");
  assert_parsed_expression_simplify_to("atan({√(3),1,1/√(3),-1})",
                                       "{π/3,π/4,π/6,-π/4}");
  assert_parsed_expression_simplify_to("atan({1,0})", "{π/4,0}");
  assert_parsed_expression_simplify_to("binomial(3,{2,3})", "{3,1}");
  assert_parsed_expression_simplify_to("binomial({2,3},1)", "{2,3}");
  assert_parsed_expression_simplify_to("binomial({0,180},{1})", "undef");
  assert_parsed_expression_simplify_to("ceil({0.3,180})", "{1,180}");
  assert_parsed_expression_simplify_to("arg({1,1+i})", "{0,π/4}");
  assert_parsed_expression_simplify_to("conj({1,1+i})", "{1,1-i}");
  assert_parsed_expression_simplify_to("cos({π/3,0,π/7,π/2})",
                                       "{1/2,1,cos(π/7),0}");
  assert_parsed_expression_simplify_to("cos({0,π})", "{1,-1}");
  assert_parsed_expression_simplify_to("diff({0,x},x,1)", "{0,1}");
  assert_parsed_expression_simplify_to("diff(x^2,x,{0,180})", "{0,360}");
  assert_parsed_expression_simplify_to("{1,3}!", "{1,6}");
  assert_parsed_expression_simplify_to("{1,2,3,4}!", "{1,2,6,24}");
  assert_parsed_expression_simplify_to("floor({1/√(2),1/2,1,-1.3})",
                                       "{0,0,1,-2}");
  assert_parsed_expression_simplify_to("floor({0.3,180})", "{0,180}");
  assert_parsed_expression_simplify_to("frac({1/√(2),1/2,1,-1.3})",
                                       "{√(2)/2,1/2,0,7/10}");
  assert_parsed_expression_simplify_to("frac({0.3,180})", "{3/10,0}");
  assert_parsed_expression_simplify_to("gcd({25,60},15)", "{5,15}");
  assert_parsed_expression_simplify_to("arcosh({0,π})",
                                       "{π/2×i,ln(π+√(π^2-1))}");
  assert_parsed_expression_simplify_to("arsinh({0,π})", "{0,arsinh(π)}");
  assert_parsed_expression_simplify_to("artanh({0,π})", "{0,artanh(π)}");
  assert_parsed_expression_simplify_to("cosh({0,π})", "{1,cosh(π)}");
  assert_parsed_expression_simplify_to("sinh({0,π})", "{0,sinh(π)}");
  assert_parsed_expression_simplify_to("tanh({0,π})", "{0,tanh(π)}");
  assert_parsed_expression_simplify_to("im({1/√(2),1/2,1,-1})", "{0,0,0,0}");
  assert_parsed_expression_simplify_to("im({1,1+i})", "{0,1}");
  assert_parsed_expression_simplify_to("int({0,180},x,1,2)", "{0,180}");
  assert_parsed_expression_simplify_to("int({x,x^2},x,1,2)",
                                       "{int(x,x,1,2),int(x^2,x,1,2)}");
  assert_parsed_expression_simplify_to("int(1,x,{0,1},1)", "{1,0}");
  assert_parsed_expression_simplify_to("int(x,x,{0,1},1)",
                                       "{int(x,x,0,1),int(x,x,1,1)}");
  assert_parsed_expression_simplify_to("log({2,3})", "{log(2),log(3)}");
  assert_parsed_expression_simplify_to("log({2,3},5)", "{log(2,5),log(3,5)}");
  assert_parsed_expression_simplify_to("log(5,{2,3})", "{log(5,2),log(5,3)}");
  assert_parsed_expression_simplify_to("log({√(2),1/2,1,3})",
                                       "{log(2)/2,-log(2),0,log(3)}");
  assert_parsed_expression_simplify_to("log({1/√(2),1/2,1,-3})",
                                       "{-log(2)/2,-log(2),0,log(-3)}");
  assert_parsed_expression_simplify_to("log({1/√(2),1/2,1,-3},3)",
                                       "{-log(2,3)/2,-log(2,3),0,log(-3,3)}");
  assert_parsed_expression_simplify_to("ln({2,3})", "{ln(2),ln(3)}");
  assert_parsed_expression_simplify_to("ln({√(2),1/2,1,3})",
                                       "{ln(2)/2,-ln(2),0,ln(3)}");
  assert_parsed_expression_simplify_to("root({2,3},5)",
                                       "{root(2,5),root(3,5)}");
  assert_parsed_expression_simplify_to("-{1/√(2),1/2,3,2,1,-3}",
                                       "{-1/√(2),-1/2,-3,-2,-1,3}");
  assert_parsed_expression_simplify_to("re({1,i})", "{1,0}");
  assert_parsed_expression_simplify_to("round({2.12,3.42}, 1)", "{21/10,17/5}");
  assert_parsed_expression_simplify_to("round(1.23456, {2,3})",
                                       "{123/100,247/200}");
  assert_parsed_expression_simplify_to("sin({π/3,0,π/7,π/2})",
                                       "{√(3)/2,0,sin(π/7),1}");
  assert_parsed_expression_simplify_to("sin({0,π})", "{0,0}");
  assert_parsed_expression_simplify_to("{2,3.4}-{0.1,3.1}", "{19/10,3/10}");
  assert_parsed_expression_simplify_to("tan({0,π/4})", "{0,1}");
  assert_parsed_expression_simplify_to("{}%", "{}");
  assert_parsed_expression_simplify_to("abs({1}%)", "{1/100}");
  assert_parsed_expression_simplify_to("2+{5,10}%",
                                       "{2×(1+5/100),2×(1+10/100)}");
  assert_parsed_expression_simplify_to("rem(sort({i})^4,0)",
                                       "rem(sort({i})^4,0)");
}

QUIZ_CASE(poincare_simplification_mix_lists) {
  assert_parsed_expression_simplify_to("{{1,2},{3,4}}", "undef");
  assert_parsed_expression_simplify_to("{[[1,2][3,4]]}", "undef");
  assert_parsed_expression_simplify_to("[[{1,2},{3,4}][3,4]]", "undef");
  assert_parsed_expression_simplify_to("{1,2}+[[1,2][3,4]]", "undef");
  assert_parsed_expression_simplify_to("{1,2}*[[1,2][3,4]]", "undef");
  assert_parsed_expression_simplify_to("{1_kg, 2_kg}+3_kg", "{4×_kg,5×_kg}",
                                       User, Radian, MetricUnitFormat);
}

QUIZ_CASE(poincare_simplification_mixed_fraction) {
#if 0
  // TODO_PCJ: these tests fail with emscripten
  assert_parsed_expression_simplify_to("1 2/3", "5/3");
  assert_parsed_expression_simplify_to("-1 2/3", "-5/3");
#endif
}

QUIZ_CASE(poincare_simplification_booleans) {
  assert_parsed_expression_simplify_to("true", "True");
  assert_parsed_expression_simplify_to("false", "False");
  assert_parsed_expression_simplify_to("True + False", "undef");
  assert_parsed_expression_simplify_to("2True", "undef");
  assert_parsed_expression_simplify_to("False^3", "undef");
}

QUIZ_CASE(poincare_simplification_comparison_operators) {
  assert_parsed_expression_simplify_to("3 < 4", "True");
  assert_parsed_expression_simplify_to("3 < 3", "False");
  assert_parsed_expression_simplify_to("3 < 2", "False");

  assert_parsed_expression_simplify_to("3 <= 4", "True");
  assert_parsed_expression_simplify_to("3 <= 3", "True");
  assert_parsed_expression_simplify_to("3 <= 2", "False");

  assert_parsed_expression_simplify_to("3 > 4", "False");
  assert_parsed_expression_simplify_to("3 > 3", "False");
  assert_parsed_expression_simplify_to("3 > 2", "True");

  assert_parsed_expression_simplify_to("3 >= 4", "False");
  assert_parsed_expression_simplify_to("3 >= 3", "True");
  assert_parsed_expression_simplify_to("3 >= 2", "True");

  assert_parsed_expression_simplify_to("3 = 4", "False");
  assert_parsed_expression_simplify_to("3 = 3", "True");
  assert_parsed_expression_simplify_to("3 = 2", "False");

  assert_parsed_expression_simplify_to("3 != 4", "True");
  assert_parsed_expression_simplify_to("3 != 3", "False");
  assert_parsed_expression_simplify_to("3 != 2", "True");

  assert_parsed_expression_simplify_to("undef = 2", "undef");
  assert_parsed_expression_simplify_to("undef != 2", "undef");

  assert_parsed_expression_simplify_to("3 + i < 1 + 2i", "undef");
  assert_parsed_expression_simplify_to("3 + i < 1 + i", "undef");
  assert_parsed_expression_simplify_to("3 + i = 3 + i", "True");
  assert_parsed_expression_simplify_to("[[0, 0]] < [[1, 1]]", "undef");

  assert_parsed_expression_simplify_to("3 > 2 >= 1 = 4 - 3 != 6", "True");
  assert_parsed_expression_simplify_to("3 < 2 >= 1 = 4 - 3 != 6", "False");
  assert_parsed_expression_simplify_to("3 > 2 >= 1 = 4 / 0", "undef");

  assert_parsed_expression_simplify_to("3=3+3<4", "False");
  assert_parsed_expression_simplify_to("(3=3)+(3<4)", "undef");
  assert_parsed_expression_simplify_to("ln(3=5)", "undef");

  assert_parsed_expression_simplify_to("4000!4=9",
                                       "\U000000124×4000!\U00000013=9");
  assert_parsed_expression_simplify_to("4000!4!=9",
                                       "\U000000124×4000!\U00000013≠9");
}

typedef bool (*BoolCompare)(bool a, bool b);
static void testLogicalOperatorTruthTable(const char* operatorString,
                                          BoolCompare evaluationFunction) {
  constexpr const char* booleanNames[] = {"False", "True"};
  constexpr static int bufferSize = 17;  // 9 == strlen("False nand False") + 1
  char buffer[bufferSize];
  int operatorLength = strlen(operatorString);
  assert(operatorLength <= 4);
  // Test truth table
  for (int a = 0; a <= 1; a++) {
    const char* aString = booleanNames[a];
    int length = strlcpy(buffer, aString, strlen(aString) + 1);
    buffer[length] = ' ';
    length++;
    length += strlcpy(buffer + length, operatorString, operatorLength + 1);
    buffer[length] = ' ';
    length++;
    for (int b = 0; b <= 1; b++) {
      const char* bString = booleanNames[b];
      strlcpy(buffer + length, bString, strlen(bString) + 1);
      const char* truthString = booleanNames[evaluationFunction(
          static_cast<bool>(a), static_cast<bool>(b))];
      assert_parsed_expression_simplify_to(buffer, truthString);
    }
  }
  // Test undefined on numbers
  const char* numberString = "1";
  int length = strlcpy(buffer, numberString, strlen(numberString) + 1);
  buffer[length] = ' ';
  length++;
  length += strlcpy(buffer + length, operatorString, operatorLength + 1);
  buffer[length] = ' ';
  length++;
  strlcpy(buffer + length, numberString, strlen(numberString) + 1);
  assert_parsed_expression_simplify_to(buffer, "undef");
}

QUIZ_CASE(poincare_simplification_logical_operators) {
  assert_parsed_expression_simplify_to("not True", "False");
  assert_parsed_expression_simplify_to("not False", "True");
  testLogicalOperatorTruthTable("and", [](bool a, bool b) { return a && b; });
  testLogicalOperatorTruthTable("or", [](bool a, bool b) { return a || b; });
  testLogicalOperatorTruthTable("xor", [](bool a, bool b) { return a != b; });
  testLogicalOperatorTruthTable("nor",
                                [](bool a, bool b) { return !(a || b); });
  testLogicalOperatorTruthTable("nand",
                                [](bool a, bool b) { return !(a && b); });

  assert_parsed_expression_simplify_to("not False and False", "False");
  assert_parsed_expression_simplify_to("not (False and False)", "True");
  assert_parsed_expression_simplify_to("True or False xor True", "False");
  assert_parsed_expression_simplify_to("True or (False xor True)", "True");
  assert_parsed_expression_simplify_to("True xor True and False", "True");
  assert_parsed_expression_simplify_to("(True xor True) and False", "False");

  assert_parsed_expression_simplify_to("True xor {False,True,False,True}",
                                       "{True,False,True,False}");

  assert_parsed_expression_simplify_to("True and -5.2", "undef");
  assert_parsed_expression_simplify_to("True and [[-5,2]]", "undef");
  assert_parsed_expression_simplify_to("True or undef", "undef");
  assert_parsed_expression_simplify_to("not undef", "undef");
  assert_parsed_expression_simplify_to("True and 2 = 2 ", "True");
  assert_parsed_expression_simplify_to("(True and 2) = 2", "undef");

  assert_parsed_expression_simplify_to("3×not True", "undef");
}

QUIZ_CASE(poincare_simplification_piecewise_operator) {
  assert_parsed_expression_simplify_to("piecewise(3,1<0,2)", "2");
  assert_parsed_expression_simplify_to("piecewise(3,1>0,2)", "3");
  assert_parsed_expression_simplify_to("piecewise(3,0>1,4,0>2,5,0<6,2)", "5");
  assert_parsed_expression_simplify_to("piecewise(3,0<1,4,0<2,5,0<6,2)", "3");

  assert_parsed_expression_simplify_to("piecewise(3,1<0,2,3=4)", "undef");
  assert_parsed_expression_simplify_to("piecewise(3,1<0,undef)", "undef");
  assert_parsed_expression_simplify_to("piecewise(3,1>0,undef)", "3");
  assert_parsed_expression_simplify_to("piecewise(-x/x,x>0,0)",
                                       "piecewise(dep(-1,{x^0}),x>0,0)");

  assert_parsed_expression_simplify_to("piecewise(3,4>0,2,2<a)", "undef", User,
                                       Radian, MetricUnitFormat, Cartesian,
                                       ReplaceAllSymbols);
}

QUIZ_CASE(poincare_simplification_integral) {
  assert_parsed_expression_simplify_to("int(tan(x),x,0,x)",
                                       "int(tan(x),x,0,x)");
  assert_parsed_expression_simplify_to("int(arccot(x),x,0,x)",
                                       "int(arccot(x),x,0,x)");
}

QUIZ_CASE(poincare_simplification_point) {
  assert_parsed_expression_simplify_to("(1,2)", "(1,2)");
  assert_parsed_expression_simplify_to("(1/0,2)", "(undef,2)");
  assert_parsed_expression_simplify_to("(1,2)+3", "undef");
  assert_parsed_expression_simplify_to("abs((1.23,4.56))", "undef");
  assert_parsed_expression_simplify_to("{(1+2,3+4),(5+6,7+8)}",
                                       "{(3,7),(11,15)}");
  assert_parsed_expression_simplify_to("sequence((k,-k+1),k,4)",
                                       "{(1,0),(2,-1),(3,-2),(4,-3)}");
}
