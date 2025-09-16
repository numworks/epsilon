#include "helper.h"

using namespace Poincare;

QUIZ_CASE(pcj_approximation_float) {
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(-1.23456789E30), "-1.23456789ᴇ30");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(1.23456789E30), "1.23456789ᴇ30");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(-1.23456789E-30), "-1.23456789ᴇ-30");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(-1.2345E-3), "-0.0012345");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(1.2345E-3), "0.0012345");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(1.2345E3), "1234.5");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(-1.2345E3), "-1234.5");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(0.99999999999995), "0.99999999999995");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(0.00000099999999999995),
      "9.9999999999995ᴇ-7");
  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(
          0.0000009999999999901200121020102010201201201021099995),
      "9.9999999999012ᴇ-7");
  assert_float_approximates_to<float>(UserExpression::Builder<float>(1.2345E-1),
                                      "0.12345");
  assert_float_approximates_to<float>(UserExpression::Builder<float>(1), "1");
  assert_float_approximates_to<float>(
      UserExpression::Builder<float>(0.9999999999999995), "1");
  assert_float_approximates_to<float>(UserExpression::Builder<float>(1.2345E6),
                                      "1234500");
  assert_float_approximates_to<float>(UserExpression::Builder<float>(-1.2345E6),
                                      "-1234500");
  assert_float_approximates_to<float>(
      UserExpression::Builder<float>(0.0000009999999999999995), "1ᴇ-6");
  assert_float_approximates_to<float>(
      UserExpression::Builder<float>(-1.2345E-1), "-0.12345");

  assert_float_approximates_to<double>(
      UserExpression::Builder<double>(INFINITY), "∞");
  assert_float_approximates_to<float>(UserExpression::Builder<float>(0.0f),
                                      "0");
  assert_float_approximates_to<float>(UserExpression::Builder<float>(NAN),
                                      "undef");
}

QUIZ_CASE(pcj_approximation_rational) {
  approximates_to<float>("1/3", 0.3333333f);
  approximates_to<double>("123456/1234567", 9.9999432999586E-2);

  approximates_to<float>("-2-3", "-5");
  approximates_to<float>("5-2/3", "4.333333");
  approximates_to<double>("2/3-5", "-4.3333333333333");
  approximates_to<float>("-2/3-5", "-5.666667");
  approximates_to<float>("4/2×(2+3)", "10");
  approximates_to<double>("4/2×(2+3)", "10");
}

QUIZ_CASE(pcj_approximation_based_integer) {
  approximates_to<float>("1232", "1232");
  approximates_to<double>("0b110101", "53");
  approximates_to<double>("0xabc1234", "180097588");
}

QUIZ_CASE(pcj_approximation_decimal) {
  approximates_to<float>("-0", "0");
  approximates_to<float>("-0.1", "-0.1");
  approximates_to<float>("-1.", "-1");
  approximates_to<float>("-.1", "-0.1");
  approximates_to<float>("-0ᴇ2", "0");
  approximates_to<float>("-0.1ᴇ2", "-10");
  approximates_to<float>("-1.ᴇ2", "-100");
  approximates_to<float>("-.1ᴇ2", "-10");
  approximates_to<float>("-0ᴇ-2", "0");
  approximates_to<float>("-0.1ᴇ-2", "-0.001");
  approximates_to<float>("-1.ᴇ-2", "-0.01");
  approximates_to<float>("-.1ᴇ-2", "-0.001");
  approximates_to<float>("-.003", "-0.003");
  approximates_to<float>("1.2343ᴇ-2", "0.012343");
  approximates_to<double>("-567.2ᴇ2", "-56720");

  approximates_to(283495231345_e, 283495231345.f);
  approximates_to(283495231345._e, 283495231345.f);
  approximates_to(283495231345.0_e, 283495231345.f);
  approximates_to(283495231345.00_e, 283495231345.f);

  approximates_to(0.028349523100_e, 0.0283495231f);
  approximates_to(0.28349523100_e, 0.283495231f);
  approximates_to(2.8349523100_e, 2.83495231f);
  approximates_to(283495231.00_e, 283495231.f);
  approximates_to(2834952310.0_e, 2834952310.f);
  approximates_to(28349523100_e, 28349523100.f);

  approximates_to<float>("-0", 0.0f);
  approximates_to<float>("-1.ᴇ-2", -0.01f);
  approximates_to<double>("-.003", -0.003);
  approximates_to<float>("1.2343ᴇ-2", 0.012343f);
  approximates_to<double>("-567.2ᴇ2", -56720.0);

  approximates_to(123_e, 123.f);
  approximates_to(-123.21_fe, -123.21f);
}

QUIZ_CASE(pcj_approximation_infinity) {
  approximates_to<float>("inf", "∞");
  approximates_to<float>("inf*1", "∞");
  approximates_to<float>("inf(-1)", "-∞");
  approximates_to<float>("inf/1", "∞");
  approximates_to<float>("inf/(-1)", "-∞");
  approximates_to<float>("-inf+1", "-∞");
  approximates_to<float>("inf-inf", "undef");
  approximates_to<float>("-inf+inf", "undef");
  approximates_to<float>("inf*(-π)", "-∞");
  approximates_to<float>("inf*2*inf", "∞");
  approximates_to<float>("0×inf", "undef");
  approximates_to<float>("3×inf", "∞");
  approximates_to<float>("-3×inf", "-∞");
  approximates_to<float>("inf×(-inf)", "-∞");
  approximates_to<float>("1/inf", "0");
  approximates_to<float>("0/inf", "0");
  approximates_to<double>("10^1000", INFINITY);
  approximates_to<double>("10^1000", "∞");
  approximates_to<double>("2*10^1000", "∞");
  approximates_to<double>("(10^1000)/2", "∞");
  approximates_to<double>("(∞)×(i)", "∞×i", k_cartesianCtx);
  approximates_to<double>("(inf×i)×(i)", "-∞", k_cartesianCtx);
  approximates_to<double>("(inf×i)×(2)", "∞×i", k_cartesianCtx);
  // (inf+i)×(2) = inf * 2 - 1 * 0 + i * (inf * 0 + 1 * 2), inf * 0 return NAN
  approximates_to<double>("(inf+i)×(2)", "undef", k_cartesianCtx);

  // x^inf
  approximates_to<float>("(-2)^inf", "undef");  // complex inf
  approximates_to<float>("(-2)^(-inf)", "0");
  approximates_to<float>("(-1)^inf", "undef");
  approximates_to<float>("(-1)^(-inf)", "undef");
  approximates_to<float>("(-0.3)^inf", "0");
  approximates_to<float>("(-0.3)^(-inf)", "undef");  // complex inf
  approximates_to<float>("0^inf", "0");
  approximates_to<float>("0^(-inf)", "undef");  // complex inf
  approximates_to<float>("0.3^inf", "0");
  approximates_to<float>("0.3^(-inf)", "∞");
  approximates_to<float>("1^inf", "undef");
  approximates_to<float>("1^(-inf)", "undef");
  approximates_to<float>("2^inf", "∞");
  approximates_to<float>("2^(-inf)", "0");
  approximates_to<double>("(-i)^inf", "undef", k_cartesianCtx);
  approximates_to<double>("(-i)^(-inf)", "undef", k_cartesianCtx);
  approximates_to<double>("i^inf", "undef", k_cartesianCtx);
  approximates_to<double>("i^(-inf)", "undef", k_cartesianCtx);
  approximates_to<double>("(3+4i)^inf", "undef", k_cartesianCtx);
  approximates_to<double>("(3+4i)^(-inf)", "0", k_cartesianCtx);

  // inf^x
  approximates_to<float>("inf^i", "undef");
  approximates_to<float>("inf^6", "∞");
  approximates_to<float>("(-inf)^6", "∞");
  approximates_to<float>("inf^7", "∞");
  approximates_to<float>("(-inf)^7", "-∞");
  approximates_to<float>("inf^-6", "0");
  approximates_to<float>("(-inf)^-6", "0");
  approximates_to<float>("inf^-7", "0");
  approximates_to<float>("(-inf)^-7", "0");
  approximates_to<float>("inf^0", "undef");
  approximates_to<float>("(-inf)^0", "undef");
  approximates_to<float>("inf^inf", "∞");
  approximates_to<float>("inf^(-inf)", "0");
  approximates_to<float>("(-inf)^inf", "undef");  // complex inf
  approximates_to<float>("(-inf)^(-inf)", "0");

  // functions
  approximates_to<float>("exp(inf)", "∞");
  approximates_to<float>("exp(-inf)", "0");
  approximates_to<float>("log(inf,0)", "undef", k_cartesianCtx);
  approximates_to<float>("log(-inf,0)", "undef", k_cartesianCtx);
  approximates_to<float>("log(inf,1)", "undef", k_cartesianCtx);
  approximates_to<float>("log(-inf,1)", "undef", k_cartesianCtx);
  approximates_to<float>("log(inf,0.3)", "-∞");
  approximates_to<float>("log(-inf,0.3)", "nonreal");
  approximates_to<float>("log(-inf,0.3)", "-∞-2.609355×i", k_cartesianCtx);
  approximates_to<float>("log(inf,3)", "∞");
  approximates_to<float>("log(-inf,3)", "nonreal");
  approximates_to<float>("log(-inf,3)", "∞+2.859601×i", k_cartesianCtx);
  approximates_to<float>("log(inf,-3)", "nonreal");
  approximates_to<float>("log(inf,-3)", "∞-∞×i", k_cartesianCtx);
  approximates_to<float>("log(0,inf)", "undef", k_cartesianCtx);
  approximates_to<float>("log(0,-inf)", "undef", k_cartesianCtx);
  approximates_to<float>("log(1,inf)", "0");
  approximates_to<float>("log(1,-inf)", "0");
  approximates_to<float>("log(0.3,inf)", "0");
  approximates_to<float>("log(0.3,-inf)", "0");
  approximates_to<float>("log(3,inf)", "0");
  approximates_to<float>("log(3,-inf)", "0");
  approximates_to<float>("log(inf,inf)", "undef", k_cartesianCtx);
  approximates_to<float>("log(-inf,inf)", "undef", k_cartesianCtx);
  approximates_to<float>("log(inf,-inf)", "undef", k_cartesianCtx);
  approximates_to<float>("log(-inf,-inf)", "undef", k_cartesianCtx);
  approximates_to<float>("ln(inf)", "∞");
  approximates_to<float>("ln(-inf)", "nonreal");
  approximates_to<float>("ln(-inf)", "∞+3.141593×i", k_cartesianCtx);
  approximates_to<float>("cos(inf)", "undef");
  approximates_to<float>("cos(-inf)", "undef", k_cartesianCtx);
  approximates_to<float>("sin(inf)", "undef", k_cartesianCtx);
  approximates_to<float>("sin(-inf)", "undef", k_cartesianCtx);
  approximates_to<float>("atan(inf)", "1.570796");
  approximates_to<float>("atan(-inf)", "-1.570796");

  // nonreal vs undef
  approximates_to<float>("nonreal", "nonreal");
  approximates_to<float>("√(-1)", "nonreal");
  approximates_to<float>("√(-1)+1/0", "undef");
  // TODO_PCJ: this test returns 0 with emscripten
  // approximates_to<float>("1/(√(-1)^2+1)", "nonreal");
  approximates_to<float>("{√(-1),1/0}", "{nonreal,undef}");
  approximates_to<float>("(√(-1),2)", "(nonreal,2)");
  approximates_to<float>("(1/0,2)", "(undef,2)");
  approximates_to<float>("[[√(-1),2]]", "[[nonreal,2]]");
}

QUIZ_CASE(pcj_approximation_constant) {
  approximates_to<double>("π", "3.1415926535898");
  approximates_to<float>("e", "2.718282");
  approximates_to<float>("i", NAN);
  approximates_to<double>("i", NAN);
  approximates_to<float>("_c", 299792458.0);
  approximates_to<float>("_e", 1.602176634e-19);
  approximates_to<float>("_G", 6.67430e-11);
  approximates_to<float>("_g0", 9.80665);
  approximates_to<float>("_k", 1.380649e-23);
  approximates_to<float>("_ke", 8.9875517873681764e9);
  approximates_to<float>("_me", 9.1093837015e-31);
  approximates_to<float>("_mn", 1.67492749804e-27);
  approximates_to<float>("_mp", 1.67262192369e-27);
  approximates_to<float>("_Na", 6.02214076e23);
  approximates_to<float>("_R", 8.31446261815324);
  approximates_to<float>("_ε0", 8.854187817e-12);
  approximates_to<float>("_μ0", 1.25663706212e-6);
  approximates_to<float>("_hplanck", 6.62607015e-34);

  approximates_to(π_e, M_PI);
  approximates_to(e_e, M_E);
}
