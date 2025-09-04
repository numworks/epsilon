#include "helper.h"

using namespace Poincare;

QUIZ_CASE(pcj_simplification_factorial) {
  simplifies_to("0!", "1");
  simplifies_to("4!", "24");
  simplifies_to("(-4)!", "undef");
  simplifies_to("1/3!", "1/6");
  simplifies_to("5!", "120");
  simplifies_to("(1/3)!", "undef");
  simplifies_to("(3/5)!", "undef");
  simplifies_to("i!", "undef");
  simplifies_to("π!", "undef");
  simplifies_to("e!", "undef");
  simplifies_to("n!", "n!");
  // simplifies_to("(n+1)!/n!", "n+1"); TODO through parametric
}

QUIZ_CASE(pcj_simplification_arithmetic) {
  simplifies_to("factor(0)", "0");
  simplifies_to("factor(1)", "1");
  simplifies_to("factor(23)", "23");
  simplifies_to("factor(42*3)", "2×3^2×7");
  simplifies_to("factor(-12)", "-2^2×3");
  simplifies_to("factor(-4/17)", "-2^2/17");
  simplifies_to("factor(2π)", "undef");
  simplifies_to("factor(42*3)", "2×3^2×7",
                {.m_complexFormat = ComplexFormat::Polar});
  simplifies_to("105×10^14", "1.05×10^16");
  simplifies_to("factor(105×10^14)", "2^14×3×5^15×7");

  simplifies_to("quo(23,5)", "4");
  simplifies_to("rem(23,5)", "3");
  simplifies_to("gcd(14,28,21)", "7");
  simplifies_to("lcm(14,6)", "42");
  simplifies_to("gcd(6,y,2,x,4)", "gcd(2,x,y)");
  simplifies_to("sign(-2)", "-1");
  simplifies_to("sign(abs(x)+1)", "1");
  simplifies_to("ceil(8/3)", "3");
  simplifies_to("frac(8/3)", "2/3");
  simplifies_to("floor(8/3)", "2");
  simplifies_to("round(1/3,2)", "33/100");
  simplifies_to("round(3.3_m)", "undef");
  simplifies_to("ceil(x)", "ceil(x)");
  simplifies_to("ceil(-x)", "-floor(x)");
  simplifies_to("floor(x)+frac(x)", "dep(x,{floor(x)})");
  simplifies_to("permute(4,2)", "12");
  simplifies_to("binomial(4,2)", "6");
  simplifies_to("1 2/3", "5/3");
  simplifies_to("-1 2/3", "-5/3");

  simplifies_to("floor({1.3,3.9})", "{1,3}");
  // TODO: Approximation is undef
  simplifies_to("ceil(1+i)", "1+ceil(i)");
  // Reductions using approximation
  simplifies_to("floor(π)", "3");
  simplifies_to("frac(π+1)+floor(π+0.1)", "π");
  simplifies_to("log(ceil(2^15+π)-4,2)", "15");
  simplifies_to("frac(871616/2π)×2π", "871616-277442×π");
  simplifies_to("frac(2^24*π)", "-52707178+16777216×π");
  simplifies_to("frac(2^32*π)-π", "4294967295×π+ceil(-4294967296×π)");
  simplifies_to("frac(2^24+π)-π", "-3");
  simplifies_to("frac(sin(10^18))", "frac(sin(10^18))");
  simplifies_to("log(floor(2^54+π)-3, 2)", "54");
  simplifies_to("floor(random())", "floor(random())");
  simplifies_to("floor(sin(0.001))", "0");
  /* Above a certain threshold, we consider that the sine or cosine of a "big"
   * angle has too many approximation errors. This blocks the floor exact
   * reduction.  */
  simplifies_to("floor(cos(1000))", "floor(cos(1000))");
  simplifies_to("sin(frac(frac(exp(6))))", "sin(-403+e^(6))");
}
